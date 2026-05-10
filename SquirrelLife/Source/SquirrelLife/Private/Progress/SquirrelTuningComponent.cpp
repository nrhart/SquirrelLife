// Copyright Epic Games, Inc. All Rights Reserved.

#include "Progress/SquirrelTuningComponent.h"

#include "MiniGames/SquirrelMiniGameModeBase.h"
#include "Progress/SquirrelGameInstance.h"
#include "Training/SquirrelTrainingGameMode.h"

USquirrelTuningComponent::USquirrelTuningComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

USquirrelProgressTuningData* USquirrelTuningComponent::ResolveProgressTuning() const
{
	if (ProgressTuning)
	{
		return ProgressTuning;
	}

	if (const UWorld* World = GetWorld())
	{
		if (const ASquirrelMiniGameModeBase* MiniGameMode = World->GetAuthGameMode<ASquirrelMiniGameModeBase>())
		{
			if (USquirrelProgressTuningData* ModeTuning = MiniGameMode->GetAssignedProgressTuning())
			{
				return ModeTuning;
			}
		}

		if (const ASquirrelTrainingGameMode* TrainingGameMode = World->GetAuthGameMode<ASquirrelTrainingGameMode>())
		{
			if (USquirrelProgressTuningData* ModeTuning = TrainingGameMode->GetAssignedProgressTuning())
			{
				return ModeTuning;
			}
		}
	}

	if (const USquirrelGameInstance* SquirrelGameInstance = GetWorld() ? GetWorld()->GetGameInstance<USquirrelGameInstance>() : nullptr)
	{
		return SquirrelGameInstance->GetProgressTuning();
	}

	return nullptr;
}

FSquirrelStatTuning USquirrelTuningComponent::GetStatTuning(ESquirrelProgressStat Stat) const
{
	if (const USquirrelProgressTuningData* TuningData = ResolveProgressTuning())
	{
		return TuningData->GetTuningForStat(Stat);
	}

	FSquirrelStatTuning DefaultTuning;
	if (Stat == ESquirrelProgressStat::Energy)
	{
		DefaultTuning.StartingLevel = 3;
		DefaultTuning.MaxLevel = 100;
	}
	return DefaultTuning;
}

FSquirrelMiniGameTuning USquirrelTuningComponent::GetMiniGameTuning(ESquirrelProgressStat Stat) const
{
	if (const USquirrelProgressTuningData* TuningData = ResolveProgressTuning())
	{
		return TuningData->GetMiniGameTuningForStat(Stat);
	}

	return FSquirrelMiniGameTuning();
}

int32 USquirrelTuningComponent::GetStartingCoins() const
{
	if (const USquirrelProgressTuningData* TuningData = ResolveProgressTuning())
	{
		return FMath::Max(TuningData->StartingCoins, 0);
	}

	return 25;
}

int32 USquirrelTuningComponent::GetEnergyPointsPerFood() const
{
	if (const USquirrelProgressTuningData* TuningData = ResolveProgressTuning())
	{
		return FMath::Max(TuningData->EnergyPointsPerFood, 0);
	}

	return 3;
}
