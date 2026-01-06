// Copyright Epic Games, Inc. All Rights Reserved.

#include "../Public/Demo3PlayerState.h"
#include "Net/UnrealNetwork.h"

ADemo3PlayerState::ADemo3PlayerState()
{
	Team = ETeam::None;
}

void ADemo3PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADemo3PlayerState, Team);
}

void ADemo3PlayerState::SetTeam(ETeam NewTeam)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		Team = NewTeam;
	}
}

void ADemo3PlayerState::OnRep_Team()
{
	// 当阵营在客户端同步时调用
    // TODO Update UI, sync team info to UMG
}

