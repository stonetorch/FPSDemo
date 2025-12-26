// Fill out your copyright notice in the Description page of Project Settings.

#include "Demo3/Public/Weapon/WeaponBase.h"
#include "Demo3Character.h"
#include "Demo3/Public/Demo3Projectile.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// 默认设置为仅对拥有者相关（背包武器）
	bOnlyRelevantToOwner = true;
	
	// 启用网络复制
	bReplicates = true;
	
	RecoilLogic = nullptr;
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AWeaponBase::OnEquipped()
{
	/*
	* - 根据是否为本地控制：
	*   - 本地：生成 1P Mesh
	*   - 远端：生成 3P Mesh
	* - 初始化动画状态
	* - 启用本地后坐力（仅本地）
	* - 绑定射击事件
	*/
	
	// 装备武器时，设置为对所有客户端可见
	bOnlyRelevantToOwner = false;
	
	// 获取拥有者
	APawn* OwnerPawn = GetOwner() ? Cast<APawn>(GetOwner()) : nullptr;
	if (!OwnerPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("AWeaponBase::OnEquipped: OwnerPawn is nullptr"));
		return;
	}
	
	ADemo3Character* Character = Cast<ADemo3Character>(OwnerPawn);
	if (!Character)
	{
		UE_LOG(LogTemp, Error, TEXT("AWeaponBase::OnEquipped: OwnerPawn is not a Demo3Character"));
		return;
	}
	
	// 更新角色的武器状态（用于动画蓝图）
	Character->SetHasRifle(true);
	
	// 设置武器网格体
	SetupWeaponMesh();
	
	// 启用本地后坐力（仅本地）
	bool bIsLocallyControlled = OwnerPawn->IsLocallyControlled();
	if (bIsLocallyControlled && RecoilLogic)
	{
		// 启用后坐力逻辑
		// 注意：具体启用逻辑需要根据 URecoilBase 的实现来实现
	}
}

void AWeaponBase::OnUnequipped()
{
	// 卸下武器时，恢复为仅对拥有者相关
	bOnlyRelevantToOwner = true;
	
	// 获取拥有者
	APawn* OwnerPawn = GetOwner() ? Cast<APawn>(GetOwner()) : nullptr;
	if (OwnerPawn)
	{
		// 转换为 Demo3Character
		ADemo3Character* Character = Cast<ADemo3Character>(OwnerPawn);
		if (Character)
		{
			// 更新角色的武器状态（用于动画蓝图）
			Character->SetHasRifle(false);
		}
	}
	
	// 分离武器网格体（由子类实现）
	DetachWeaponMesh();
	
	// 禁用后坐力
	if (RecoilLogic)
	{
		// 禁用后坐力逻辑
		// 注意：具体禁用逻辑需要根据 URecoilBase 的实现来实现
	}
}

void AWeaponBase::Fire()
{
	// 开火逻辑的入口，由PlayerController调用
	APawn* OwnerPawn = GetOwner() ? Cast<APawn>(GetOwner()) : nullptr;
	if (!OwnerPawn)
	{
		return;
	}
	// 1. 播放本地效果
	PlayFireEffectsLocal();

	// 2. 调用后坐力组件
	if (OwnerPawn && OwnerPawn->IsLocallyControlled() && RecoilLogic)
	{
		// 应用后坐力
		// 注意：具体后坐力应用逻辑需要根据 URecoilBase 的实现来实现
	}
	
	// 3. 使用ServerRPC实现生成子弹的逻辑等
	ServerFire();
}

void AWeaponBase::ServerFire_Implementation()
{
	// 服务器权威的射击 RPC
	// 校验弹药
	if (!CanFire())
	{
		return;
	}
	
	APawn* OwnerPawn = GetOwner() ? Cast<APawn>(GetOwner()) : nullptr;
	if (!OwnerPawn || !OwnerPawn->GetController())
	{
		return;
	}
	
	// 扣除弹药
	ConsumeAmmo();
	
	// 生成子弹
	SpawnProjectile();
	
	// 向所有客户端广播开火效果（音效、动画等）
	NetMulticastFire();
}

void AWeaponBase::NetMulticastFire_Implementation()
{
	// 网络多播：在所有客户端播放开火特效
	// 只在其他客户端上执行
	if (GetLocalRole() != ROLE_SimulatedProxy) return;
	
	// 播放开火效果
	PlayFireEffectsMulticast();
}

void AWeaponBase::SetupWeaponMesh()
{
	// 默认实现为空，由子类实现
}

void AWeaponBase::DetachWeaponMesh()
{
	// 默认实现为空，由子类实现
}

bool AWeaponBase::CanFire() const
{
	// 默认实现：总是返回 true，由子类实现具体逻辑
	return true;
}

void AWeaponBase::ConsumeAmmo()
{
	// 默认实现为空，由子类实现
}

void AWeaponBase::SpawnProjectile()
{
	// 默认实现为空，由子类实现
}

void AWeaponBase::PlayFireEffectsLocal()
{
	// 默认实现为空，由子类实现
}

void AWeaponBase::PlayFireEffectsMulticast()
{
	// 默认实现为空，由子类实现
}

