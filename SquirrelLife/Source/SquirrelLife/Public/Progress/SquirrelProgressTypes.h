// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SquirrelProgressTypes.generated.h"

UENUM(BlueprintType)
enum class ESquirrelProgressStat : uint8
{
	Energy,
	Running,
	Climbing,
	Flying,
	Swimming
};

USTRUCT(BlueprintType)
struct FSquirrelLevelProgress
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress", meta = (ClampMin = "0"))
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress", meta = (ClampMin = "0"))
	int32 ProgressPoints = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress", meta = (ClampMin = "1"))
	int32 MaxLevel = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress", meta = (ClampMin = "1"))
	int32 PointsPerLevel = 5;

	void Normalize()
	{
		MaxLevel = FMath::Max(MaxLevel, 1);
		PointsPerLevel = FMath::Max(PointsPerLevel, 1);
		Level = FMath::Clamp(Level, 0, MaxLevel);
		ProgressPoints = FMath::Clamp(ProgressPoints, 0, PointsPerLevel - 1);
	}

	bool AddProgress(int32 Amount)
	{
		if (Amount <= 0 || Level >= MaxLevel)
		{
			Normalize();
			return false;
		}

		const int32 PreviousLevel = Level;
		ProgressPoints += Amount;

		while (ProgressPoints >= PointsPerLevel && Level < MaxLevel)
		{
			ProgressPoints -= PointsPerLevel;
			Level++;
		}

		if (Level >= MaxLevel)
		{
			Level = MaxLevel;
			ProgressPoints = 0;
		}

		Normalize();
		return Level > PreviousLevel;
	}

	bool TrySpendLevels(int32 Amount)
	{
		if (Amount <= 0)
		{
			return true;
		}

		if (Level < Amount)
		{
			return false;
		}

		Level -= Amount;
		Normalize();
		return true;
	}

	float GetProgressPercent() const
	{
		return PointsPerLevel > 0 ? static_cast<float>(ProgressPoints) / static_cast<float>(PointsPerLevel) : 0.0f;
	}

	float GetLevelPercent() const
	{
		return MaxLevel > 0 ? static_cast<float>(Level) / static_cast<float>(MaxLevel) : 0.0f;
	}
};
