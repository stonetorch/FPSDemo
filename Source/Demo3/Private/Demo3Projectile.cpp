// Copyright Epic Games, Inc. All Rights Reserved.

#include "../Public/Demo3Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Demo3Character.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"

ADemo3Projectile::ADemo3Projectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ADemo3Projectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	// 默认伤害值
	DamageAmount = 10.0f;
}

void ADemo3Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == nullptr || OtherActor == this)
	{
		return;
	}

	if (ADemo3Character* HitCharacter = Cast<ADemo3Character>(OtherActor))
	{
		if (HitCharacter != GetOwner())
		{
			// 使用标准伤害系统，会自动调用 TakeDamage
			UGameplayStatics::ApplyDamage(HitCharacter, DamageAmount, GetInstigatorController(), this, UDamageType::StaticClass());
		}
	}

	// 销毁投射物
	Destroy();
}