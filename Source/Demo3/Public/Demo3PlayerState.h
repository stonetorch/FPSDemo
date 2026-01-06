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

	/** 玩家所属阵营 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Team, ReplicatedUsing = OnRep_Team)
	ETeam Team;


	/** 设置玩家阵营 */
	UFUNCTION(BlueprintCallable, Category = Team)
	void SetTeam(ETeam NewTeam);

	/** 获取玩家阵营 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Team)
	ETeam GetTeam() const { return Team; }

protected:
	/** 当阵营变化时调用（用于通知客户端） */
	UFUNCTION()
	virtual void OnRep_Team();
};

