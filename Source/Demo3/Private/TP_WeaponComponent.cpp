// Copyright Epic Games, Inc. All Rights Reserved.


#include "../Public/TP_WeaponComponent.h"
#include "Demo3Character.h"
#include "../Public/Demo3Projectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
	SetIsReplicatedByDefault(true);
}


void UTP_WeaponComponent::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	// 调用服务器RPC - 将在服务器上执行（如果从服务器调用则直接执行，如果从客户端调用则通过RPC执行）
	ServerFire();
}

void UTP_WeaponComponent::ServerFire_Implementation()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	// 在服务器上尝试生成投射物
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			if (PlayerController != nullptr && PlayerController->PlayerCameraManager != nullptr)
			{
				const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
		
				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
				ActorSpawnParams.Owner = GetOwner();
		
				// Spawn the projectile at the muzzle
				World->SpawnActor<ADemo3Projectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			}
		}
	}

	// 向所有客户端广播开火效果
	NetMulticastFire();
}

bool UTP_WeaponComponent::ServerFire_Validate()
{
	return true;
}

void UTP_WeaponComponent::NetMulticastFire_Implementation()
{
	if (Character == nullptr)
	{
		return;
	}
	
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void UTP_WeaponComponent::AttachWeapon(ADemo3Character* TargetCharacter)
{
	Character = TargetCharacter;
	if (Character == nullptr)
	{
		return;
	}

	EquipMesh(TargetCharacter);
	CompleteEquip(TargetCharacter);
}

void UTP_WeaponComponent::EquipMesh(ADemo3Character* TargetCharacter)
{
	if (TargetCharacter == nullptr)
	{
		return;
	}

	Character = TargetCharacter;

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(TargetCharacter->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
	
	// switch bHasRifle so the animation blueprint can switch to another animation set
	TargetCharacter->SetHasRifle(true);
}

void UTP_WeaponComponent::UnequipMesh(ADemo3Character* TargetCharacter)
{
	if (TargetCharacter == nullptr)
	{
		return;
	}

	// Detach the weapon
	DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	
	// switch bHasRifle so the animation blueprint can switch back
	TargetCharacter->SetHasRifle(false);
}

void UTP_WeaponComponent::CompleteEquip(ADemo3Character* TargetCharacter)
{
	if (TargetCharacter == nullptr)
	{
		return;
	}

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(TargetCharacter->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Fire);
		}
	}
}

void UTP_WeaponComponent::CompleteUnequip(ADemo3Character* TargetCharacter)
{
	if (TargetCharacter == nullptr)
	{
		return;
	}

	// Remove action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(TargetCharacter->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}