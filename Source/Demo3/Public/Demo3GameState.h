// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Demo3GameState.generated.h"

/**
 * 阵营枚举
 */
UENUM(BlueprintType)
enum class ETeam : uint8
{
	None,
	Red	,
	Blue
};

/**
 * GameState类，管理全局游戏状态
 */
UCLASS()
class DEMO3_API ADemo3GameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ADemo3GameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** 游戏初始时间（秒），可在编辑器中配置，默认15分钟 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Time", meta = (ClampMin = "0.0"))
	float InitialGameTime;

	/** 游戏剩余时间（秒） */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Time", ReplicatedUsing = OnRep_RemainingTime)
	float RemainingTime;

	/** 红队得分 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score", ReplicatedUsing = OnRep_RedTeamScore)
	int32 RedTeamScore;

	/** 蓝队得分 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score", ReplicatedUsing = OnRep_BlueTeamScore)
	int32 BlueTeamScore;

	/** 设置游戏剩余时间 */
	UFUNCTION(BlueprintCallable, Category = "Game Time")
	void SetRemainingTime(float NewTime);

	/** 获取游戏剩余时间 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Game Time")
	float GetRemainingTime() const { return RemainingTime; }

	/** 增加红队得分 */
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddRedTeamScore(int32 Points);

	/** 增加蓝队得分 */
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddBlueTeamScore(int32 Points);

	/** 设置红队得分 */
	UFUNCTION(BlueprintCallable, Category = "Score")
	void SetRedTeamScore(int32 NewScore);

	/** 设置蓝队得分 */
	UFUNCTION(BlueprintCallable, Category = "Score")
	void SetBlueTeamScore(int32 NewScore);

	/** 获取红队得分 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Score")
	int32 GetRedTeamScore() const { return RedTeamScore; }

	/** 获取蓝队得分 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Score")
	int32 GetBlueTeamScore() const { return BlueTeamScore; }

	/** 根据阵营获取得分 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Score")
	int32 GetTeamScore(ETeam Team) const;

	/** 得分更新事件分发器 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScoreChanged, int32, RedTeamScore, int32, BlueTeamScore);

	/** 得分更新事件 */
	UPROPERTY(BlueprintAssignable, Category = "Score")
	FOnScoreChanged OnScoreChanged;

protected:
	/** 当剩余时间变化时调用（用于通知客户端） */
	UFUNCTION()
	virtual void OnRep_RemainingTime();

	/** 当红队得分变化时调用（用于通知客户端） */
	UFUNCTION()
	virtual void OnRep_RedTeamScore();

	/** 当蓝队得分变化时调用（用于通知客户端） */
	UFUNCTION()
	virtual void OnRep_BlueTeamScore();
};

