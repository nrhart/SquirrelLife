// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Progress/SquirrelProgressTypes.h"
#include "SquirrelProgressTuningData.generated.h"

USTRUCT(BlueprintType)
struct FSquirrelStatTuning
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress", meta = (ClampMin = "0"))
	int32 StartingLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress", meta = (ClampMin = "1"))
	int32 MaxLevel = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress", meta = (ClampMin = "1"))
	int32 PointsPerLevel = 5;
};

USTRUCT(BlueprintType)
struct FSquirrelMiniGameTuning
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game", meta = (ClampMin = "0"))
	int32 CoinsPerScoreUnit = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game", meta = (ClampMin = "0"))
	int32 ProgressPerScoreUnit = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game", meta = (ClampMin = "0"))
	int32 MinimumProgressPerRun = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game", meta = (ClampMin = "1", Units = "cm"))
	float DistancePerScoreUnit = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game", meta = (ClampMin = "1", Units = "cm"))
	float DistancePerCoin = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game", meta = (ClampMin = "1", Units = "cm"))
	float DistanceDisplayDivisor = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game", meta = (ClampMin = "0", Units = "cm/s"))
	float BaseScrollSpeed = 280.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game", meta = (ClampMin = "0", Units = "cm/s"))
	float ScrollSpeedPerLevel = 12.0f;
};

UCLASS(BlueprintType)
class SQUIRRELLIFE_API USquirrelProgressTuningData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Economy", meta = (ClampMin = "0"))
	int32 StartingCoins = 25;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Training", meta = (ClampMin = "0"))
	int32 EnergyPointsPerFood = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	TMap<ESquirrelProgressStat, FSquirrelStatTuning> StatTunings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mini Games")
	TMap<ESquirrelProgressStat, FSquirrelMiniGameTuning> MiniGameTunings;

	USquirrelProgressTuningData();

	UFUNCTION(BlueprintPure, Category = "Progress")
	FSquirrelStatTuning GetTuningForStat(ESquirrelProgressStat Stat) const;

	UFUNCTION(BlueprintPure, Category = "Mini Game")
	FSquirrelMiniGameTuning GetMiniGameTuningForStat(ESquirrelProgressStat Stat) const;
};
