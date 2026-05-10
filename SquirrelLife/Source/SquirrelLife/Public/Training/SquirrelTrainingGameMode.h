// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SquirrelTrainingGameMode.generated.h"

class ASquirrelFoodActor;
class ASquirrelTrainingPawn;
class APlayerController;
class USquirrelProgressTuningData;

/**
 * Bootstraps the first Squirrel Life training prototype.
 */
UCLASS(Blueprintable)
class SQUIRRELLIFE_API ASquirrelTrainingGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASquirrelTrainingGameMode();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Food")
	TSubclassOf<ASquirrelFoodActor> FoodClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Food")
	TArray<FVector> DefaultFoodLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Food")
	bool bAutoSpawnDefaultFood = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Tuning")
	TObjectPtr<USquirrelProgressTuningData> ProgressTuning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Spawn")
	bool bEnsureTrainingPawnAtPlayerStart = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Spawn", meta = (EditCondition = "bEnsureTrainingPawnAtPlayerStart"))
	bool bMoveExistingTrainingPawnToPlayerStart = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Spawn", meta = (EditCondition = "bEnsureTrainingPawnAtPlayerStart"))
	TSubclassOf<ASquirrelTrainingPawn> TrainingPawnClass;

	virtual void BeginPlay() override;

	bool HasFoodInLevel() const;
	void SpawnDefaultFood();
	void EnsureTrainingPawnAtPlayerStart();
	AActor* FindTrainingPlayerStart(APlayerController* PlayerController);
	TSubclassOf<ASquirrelTrainingPawn> GetTrainingPawnClass() const;

public:
	UFUNCTION(BlueprintPure, Category = "Training|Tuning")
	USquirrelProgressTuningData* GetProgressTuning() const { return ProgressTuning; }

	UFUNCTION(BlueprintPure, Category = "Training|Tuning")
	USquirrelProgressTuningData* GetAssignedProgressTuning() const { return ProgressTuning; }
};
