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

void ADemo3GameMode::AssignTeamFor(APlayerController* InPlayerController)
{
	if (ADemo3PlayerState* Demo3PlayerState = InPlayerController->GetPlayerState<ADemo3PlayerState>())
	{
		// 统计当前所有玩家的队伍人数
		int32 RedTeamCount = 0;
		int32 BlueTeamCount = 0;

		if (ADemo3GameState* Demo3GameState = GetGameState<ADemo3GameState>())
		{
			// 遍历所有玩家的 PlayerState
			for (APlayerState* ps : Demo3GameState->PlayerArray)
			{
				if (ADemo3PlayerState* OtherPlayerState = Cast<ADemo3PlayerState>(ps))
				{
					// 跳过当前要分配的玩家
					if (OtherPlayerState == Demo3PlayerState)
					{
						continue;
					}

					// 统计各队伍人数
					if (OtherPlayerState->Team == ETeam::Red)
					{
						RedTeamCount++;
					}
					else if (OtherPlayerState->Team == ETeam::Blue)
					{
						BlueTeamCount++;
					}
				}
			}
		}

		// 分配到人数少的队伍，如果人数相等则默认分配到红队
		ETeam AssignedTeam = (RedTeamCount <= BlueTeamCount) ? ETeam::Red : ETeam::Blue;
		Demo3PlayerState->SetTeam(AssignedTeam);
	}
}
