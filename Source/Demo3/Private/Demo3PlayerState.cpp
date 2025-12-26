// Copyright Epic Games, Inc. All Rights Reserved.

#include "../Public/Demo3PlayerState.h"
#include "Net/UnrealNetwork.h"

ADemo3PlayerState::ADemo3PlayerState()
{
	// 初始化血量
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
	bIsDead = false;
	Team = ETeam::None;
}

void ADemo3PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADemo3PlayerState, MaxHealth);
	DOREPLIFETIME(ADemo3PlayerState, CurrentHealth);
	DOREPLIFETIME(ADemo3PlayerState, bIsDead);
	DOREPLIFETIME(ADemo3PlayerState, Team);
}

void ADemo3PlayerState::SetMaxHealth(float NewMaxHealth)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		MaxHealth = NewMaxHealth;
		if (CurrentHealth > MaxHealth)
		{
			CurrentHealth = MaxHealth;
		}
		OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, GetHealthPercent());
	}
}

void ADemo3PlayerState::SetCurrentHealth(float NewCurrentHealth)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		float OldHealth = CurrentHealth;
		CurrentHealth = FMath::Clamp(NewCurrentHealth, 0.0f, MaxHealth);
		
		// 如果血量发生变化，广播事件
		if (OldHealth != CurrentHealth)
		{
			OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, GetHealthPercent());
		}
	}
}

void ADemo3PlayerState::SetIsDead(bool bNewIsDead)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		bIsDead = bNewIsDead;
	}
}

float ADemo3PlayerState::GetHealthPercent() const
{
	if (MaxHealth > 0.0f)
	{
		return CurrentHealth / MaxHealth;
	}
	return 0.0f;
}

void ADemo3PlayerState::OnRep_CurrentHealth()
{
	// 当血量在客户端同步时调用
	// 广播生命值更新事件，用于UI更新
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, GetHealthPercent());
}

void ADemo3PlayerState::OnRep_IsDead()
{
	// 当死亡状态在客户端同步时调用
	// 可以在这里添加死亡效果等
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

