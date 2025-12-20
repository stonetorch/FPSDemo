// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSDemoPlayerController.h"

#include "Demo3PlayerState.h"


void AFPSDemoPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	ADemo3PlayerState* PS = GetPlayerState<ADemo3PlayerState>();
	if (!PS) return;

	HandlePlayerStateChange(PS);
}

