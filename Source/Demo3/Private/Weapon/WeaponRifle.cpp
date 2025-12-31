// Fill out your copyright notice in the Description page of Project Settings.

#include "Demo3/Public/Weapon/WeaponRifle.h"
#include "Demo3Character.h"
#include "Demo3/Public/Demo3Projectile.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

AWeaponRifle::AWeaponRifle()
{
	// 初始化弹药
	AmmoInClip = 100;
	
	// 创建武器网格体组件
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
	
	// 默认隐藏 Mesh（装备时再显示）
	WeaponMesh->SetVisibility(false);
	
	// 初始化默认值
	ProjectileClass = nullptr;
	FireSound = nullptr;
	FireAnimation = nullptr;
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}

void AWeaponRifle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AWeaponRifle, AmmoInClip);
}

void AWeaponRifle::SetupWeaponMesh()
{
	// 获取拥有者
	APawn* OwnerPawn = GetOwner() ? Cast<APawn>(GetOwner()) : nullptr;
	if (!OwnerPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("AWeaponRifle::SetupWeaponMesh: OwnerPawn is nullptr"));
		return;
	}
	
	ADemo3Character* Character = Cast<ADemo3Character>(OwnerPawn);
	if (!Character)
	{
		UE_LOG(LogTemp, Error, TEXT("AWeaponRifle::SetupWeaponMesh: OwnerPawn is not a Demo3Character"));
		return;
	}
	
	// 判断是否为本地控制
	bool bIsLocallyControlled = OwnerPawn->IsLocallyControlled() && OwnerPawn->IsPlayerControlled();
	
	// 显示武器 Mesh
	if (WeaponMesh)
	{
		WeaponMesh->SetVisibility(true);
		
		// 根据是否为本地控制，附加到不同的骨骼
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
		
		if (bIsLocallyControlled)
		{
			// 本地：附加到第一人称手臂 Mesh 的 GripPoint 插槽
			if (Character->GetMesh1P())
			{
				WeaponMesh->AttachToComponent(
					Character->GetMesh1P(),
					AttachmentRules,
					FName(TEXT("GripPoint"))
				);
				// 第一人称武器只对拥有者可见
				WeaponMesh->SetOnlyOwnerSee(true);
				WeaponMesh->SetOwnerNoSee(false);
			}
		}
		else
		{
			// 远端：附加到第三人称身体 Mesh 的 hand_r 插槽（或其他合适的插槽）
			if (Character->GetMesh3P())
			{
				WeaponMesh->AttachToComponent(
					Character->GetMesh3P(),
					AttachmentRules,
					FName(TEXT("Weapon_R")) 
				);
				// 第三人称武器对其他人可见，但拥有者不可见（因为拥有者看第一人称）
				WeaponMesh->SetOnlyOwnerSee(false);
				WeaponMesh->SetOwnerNoSee(true);
			}
		}
	}
}

void AWeaponRifle::DetachWeaponMesh()
{
	// 分离武器 Mesh
	if (WeaponMesh)
	{
		WeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		// 隐藏 Mesh
		WeaponMesh->SetVisibility(false);
	}
}

bool AWeaponRifle::CanFire() const
{
	// 检查弹药
	return AmmoInClip > 0;
}

void AWeaponRifle::ConsumeAmmo()
{
	// 扣除弹药
	if (AmmoInClip > 0)
	{
		AmmoInClip--;
	}
}

void AWeaponRifle::SpawnProjectile()
{
	// 生成子弹
	if (ProjectileClass == nullptr)
	{
		return;
	}
	
	APawn* OwnerPawn = GetOwner() ? Cast<APawn>(GetOwner()) : nullptr;
	if (!OwnerPawn || !OwnerPawn->GetController())
	{
		return;
	}
	
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
	
	APlayerController* PlayerController = Cast<APlayerController>(OwnerPawn->GetController());
	if (PlayerController == nullptr || PlayerController->PlayerCameraManager == nullptr)
	{
		return;
	}
	
	// 获取相机旋转（射击方向）
	const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
	
	// MuzzleOffset 是相对于相机空间的偏移，需要转换为世界空间
	// 从角色位置加上旋转后的偏移量得到最终的枪口位置
	const FVector SpawnLocation = OwnerPawn->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
	
	// 设置生成参数
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	ActorSpawnParams.Owner = OwnerPawn;
	ActorSpawnParams.Instigator = OwnerPawn;
	
	// 在枪口位置生成投射物
	World->SpawnActor<ADemo3Projectile>(
		ProjectileClass,
		SpawnLocation,
		SpawnRotation,
		ActorSpawnParams
	);
}

void AWeaponRifle::PlayFireEffectsLocal()
{
	// 播放本地开火效果（音效、动画等）
	APawn* OwnerPawn = GetOwner() ? Cast<APawn>(GetOwner()) : nullptr;
	if (!OwnerPawn)
	{
		return;
	}
	
	ADemo3Character* Character = Cast<ADemo3Character>(OwnerPawn);
	if (!Character)
	{
		return;
	}
	
	// 播放开火动画
	if (FireAnimation != nullptr)
	{
		// 获取第一人称手臂的动画实例
		if (Character->GetMesh1P())
		{
			UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
			if (AnimInstance != nullptr)
			{
				AnimInstance->Montage_Play(FireAnimation, 1.0f);
			}
		}
	}
	
	// 播放开火音效
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			FireSound,
			Character->GetActorLocation()
		);
	}
}

void AWeaponRifle::PlayFireEffectsMulticast()
{
	// 播放多播开火效果（音效等）
	APawn* OwnerPawn = GetOwner() ? Cast<APawn>(GetOwner()) : nullptr;
	if (!OwnerPawn)
	{
		return;
	}
	
	ADemo3Character* Character = Cast<ADemo3Character>(OwnerPawn);
	if (!Character)
	{
		return;
	}
	
	// 播放开火音效
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			FireSound,
			Character->GetActorLocation()
		);
	}
}

