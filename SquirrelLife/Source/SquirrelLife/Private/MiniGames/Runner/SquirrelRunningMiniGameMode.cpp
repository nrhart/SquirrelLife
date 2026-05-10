// Copyright Epic Games, Inc. All Rights Reserved.

#include "MiniGames/Runner/SquirrelRunningMiniGameMode.h"

#include "MiniGames/Runner/SquirrelEndlessRunnerPawn.h"

ASquirrelRunningMiniGameMode::ASquirrelRunningMiniGameMode()
{
	DefaultPawnClass = ASquirrelEndlessRunnerPawn::StaticClass();
	TrainedStat = ESquirrelProgressStat::Running;
}
