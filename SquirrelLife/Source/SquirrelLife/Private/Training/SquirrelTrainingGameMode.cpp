// Copyright Epic Games, Inc. All Rights Reserved.

#include "Training/SquirrelTrainingGameMode.h"

#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Training/SquirrelFoodActor.h"
#include "Training/SquirrelTrainingPawn.h"
#include "Training/SquirrelTrainingPlayerController.h"

ASquirrelTrainingGameMode::ASquirrelTrainingGameMode()
{
	DefaultPawnClass = ASquirrelTrainingPawn::StaticClass();
	PlayerControllerClass = ASquirrelTrainingPlayerController::StaticClass();
	TrainingPawnClass = ASquirrelTrainingPawn::StaticClass();
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

	if (bEnsureTrainingPawnAtPlayerStart)
	{
		EnsureTrainingPawnAtPlayerStart();
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

void ASquirrelTrainingGameMode::EnsureTrainingPawnAtPlayerStart()
{
	UWorld* World = GetWorld();
	APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;
	if (!World || !PlayerController)
	{
		return;
	}

	AActor* StartSpot = FindTrainingPlayerStart(PlayerController);
	if (!StartSpot)
	{
		return;
	}

	const FTransform StartTransform(StartSpot->GetActorRotation(), StartSpot->GetActorLocation());
	if (ASquirrelTrainingPawn* ExistingTrainingPawn = Cast<ASquirrelTrainingPawn>(PlayerController->GetPawn()))
	{
		if (bMoveExistingTrainingPawnToPlayerStart)
		{
			ExistingTrainingPawn->SetActorTransform(StartTransform, false, nullptr, ETeleportType::TeleportPhysics);
		}
		return;
	}

	if (APawn* ExistingPawn = PlayerController->GetPawn())
	{
		ExistingPawn->Destroy();
	}

	const TSubclassOf<ASquirrelTrainingPawn> PawnClass = GetTrainingPawnClass();
	if (!PawnClass)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ASquirrelTrainingPawn* SpawnedPawn = World->SpawnActor<ASquirrelTrainingPawn>(PawnClass, StartTransform, SpawnParams);
	if (SpawnedPawn)
	{
		PlayerController->Possess(SpawnedPawn);
	}
}

AActor* ASquirrelTrainingGameMode::FindTrainingPlayerStart(APlayerController* PlayerController)
{
	if (AActor* ChosenStart = ChoosePlayerStart(PlayerController))
	{
		return ChosenStart;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		return *It;
	}

	return nullptr;
}

TSubclassOf<ASquirrelTrainingPawn> ASquirrelTrainingGameMode::GetTrainingPawnClass() const
{
	if (TrainingPawnClass)
	{
		return TrainingPawnClass;
	}

	if (DefaultPawnClass && DefaultPawnClass->IsChildOf(ASquirrelTrainingPawn::StaticClass()))
	{
		return *DefaultPawnClass;
	}

	return ASquirrelTrainingPawn::StaticClass();
}
