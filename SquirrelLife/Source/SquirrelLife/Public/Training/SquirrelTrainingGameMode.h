// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SquirrelTrainingGameMode.generated.h"

class ASquirrelFoodActor;

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

	virtual void BeginPlay() override;

	bool HasFoodInLevel() const;
	void SpawnDefaultFood();
};
