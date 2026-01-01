// Fill out your copyright notice in the Description page of Project Settings.

#include "Demo3/Public/Weapon/WeaponSystemComponent.h"

#include "Demo3Character.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FPSDemoPlayerController.h"
#include "Demo3/Public/Weapon/WeaponBase.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/RecoilBase.h"

// Sets default values for this component's properties
UWeaponSystemComponent::UWeaponSystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.

	// 初始状态下不Tick，但允许后续启用
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	
	// 启用网络复制
	SetIsReplicatedByDefault(true);
	
	CurrentWeapon = nullptr;
	LastWeapon = nullptr;
}

void UWeaponSystemComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWeaponSystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	// 销毁系统内管理的所有武器

	// 权威：销毁当前手持武器、销毁背包中的所有武器
	if (GetOwnerRole() == ROLE_Authority)
	{
		if (CurrentWeapon)
		{
			CurrentWeapon->OnUnequipped();
			CurrentWeapon->Destroy();
		}
		for (AWeaponBase* Weapon : InventoryWeapons)
		{
			Weapon->Destroy();
		}
	}
	// 持有者客户端：调用Weapon::OnUnequipped以取消Action绑定
	// 其他客户端：调用Weapon::OnUnequipped销毁手持武器Mesh
	if (GetOwnerRole() <= ROLE_AutonomousProxy)
	{
		if (CurrentWeapon)
		{
			CurrentWeapon->OnUnequipped();
			CurrentWeapon->Destroy();
		}
	}
}

void UWeaponSystemComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 后坐力更新
	if (ActiveRecoilLogic) ActiveRecoilLogic->Update(DeltaTime);
}

void UWeaponSystemComponent::SetPlayerController(AFPSDemoPlayerController* PC)
{
	if (GetOwnerRole() >= ROLE_AutonomousProxy)
	{
		SetComponentTickEnabled(true); // enable tick
		PlayerController = PC;
	}
}

void UWeaponSystemComponent::AddWeapon_Implementation(AWeaponBase* weaponActor)
{
	// 将一把武器添加到当前玩家武器系统的库存中
	// - 修改weaponActor的父Actor
	// - 修改weaponActor网络同步的所属

	if (!weaponActor)
	{
		return;
	}

	// 检查武器是否已经在背包中，避免重复添加
	if (InventoryWeapons.Contains(weaponActor))
	{
		return;
	}

	// 获取组件拥有者（玩家角色）
	AActor* ComponentOwner = GetOwner();
	if (!ComponentOwner)
	{
		return;
	}
	
	// 修改weaponActor的父Actor（设置Owner）
	// 之后网络同步中，该Pawn对应的客户端就能控制这个weaponActor
	weaponActor->SetOwner(ComponentOwner);
	weaponActor->bOnlyRelevantToOwner = true;
	
	// 将武器添加到库存数组
	InventoryWeapons.Add(weaponActor);
	
	// 如果当前没有手持武器，自动装备第一把武器
	if (!CurrentWeapon && InventoryWeapons.Num() > 0)
	{
		SwitchWeapon(InventoryWeapons[0]);
	}
}


void UWeaponSystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// 背包武器数组：仅服务器与拥有者同步
	DOREPLIFETIME_CONDITION(UWeaponSystemComponent, InventoryWeapons, COND_OwnerOnly);
	
	// 当前手持武器：所有客户端同步
	DOREPLIFETIME(UWeaponSystemComponent, CurrentWeapon);
}

void UWeaponSystemComponent::EnableWeaponInput(AWeaponBase* Weapon)
{
	// 对于AutonomousProxy和Authority运行
	if (GetOwnerRole() < ROLE_AutonomousProxy)	return;
	APawn* OwnerPawn = GetOwner() ? Cast<APawn>(GetOwner()) : nullptr;
	if (!OwnerPawn)
	{
		return;
	}
	// Set up action bindings
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, Weapon, &AWeaponBase::Fire);
		}
	}
}

void UWeaponSystemComponent::DisableWeaponInput(AWeaponBase* Weapon)
{
	if (GetOwnerRole() != ROLE_AutonomousProxy)	return;
	APawn* OwnerPawn = GetOwner() ? Cast<APawn>(GetOwner()) : nullptr;
	if (!OwnerPawn)
	{
		return;
	}
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}

AWeaponBase* UWeaponSystemComponent::GetCurrentWeapon()
{
	return CurrentWeapon;
}

void UWeaponSystemComponent::OnRep_CurrentWeapon()
{
	// 当前手持武器发生变化时的回调
	// 该函数必须是"幂等"的，允许被多次调用而不产生副作用叠加
	
	// 获取旧的武器（在更新 LastWeapon 之前）
	AWeaponBase* OldWeapon = LastWeapon;
	
	// 如果上一把武器存在且与当前武器不同，卸下它
	if (OldWeapon && OldWeapon != CurrentWeapon)
	{
		OldWeapon->OnUnequipped();
		DisableWeaponInput(CurrentWeapon);
	}
	
	// 如果当前武器存在，装备它
	if (CurrentWeapon)
	{
		CurrentWeapon->OnEquipped();

		// 配置输入
		EnableWeaponInput(CurrentWeapon);

		// 激活后坐力组件
		ActiveRecoilLogic = CurrentWeapon->GetRecoil();
	}
	
	// 更新 LastWeapon 为当前的 CurrentWeapon（用于下次 OnRep 调用）
	LastWeapon = CurrentWeapon;
}

void UWeaponSystemComponent::TryFire()
{
	// 尝试开火（客户端入口）
	// 本地立即执行后坐力（通过 Weapon 提供的数据）
	// 调用 Weapon 的 ServerFire RPC
	
	if (!CurrentWeapon)
	{
		return;
	}
	
	// 调用武器的 Fire 方法，它会处理本地效果和后坐力，然后调用 ServerFire
	CurrentWeapon->Fire();
}

void UWeaponSystemComponent::SwitchWeapon(AWeaponBase* NewWeapon)
{
	/**
	* 网络语义：
	* - 客户端调用后，通过 RPC 请求服务器切换
	* - 服务器修改 CurrentWeapon 并触发复制
	*
	* 实现要点：
	* - 切换为手持时，需要将 Weapon 设置为“全客户端相关”
	* - 原手持武器切回背包时，需恢复为 Owner-only
	*/
	
    // 切换当前手持武器
	// 客户端调用后，通过 RPC 请求服务器切换
	if (!NewWeapon)
	{
		return;
	}
	
	// 检查武器是否在背包中
	if (!InventoryWeapons.Contains(NewWeapon))
	{
		return;
	}
	
	// 通过 RPC 请求服务器切换
	ServerSwitchWeapon(NewWeapon);
}

void UWeaponSystemComponent::SwitchToNextWeapon()
{
	// 找到当前武器在背包中的索引
	int32 CurrentIndex = InventoryWeapons.Find(CurrentWeapon);
	if (CurrentIndex == INDEX_NONE)
	{
		// 如果当前武器不在背包中，切换到第一把
		if (InventoryWeapons.Num() > 0)
		{
			SwitchWeapon(InventoryWeapons[0]);
		}
		return;
	}
	
	// 切换到下一把（循环）
	int32 NextIndex = (CurrentIndex + 1) % InventoryWeapons.Num();
	SwitchWeapon(InventoryWeapons[NextIndex]);
}

void UWeaponSystemComponent::SwitchToPreviousWeapon()
{
	// 找到当前武器在背包中的索引
	int32 CurrentIndex = InventoryWeapons.Find(CurrentWeapon);
	if (CurrentIndex == INDEX_NONE)
	{
		// 如果当前武器不在背包中，切换到第一把
		if (InventoryWeapons.Num() > 0)
		{
			SwitchWeapon(InventoryWeapons[0]);
		}
		return;
	}
	
	// 切换到上一把（循环）
	int32 PreviousIndex = (CurrentIndex - 1 + InventoryWeapons.Num()) % InventoryWeapons.Num();
	SwitchWeapon(InventoryWeapons[PreviousIndex]);
}

void UWeaponSystemComponent::ServerSwitchWeapon_Implementation(AWeaponBase* NewWeapon)
{
	/*
	* - 校验 NewWeapon 是否属于该玩家
	* - 修改 CurrentWeapon
	* - 调整 Weapon 的网络可见性
	*/

	// 服务器端切换武器的 RPC
	// 校验 NewWeapon 是否属于该玩家
	if (!NewWeapon)
	{
		return;
	}
	
	// 检查武器是否在背包中
	if (!InventoryWeapons.Contains(NewWeapon))
	{
		return;
	}
	
	// 检查武器的拥有者是否匹配
	AActor* ComponentOwner = GetOwner();
	if (!ComponentOwner || NewWeapon->GetOwner() != ComponentOwner)
	{
		return;
	}
	
	// 如果切换的是同一把武器，直接返回
	if (CurrentWeapon == NewWeapon)
	{
		return;
	}
	
	// 保存上一把武器到成员变量（用于 OnRep 回调）
	LastWeapon = CurrentWeapon;
	
	// 调整 Weapon 的网络可见性
	// 上一把武器切回背包时，需恢复为 Owner-only
	if (LastWeapon)
	{
		LastWeapon->SetOwner(ComponentOwner);
		LastWeapon->bOnlyRelevantToOwner = true;
	}
	
	// 新武器装备时，设置为全客户端相关
	NewWeapon->SetOwner(ComponentOwner);
	NewWeapon->bOnlyRelevantToOwner = false;
	
	// 修改 CurrentWeapon（这会触发网络复制和 OnRep_CurrentWeapon）
	CurrentWeapon = NewWeapon;
	
	// OnRep_CurrentWeapon 会在复制时自动调用
	// 但为了确保服务器端也执行，手动调用一次
	OnRep_CurrentWeapon();
}
