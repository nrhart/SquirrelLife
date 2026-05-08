// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SquirrelFoodDispenserActor.generated.h"

class ASquirrelFoodActor;
class ASquirrelTrainingPlayerController;
class USceneComponent;
class UStaticMeshComponent;
class UWidgetComponent;

/**
 * Placeable dispenser that sells food through an in-world UI button.
 */
UCLASS(Blueprintable)
class SQUIRRELLIFE_API ASquirrelFoodDispenserActor : public AActor
{
	GENERATED_BODY()

public:
	ASquirrelFoodDispenserActor();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DispenserMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> FoodSpawnPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UWidgetComponent> PurchaseWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispenser")
	TSubclassOf<ASquirrelFoodActor> FoodClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispenser", meta = (ClampMin = "0"))
	int32 FoodCost = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispenser")
	bool bSpawnedFoodRespawns = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispenser", meta = (ClampMin = "1"))
	int32 MaxActiveFoodCount = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispenser", meta = (ClampMin = "0", Units = "s"))
	float DuplicateClickGuardSeconds = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispenser", meta = (ClampMin = "0"))
	float DispenseSpacing = 55.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispenser", meta = (ClampMin = "0"))
	float DispenseForwardImpulse = 130.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispenser", meta = (ClampMin = "0"))
	float DispenseUpImpulse = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispenser", meta = (ClampMin = "0"))
	float DispenseSideImpulseRange = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispenser", meta = (ClampMin = "0", Units = "deg"))
	float SpawnPitchRollRange = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispenser")
	FVector DispenserMeshScale = FVector(0.7f, 0.7f, 1.2f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispenser|Widget")
	FVector PurchaseWidgetRelativeLocation = FVector(0.0f, -12.0f, 150.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispenser|Widget")
	FRotator PurchaseWidgetRelativeRotation = FRotator(0.0f, 180.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispenser|Widget")
	FVector2D PurchaseWidgetDrawSize = FVector2D(220.0f, 90.0f);

	int32 DispensedFoodCount = 0;
	TArray<TWeakObjectPtr<ASquirrelFoodActor>> ActiveSpawnedFoods;
	float LastDispenseTimeSeconds = -TNumericLimits<float>::Max();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

	void ApplyDispenserTuning();
	ASquirrelTrainingPlayerController* GetTrainingPlayerController() const;
	FVector GetNextFoodSpawnLocation() const;
	FVector GetFoodDispenseImpulse() const;
	void PruneInactiveFood();
	int32 GetActiveFoodCount() const;

public:
	UFUNCTION(BlueprintCallable, Category = "Dispenser")
	bool TryDispenseFood();

	UFUNCTION(BlueprintPure, Category = "Dispenser")
	FText GetButtonText() const;

	UFUNCTION(BlueprintPure, Category = "Dispenser")
	int32 GetFoodCost() const { return FoodCost; }
};
