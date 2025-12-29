// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/FPSDemoPlayerController.h"

#include "Demo3PlayerState.h"
#include "Demo3Character.h"
#include "Weapon/WeaponSystemComponent.h"


void AFPSDemoPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	ADemo3PlayerState* PS = GetPlayerState<ADemo3PlayerState>();
	if (!PS) return;

	HandlePlayerStateChange(PS);
}

void AFPSDemoPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (auto ps = Cast<ADemo3PlayerState>(PlayerState))
	{
		HandlePlayerStateChange(ps);
	}
}

void AFPSDemoPlayerController::SwitchToNextWeapon()
{
	ADemo3Character* Demo3Character = Cast<ADemo3Character>(GetPawn());
	if (Demo3Character == nullptr)
	{
		return;
	}

	UWeaponSystemComponent* WeaponComp = Demo3Character->GetWeaponSystemComponent();
	if (WeaponComp == nullptr)
	{
		return;
	}

	WeaponComp->SwitchToNextWeapon();
}

void AFPSDemoPlayerController::SwitchToPreviousWeapon()
{
	ADemo3Character* Demo3Character = Cast<ADemo3Character>(GetPawn());
	if (Demo3Character == nullptr)
	{
		return;
	}

	UWeaponSystemComponent* WeaponComp = Demo3Character->GetWeaponSystemComponent();
	if (WeaponComp == nullptr)
	{
		return;
	}

	WeaponComp->SwitchToPreviousWeapon();
}

void AFPSDemoPlayerController::FinishGame_Implementation()
{
	ClientFinishGame();
}

void AFPSDemoPlayerController::ClientFinishGame_Implementation()
{
	
}

void AFPSDemoPlayerController::SetMouseVerticalSensitivity(float Sensitivity)
{
	MouseVerticalSensitivity = FMath::Max(Sensitivity, 0.01f);
}

void AFPSDemoPlayerController::SetMouseHorizontalSensitivity(float Sensitivity)
{
	MouseHorizontalSensitivity = FMath::Max(Sensitivity, 0.1f);
}

