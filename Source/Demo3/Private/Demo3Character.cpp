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
#include "Kismet/GameplayStatics.h"


//////////////////////////////////////////////////////////////////////////
// ADemo3Character

ADemo3Character::ADemo3Character()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;

	// 初始化生命值
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
	bIsDead = false;

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

void ADemo3Character::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	WeaponSystemComponent->SetPlayerController(Cast<AFPSDemoPlayerController>(NewController));
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

		//Weapon Switching
		EnhancedInputComponent->BindAction(IA_SwtichToWeaponSlot1, ETriggerEvent::Triggered, this, &ADemo3Character::SwitchToWeaponSlot1);
		EnhancedInputComponent->BindAction(IA_SwtichToWeaponSlot2, ETriggerEvent::Triggered, this, &ADemo3Character::SwitchToWeaponSlot2);
		EnhancedInputComponent->BindAction(IA_SwtichToWeaponSlot3, ETriggerEvent::Triggered, this, &ADemo3Character::SwitchToWeaponSlot3);
		EnhancedInputComponent->BindAction(IA_SwtichToNextWeapon, ETriggerEvent::Triggered, this, &ADemo3Character::SwitchToNextWeapon);
		EnhancedInputComponent->BindAction(IA_SwtichToPreviousWeapon, ETriggerEvent::Triggered, this, &ADemo3Character::SwitchToPreviousWeapon);

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
		// 获取鼠标灵敏度
		float HorizontalSensitivity = 1.0f;
		float VerticalSensitivity = 1.0f;
		
		if (AFPSDemoPlayerController* FPSController = Cast<AFPSDemoPlayerController>(Controller))
		{
			HorizontalSensitivity = FPSController->GetMouseHorizontalSensitivity();
			VerticalSensitivity = FPSController->GetMouseVerticalSensitivity();
		}

		// add yaw and pitch input to controller with sensitivity
		AddControllerYawInput(LookAxisVector.X * HorizontalSensitivity);
		AddControllerPitchInput(LookAxisVector.Y * VerticalSensitivity);
	}
}

void ADemo3Character::SwitchToWeaponSlot1()
{
	if (WeaponSystemComponent && WeaponSystemComponent->InventoryWeapons.Num() > 0)
	{
		WeaponSystemComponent->SwitchWeapon(WeaponSystemComponent->InventoryWeapons[0]);
	}
}

void ADemo3Character::SwitchToWeaponSlot2()
{
	if (WeaponSystemComponent && WeaponSystemComponent->InventoryWeapons.Num() > 1)
	{
		WeaponSystemComponent->SwitchWeapon(WeaponSystemComponent->InventoryWeapons[1]);
	}
}

void ADemo3Character::SwitchToWeaponSlot3()
{
	if (WeaponSystemComponent && WeaponSystemComponent->InventoryWeapons.Num() > 2)
	{
		WeaponSystemComponent->SwitchWeapon(WeaponSystemComponent->InventoryWeapons[2]);
	}
}

void ADemo3Character::SwitchToNextWeapon()
{
	if (WeaponSystemComponent)
	{
		WeaponSystemComponent->SwitchToNextWeapon();
	}
}

void ADemo3Character::SwitchToPreviousWeapon()
{
	if (WeaponSystemComponent)
	{
		WeaponSystemComponent->SwitchToPreviousWeapon();
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

void ADemo3Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADemo3Character, MaxHealth);
	DOREPLIFETIME(ADemo3Character, CurrentHealth);
	DOREPLIFETIME(ADemo3Character, bIsDead);
}

float ADemo3Character::GetHealthPercent() const
{
	if (MaxHealth > 0.0f)
	{
		return CurrentHealth / MaxHealth;
	}
	return 0.0f;
}

float ADemo3Character::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                                  class AController* EventInstigator, AActor* DamageCauser)
{
	// 只在服务器处理伤害逻辑
	if (GetLocalRole() != ROLE_Authority)
	{
		return 0.0f;
	}

	if (DamageAmount <= 0.0f || bIsDead)
	{
		return 0.0f;
	}

	// 计算实际受到的伤害
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 计算受伤方向（从自身指向伤害源的单位向量）
	FVector DamageDirection = FVector::ZeroVector;
	if (DamageCauser != nullptr)
	{
		FVector MyLocation = GetActorLocation();
		FVector DamageSourceLocation = DamageCauser->GetActorLocation();
		DamageDirection = (DamageSourceLocation - MyLocation).GetSafeNormal();
	}

	// 计算新血量
	float NewHealth = FMath::Max(0.0f, CurrentHealth - ActualDamage);
	CurrentHealth = NewHealth;

	// 广播生命值更新事件（服务器端）
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, GetHealthPercent());

	// 使用RPC通知客户端受到伤害
	ClientOnDamaged(ActualDamage, DamageDirection);

	// 如果血量归零，触发死亡
	if (NewHealth <= 0.0f)
	{
		CurrentHealth = 0.0f;
		bIsDead = true;
		OnDeath();
	}

	return ActualDamage;
}

void ADemo3Character::OnRep_CurrentHealth()
{
	// 当血量在客户端同步时调用
	// 广播生命值更新事件，用于UI更新等
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, GetHealthPercent());
}

void ADemo3Character::OnRep_IsDead()
{
	// 当死亡状态在客户端同步时调用
	if (bIsDead)
	{
		// 可以在这里添加死亡效果等
	}
}

void ADemo3Character::OnDeath()
{
	WeaponSystemComponent->DestroyComponent();
    if (GetLocalRole() != ROLE_Authority) return;
    // 服务器端处理：禁用碰撞
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    if (GetMesh()) {
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

void ADemo3Character::ClientOnDamaged_Implementation(float DamageAmount, FVector DamageDirection)
{
	// 广播受到伤害事件
	OnDamaged.Broadcast(DamageAmount, DamageDirection);
	
	// 广播生命值更新事件
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, GetHealthPercent());
}