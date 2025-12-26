// Copyright Epic Games, Inc. All Rights Reserved.

#include "../Public/Demo3GameMode.h"
#include "Demo3Character.h"
#include "../Public/Demo3PlayerState.h"
#include "../Public/Demo3GameState.h"
#include "UObject/ConstructorHelpers.h"

ADemo3GameMode::ADemo3GameMode()
	: Super()
{
	// 设置默认的PlayerState类
	PlayerStateClass = ADemo3PlayerState::StaticClass();
	// 设置默认的GameState类
	GameStateClass = ADemo3GameState::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
