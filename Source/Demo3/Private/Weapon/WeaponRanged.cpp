#include "Demo3/Public/Weapon/WeaponRanged.h"
#include "Demo3Character.h"
#include "Demo3/Public/Demo3Projectile.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/ProjectileMovementComponent.h"

AWeaponRanged::AWeaponRanged()
{
	// 初始化弹药
	AmmoInClip = 100;
	
	// 初始化默认值
	ProjectileClass = nullptr;
	FireSound = nullptr;
	FireAnimation = nullptr;
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}

void AWeaponRanged::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AWeaponRanged, AmmoInClip);
}

void AWeaponRanged::SetupWeaponMesh()
{
	// 获取拥有者
	APawn* OwnerPawn = GetOwner() ? Cast<APawn>(GetOwner()) : nullptr;
	if (!OwnerPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("AWeaponRanged::SetupWeaponMesh: OwnerPawn is nullptr"));
		return;
	}
	
	ADemo3Character* Character = Cast<ADemo3Character>(OwnerPawn);
	if (!Character)
	{
		UE_LOG(LogTemp, Error, TEXT("AWeaponRanged::SetupWeaponMesh: OwnerPawn is not a Demo3Character"));
		return;
	}
	
	// 判断是否为本地控制
	bool bIsLocallyControlled = OwnerPawn->IsLocallyControlled() && OwnerPawn->IsPlayerControlled();

	if (GetLocalRole() == ROLE_Authority) return;
	
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
			}
		}
		else
		{
			// 远端：附加到第三人称身体 Mesh 的 Weapon_R 插槽
			if (Character->GetMesh3P())
			{
				WeaponMesh->AttachToComponent(
					Character->GetMesh3P(),
					AttachmentRules,
					FName(TEXT("Weapon_R")) 
				);
			}
		}
	}
}

void AWeaponRanged::DetachWeaponMesh()
{
	if (GetLocalRole() == ROLE_Authority) return;
	// 分离武器 Mesh
	if (WeaponMesh)
	{
		WeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		// 隐藏 Mesh
		WeaponMesh->SetVisibility(false);
	}
}

bool AWeaponRanged::CanFire() const
{
	// 检查弹药
	return AmmoInClip > 0;
}

void AWeaponRanged::ConsumeAmmo()
{
	// 扣除弹药
	if (AmmoInClip > 0)
	{
		AmmoInClip--;
	}
}

void AWeaponRanged::PlayFireEffectsLocal()
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

void AWeaponRanged::PlayFireEffectsMulticast()
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

void AWeaponRanged::SpawnProjectileAimingAt(const FVector& TargetLocation)
{
	// 计算弹道方向并开火
	APawn* OwnerPawn = Cast<APawn>(Owner);
	if (!OwnerPawn)
	{
		return;
	}
	
	// 获取投射物速度
	float ProjectileSpeed = 3000.0f; // 默认速度
	if (ProjectileClass)
	{
		ADemo3Projectile* ProjectileCDO = ProjectileClass->GetDefaultObject<ADemo3Projectile>();
		if (ProjectileCDO && ProjectileCDO->GetProjectileMovement())
		{
			ProjectileSpeed = ProjectileCDO->GetProjectileMovement()->InitialSpeed;
		}
	}
	
	// 计算初始发射位置（使用从角色位置到目标的方向估算）
	FVector OwnerLocation = OwnerPawn->GetActorLocation();
	FVector DirectionToTarget = (TargetLocation - OwnerLocation).GetSafeNormal();
	FRotator InitialDirection = DirectionToTarget.Rotation();
	FVector LaunchLocation = OwnerLocation + InitialDirection.RotateVector(MuzzleOffset);
	
	// 计算能够命中目标的弹道方向
	FVector LaunchVelocity;
	FRotator AimRotator;
	
	// 使用 UE 的弹道预测函数计算考虑重力的弹道
	if (UGameplayStatics::SuggestProjectileVelocity(
		GetWorld(),
		LaunchVelocity,
		LaunchLocation,
		TargetLocation,
		ProjectileSpeed,
		false,
		0.0f,
		0.0f,
		ESuggestProjVelocityTraceOption::DoNotTrace))
	{
		// 成功计算出速度向量，转换为 Rotator（全局坐标系）
		AimRotator = LaunchVelocity.Rotation();
	}
	else
	{
		// 如果无法计算（例如目标太远或无法到达），使用直接瞄准
		FVector FinalDirectionToTarget = (TargetLocation - LaunchLocation).GetSafeNormal();
		AimRotator = FinalDirectionToTarget.Rotation();
	}
	
	// 生成投射物（子类实现具体逻辑）
	SpawnProjectile(AimRotator);
}
