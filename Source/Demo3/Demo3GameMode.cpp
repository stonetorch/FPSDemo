// Copyright Epic Games, Inc. All Rights Reserved.

#include "Demo3GameMode.h"
#include "Demo3Character.h"
#include "UObject/ConstructorHelpers.h"

ADemo3GameMode::ADemo3GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
