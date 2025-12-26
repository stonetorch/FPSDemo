// Copyright Epic Games, Inc. All Rights Reserved.

#include "../Public/Demo3GameState.h"
#include "Net/UnrealNetwork.h"

ADemo3GameState::ADemo3GameState()
{
	// 默认15分钟
	InitialGameTime = 900.0f;
	RemainingTime = InitialGameTime;
	RedTeamScore = 0;
	BlueTeamScore = 0;
}

void ADemo3GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADemo3GameState, RemainingTime);
	DOREPLIFETIME(ADemo3GameState, RedTeamScore);
	DOREPLIFETIME(ADemo3GameState, BlueTeamScore);
}

void ADemo3GameState::SetRemainingTime(float NewTime)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		RemainingTime = FMath::Max(0.0f, NewTime);
	}
}

void ADemo3GameState::AddRedTeamScore(int32 Points)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		RedTeamScore += Points;
		RedTeamScore = FMath::Max(0, RedTeamScore);
		OnScoreChanged.Broadcast(RedTeamScore, BlueTeamScore);
	}
}

void ADemo3GameState::AddBlueTeamScore(int32 Points)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		BlueTeamScore += Points;
		BlueTeamScore = FMath::Max(0, BlueTeamScore);
		OnScoreChanged.Broadcast(RedTeamScore, BlueTeamScore);
	}
}

void ADemo3GameState::SetRedTeamScore(int32 NewScore)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		RedTeamScore = FMath::Max(0, NewScore);
		OnScoreChanged.Broadcast(RedTeamScore, BlueTeamScore);
	}
}

void ADemo3GameState::SetBlueTeamScore(int32 NewScore)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		BlueTeamScore = FMath::Max(0, NewScore);
		OnScoreChanged.Broadcast(RedTeamScore, BlueTeamScore);
	}
}

int32 ADemo3GameState::GetTeamScore(ETeam Team) const
{
	switch (Team)
	{
	case ETeam::Red:
		return RedTeamScore;
	case ETeam::Blue:
		return BlueTeamScore;
	default:
		return 0;
	}
}

void ADemo3GameState::OnRep_RemainingTime()
{
	// 当剩余时间在客户端同步时调用
}

void ADemo3GameState::OnRep_RedTeamScore()
{
	// 当红队得分在客户端同步时调用
	// 广播得分更新事件，用于UI更新
	OnScoreChanged.Broadcast(RedTeamScore, BlueTeamScore);
}

void ADemo3GameState::OnRep_BlueTeamScore()
{
	// 当蓝队得分在客户端同步时调用
	// 广播得分更新事件，用于UI更新
	OnScoreChanged.Broadcast(RedTeamScore, BlueTeamScore);
}

