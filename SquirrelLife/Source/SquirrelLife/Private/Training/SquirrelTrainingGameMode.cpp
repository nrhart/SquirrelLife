// Copyright Epic Games, Inc. All Rights Reserved.

#include "Training/SquirrelTrainingGameMode.h"

#include "EngineUtils.h"
#include "Training/SquirrelFoodActor.h"
#include "Training/SquirrelTrainingPawn.h"
#include "Training/SquirrelTrainingPlayerController.h"

ASquirrelTrainingGameMode::ASquirrelTrainingGameMode()
{
	DefaultPawnClass = ASquirrelTrainingPawn::StaticClass();
	PlayerControllerClass = ASquirrelTrainingPlayerController::StaticClass();
	FoodClass = ASquirrelFoodActor::StaticClass();

	DefaultFoodLocations = {
		FVector(-360.0f, 0.0f, 140.0f),
		FVector(0.0f, 0.0f, 290.0f),
		FVector(360.0f, 0.0f, 170.0f)
	};
}

void ASquirrelTrainingGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoSpawnDefaultFood && !HasFoodInLevel())
	{
		SpawnDefaultFood();
	}
}

bool ASquirrelTrainingGameMode::HasFoodInLevel() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	for (TActorIterator<ASquirrelFoodActor> It(World); It; ++It)
	{
		return true;
	}

	return false;
}

void ASquirrelTrainingGameMode::SpawnDefaultFood()
{
	UWorld* World = GetWorld();
	if (!World || !FoodClass)
	{
		return;
	}

	for (const FVector& FoodLocation : DefaultFoodLocations)
	{
		World->SpawnActor<ASquirrelFoodActor>(FoodClass, FoodLocation, FRotator::ZeroRotator);
	}
}
