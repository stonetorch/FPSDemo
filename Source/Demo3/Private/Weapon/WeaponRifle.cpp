// Fill out your copyright notice in the Description page of Project Settings.

#include "Demo3/Public/Weapon/WeaponRifle.h"
#include "Demo3/Public/Demo3Projectile.h"
#include "Weapon/WeaponTriggerSingle.h"
#include "Net/UnrealNetwork.h"
#include "Components/SkeletalMeshComponent.h"

AWeaponRifle::AWeaponRifle()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	if (WeaponMesh)
	{
		WeaponMesh->SetIsReplicated(false);
		RootComponent = WeaponMesh;
		// 默认隐藏 Mesh（装备时再显示）
		WeaponMesh->SetVisibility(false);
	}
	
	// 设置默认触发器为单发模式
	TriggerClass = UWeaponTriggerSingle::StaticClass();
}

void AWeaponRifle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AWeaponRifle, AmmoInClip);
}

void AWeaponRifle::SpawnProjectile(const FRotator& SpawnRotation)
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
	
	
	// MuzzleOffset 是相对于相机空间的偏移，需要转换为世界空间
	// 从角色位置加上旋转后的偏移量得到最终的枪口位置
	const FVector SpawnLocation = OwnerPawn->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
	
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


