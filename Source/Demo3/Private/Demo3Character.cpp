// Copyright Epic Games, Inc. All Rights Reserved.

#include "Demo3Character.h"
#include "Demo3Projectile.h"
#include "Demo3PlayerState.h"
#include "Demo3GameMode.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FPSDemoPlayerController.h"
#include "Net/UnrealNetwork.h"


//////////////////////////////////////////////////////////////////////////
// ADemo3Character

ADemo3Character::ADemo3Character()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	// Create a mesh component that will be used when being viewed from a '3st person' view
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P"));
	Mesh3P->SetOwnerNoSee(true);
	Mesh3P->SetupAttachment(FirstPersonCameraComponent);
	// Create Combat Component
	WeaponSystemComponent = CreateDefaultSubobject<UWeaponSystemComponent>(TEXT("WeaponSystemComponent"));
}

void ADemo3Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	WeaponSystemComponent->SetPlayerController(Cast<AFPSDemoPlayerController>(Controller));

}

//////////////////////////////////////////////////////////////////////////// Input

void ADemo3Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADemo3Character::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADemo3Character::Look);

		// 客户端处理：禁用输入（只在拥有该角色的客户端执行）
		if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			EnableInput(PlayerController);
		}
	}
}


void ADemo3Character::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ADemo3Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ADemo3Character::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool ADemo3Character::GetHasRifle()
{
	return bHasRifle;
}

ADemo3PlayerState* ADemo3Character::GetDemo3PlayerState() const
{
	return Cast<ADemo3PlayerState>(GetPlayerState());
}

UWeaponSystemComponent* ADemo3Character::GetWeaponSystemComponent()
{
	return WeaponSystemComponent;
}

void ADemo3Character::TakeDamage(float DamageAmount)
{
	if (DamageAmount <= 0.0f)
	{
		return;
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		// 服务器权威逻辑
		ADemo3PlayerState* PS = GetDemo3PlayerState();
		if (PS == nullptr)
		{
			return;
		}

		// 如果已经死亡，不再处理伤害
		if (PS->GetIsDead())
		{
			return;
		}

		// 计算新血量
		float NewHealth = FMath::Max(0.0f, PS->GetCurrentHealth() - DamageAmount);
		PS->SetCurrentHealth(NewHealth);

		// 如果血量归零，触发死亡
		if (NewHealth <= 0.0f)
		{
			PS->SetCurrentHealth(0.0f);
			PS->SetIsDead(true);
			OnDeath();
		}
	}else if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		// TODO 预测逻辑
	}
}

void ADemo3Character::OnDeath()
{
	// 服务器端处理：禁用碰撞
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// 调用玩家死亡事件分发器
	if (UWorld* World = GetWorld())
	{
		if (ADemo3GameMode* GameMode = Cast<ADemo3GameMode>(World->GetAuthGameMode()))
		{
			APlayerController* PlayerController = Cast<APlayerController>(GetController());
			GameMode->OnPlayerDied.Broadcast(this, PlayerController);
		}
	}

	// 调用客户端RPC，自动发送到拥有该角色的客户端
	ClientOnDeath();
}

void ADemo3Character::ClientOnDeath_Implementation()
{
	// 客户端处理：禁用输入（只在拥有该角色的客户端执行）
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		DisableInput(PlayerController);
	}

	// TODO 死亡效果、复活逻辑（UI更新、音效、动画等）
}