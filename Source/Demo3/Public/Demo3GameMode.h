// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Demo3GameMode.generated.h"

class ADemo3PlayerState;
class ACharacter;
class APlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerDiedDelegate, ACharacter*, Character, APlayerController*, PlayerController);

UCLASS(minimalapi)
class ADemo3GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADemo3GameMode();

	// 玩家死亡事件分发器
	UPROPERTY(BlueprintAssignable)
	FOnPlayerDiedDelegate OnPlayerDied;

	UFUNCTION(BlueprintCallable)
	void AssignTeamFor(APlayerController* InPlayerController);
};



