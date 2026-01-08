// Fill out your copyright notice in the Description page of Project Settings.

#include "Demo3/Public/Weapon/WeaponSMG.h"
#include "Demo3Character.h"
#include "Demo3/Public/Demo3Projectile.h"
#include "Weapon/WeaponTriggerAuto.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/DamageEvents.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

AWeaponSMG::AWeaponSMG()
{
	DamageAmount = 10.0f;
	BeamEffect = nullptr;
	TracerFrequency = 1;
	FireCount = 0;
	MaxRange = 10000;

	// 设置默认触发器为自动连续射击模式
	TriggerClass = UWeaponTriggerAuto::StaticClass();
}

void AWeaponSMG::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AWeaponSMG, AmmoInClip);
}

void AWeaponSMG::SpawnProjectile(const FRotator& SpawnRotation)
{
	// 使用射线检测完成伤害（仅在服务端执行）
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
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	
	const FVector StartLocation = OwnerPawn->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
	
	// 射线方向
	const FVector ForwardVector = SpawnRotation.Vector();
	MaxRange = 10000.0f;
	const FVector EndLocation = StartLocation + ForwardVector * MaxRange;
	
	// 射线检测参数
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerPawn);
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnPhysicalMaterial = false;
	
	// 执行射线检测
	FHitResult HitResult;
	bool bHit = World->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECC_Pawn, // 碰撞通道，可以根据需要调整
		QueryParams
	);
	
	// 计算实际的结束位置（命中点或最大射程点）
	FVector ActualEndLocation = bHit ? HitResult.ImpactPoint : EndLocation;
	
	// 如果命中目标，应用伤害
	if (bHit && HitResult.GetActor())
	{
		AActor* HitActor = HitResult.GetActor();
		
		// 检查是否命中角色
		if (ADemo3Character* HitCharacter = Cast<ADemo3Character>(HitActor))
		{
			// 不会伤害自己
			if (HitCharacter != OwnerPawn)
			{
				UGameplayStatics::ApplyDamage(
					HitCharacter,
					DamageAmount,
					OwnerPawn->GetController(),
					this,
					UDamageType::StaticClass()
				);
			}
		}
	}
	
	// Tracer 方案：只在达到频率时才显示射线效果
	FireCount++;
	if (FireCount >= TracerFrequency)
	{
		FireCount = 0; // 重置计数器
		// 在所有客户端显示射线效果
		NetMulticastSpawnBeamEffect(StartLocation, ActualEndLocation);
	}
}

void AWeaponSMG::PlayFireEffectsLocal()
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

void AWeaponSMG::PlayFireEffectsMulticast()
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

void AWeaponSMG::SpawnProjectileAimingAt(const FVector& TargetLocation)
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
	
	// 生成投射物
	SpawnProjectile(AimRotator);
}

void AWeaponSMG::NetMulticastSpawnBeamEffect_Implementation(const FVector& StartLocation, const FVector& EndLocation)
{
	// 在所有客户端显示射线效果
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	
	if (BeamEffect)
	{
		// 计算光束的方向
		FVector Direction = (EndLocation - StartLocation).GetSafeNormal();
		
		// 生成 Niagara 粒子系统（在起始位置）
		UNiagaraComponent* BeamComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			World,
			BeamEffect,
			StartLocation,
			Direction.Rotation()
		);
		
		if (BeamComponent)
		{
			BeamComponent->SetVariableVec3(FName("User.BeamEnd"), EndLocation);
		}
	}
}
