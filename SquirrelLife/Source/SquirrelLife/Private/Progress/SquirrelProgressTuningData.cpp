// Copyright Epic Games, Inc. All Rights Reserved.

#include "Progress/SquirrelProgressTuningData.h"

USquirrelProgressTuningData::USquirrelProgressTuningData()
{
	StatTunings.Add(ESquirrelProgressStat::Energy, FSquirrelStatTuning{3, 100, 5});
	StatTunings.Add(ESquirrelProgressStat::Running, FSquirrelStatTuning{1, 50, 5});
	StatTunings.Add(ESquirrelProgressStat::Climbing, FSquirrelStatTuning{1, 50, 5});
	StatTunings.Add(ESquirrelProgressStat::Flying, FSquirrelStatTuning{1, 50, 5});
	StatTunings.Add(ESquirrelProgressStat::Swimming, FSquirrelStatTuning{1, 50, 5});

	MiniGameTunings.Add(ESquirrelProgressStat::Running, FSquirrelMiniGameTuning{1, 1, 1, 100.0f, 250.0f, 100.0f, 280.0f, 12.0f});
	MiniGameTunings.Add(ESquirrelProgressStat::Climbing, FSquirrelMiniGameTuning{1, 1, 1, 100.0f, 250.0f, 100.0f, 280.0f, 12.0f});
	MiniGameTunings.Add(ESquirrelProgressStat::Flying, FSquirrelMiniGameTuning{1, 1, 1, 100.0f, 250.0f, 100.0f, 280.0f, 12.0f});
	MiniGameTunings.Add(ESquirrelProgressStat::Swimming, FSquirrelMiniGameTuning{1, 1, 1, 100.0f, 250.0f, 100.0f, 280.0f, 12.0f});
}

FSquirrelStatTuning USquirrelProgressTuningData::GetTuningForStat(ESquirrelProgressStat Stat) const
{
	if (const FSquirrelStatTuning* Tuning = StatTunings.Find(Stat))
	{
		FSquirrelStatTuning NormalizedTuning = *Tuning;
		NormalizedTuning.StartingLevel = FMath::Max(NormalizedTuning.StartingLevel, 0);
		NormalizedTuning.MaxLevel = FMath::Max(NormalizedTuning.MaxLevel, 1);
		NormalizedTuning.PointsPerLevel = FMath::Max(NormalizedTuning.PointsPerLevel, 1);
		return NormalizedTuning;
	}

	FSquirrelStatTuning DefaultTuning;
	if (Stat == ESquirrelProgressStat::Energy)
	{
		DefaultTuning.StartingLevel = 3;
		DefaultTuning.MaxLevel = 100;
	}

	return DefaultTuning;
}

FSquirrelMiniGameTuning USquirrelProgressTuningData::GetMiniGameTuningForStat(ESquirrelProgressStat Stat) const
{
	FSquirrelMiniGameTuning Tuning;
	if (const FSquirrelMiniGameTuning* FoundTuning = MiniGameTunings.Find(Stat))
	{
		Tuning = *FoundTuning;
	}

	Tuning.CoinsPerScoreUnit = FMath::Max(Tuning.CoinsPerScoreUnit, 0);
	Tuning.ProgressPerScoreUnit = FMath::Max(Tuning.ProgressPerScoreUnit, 0);
	Tuning.MinimumProgressPerRun = FMath::Max(Tuning.MinimumProgressPerRun, 0);
	Tuning.DistancePerScoreUnit = FMath::Max(Tuning.DistancePerScoreUnit, 1.0f);
	Tuning.DistancePerCoin = FMath::Max(Tuning.DistancePerCoin, 1.0f);
	Tuning.DistanceDisplayDivisor = FMath::Max(Tuning.DistanceDisplayDivisor, 1.0f);
	Tuning.BaseScrollSpeed = FMath::Max(Tuning.BaseScrollSpeed, 0.0f);
	Tuning.ScrollSpeedPerLevel = FMath::Max(Tuning.ScrollSpeedPerLevel, 0.0f);
	return Tuning;
}
