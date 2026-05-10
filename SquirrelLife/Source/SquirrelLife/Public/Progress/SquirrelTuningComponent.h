// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Progress/SquirrelProgressTuningData.h"
#include "SquirrelTuningComponent.generated.h"

UCLASS(ClassGroup = (Squirrel), Blueprintable, meta = (BlueprintSpawnableComponent))
class SQUIRRELLIFE_API USquirrelTuningComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USquirrelTuningComponent();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tuning")
	TObjectPtr<USquirrelProgressTuningData> ProgressTuning;

public:
	UFUNCTION(BlueprintCallable, Category = "Tuning")
	void SetProgressTuning(USquirrelProgressTuningData* NewProgressTuning) { ProgressTuning = NewProgressTuning; }

	UFUNCTION(BlueprintPure, Category = "Tuning")
	USquirrelProgressTuningData* GetAssignedProgressTuning() const { return ProgressTuning; }

	UFUNCTION(BlueprintPure, Category = "Tuning")
	USquirrelProgressTuningData* ResolveProgressTuning() const;

	UFUNCTION(BlueprintPure, Category = "Tuning")
	FSquirrelStatTuning GetStatTuning(ESquirrelProgressStat Stat) const;

	UFUNCTION(BlueprintPure, Category = "Tuning")
	FSquirrelMiniGameTuning GetMiniGameTuning(ESquirrelProgressStat Stat) const;

	UFUNCTION(BlueprintPure, Category = "Tuning")
	int32 GetStartingCoins() const;

	UFUNCTION(BlueprintPure, Category = "Tuning")
	int32 GetEnergyPointsPerFood() const;
};
