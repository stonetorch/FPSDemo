// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Demo3GameState.h"
#include "Demo3PlayerState.generated.h"

UCLASS()
class DEMO3_API ADemo3PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ADemo3PlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** 最大血量 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Health, Replicated)
	float MaxHealth;

	/** 当前血量 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, ReplicatedUsing = OnRep_CurrentHealth)
	float CurrentHealth;

	/** 是否已死亡 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, ReplicatedUsing = OnRep_IsDead)
	bool bIsDead;

	/** 玩家所属阵营 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Team, ReplicatedUsing = OnRep_Team)
	ETeam Team;

	/** 设置最大血量 */
	UFUNCTION(BlueprintCallable, Category = Health)
	void SetMaxHealth(float NewMaxHealth);

	/** 设置当前血量 */
	UFUNCTION(BlueprintCallable, Category = Health)
	void SetCurrentHealth(float NewCurrentHealth);

	/** 设置死亡状态 */
	UFUNCTION(BlueprintCallable, Category = Health)
	void SetIsDead(bool bNewIsDead);

	/** 获取最大血量 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Health)
	float GetMaxHealth() const { return MaxHealth; }

	/** 获取当前血量 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Health)
	float GetCurrentHealth() const { return CurrentHealth; }

	/** 获取是否已死亡 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Health)
	bool GetIsDead() const { return bIsDead; }

	/** 获取血量百分比 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Health)
	float GetHealthPercent() const;

	/** 设置玩家阵营 */
	UFUNCTION(BlueprintCallable, Category = Team)
	void SetTeam(ETeam NewTeam);

	/** 获取玩家阵营 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Team)
	ETeam GetTeam() const { return Team; }

	/** 生命值更新事件分发器 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthChanged, float, CurrentHealth, float, MaxHealth, float, HealthPercent);

	/** 生命值更新事件 */
	UPROPERTY(BlueprintAssignable, Category = Health)
	FOnHealthChanged OnHealthChanged;

protected:
	/** 当血量变化时调用（用于通知客户端） */
	UFUNCTION()
	virtual void OnRep_CurrentHealth();

	/** 当死亡状态变化时调用（用于通知客户端） */
	UFUNCTION()
	virtual void OnRep_IsDead();

	/** 当阵营变化时调用（用于通知客户端） */
	UFUNCTION()
	virtual void OnRep_Team();
};

