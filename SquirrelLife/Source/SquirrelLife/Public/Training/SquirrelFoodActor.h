// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SquirrelFoodActor.generated.h"

class ASquirrelTrainingPawn;
class UStaticMeshComponent;

/**
 * Food pickup for the early Squirrel Life training loop.
 * Drag the squirrel onto it to increase training power.
 */
UCLASS(Blueprintable)
class SQUIRRELLIFE_API ASquirrelFoodActor : public AActor
{
	GENERATED_BODY()

public:
	ASquirrelFoodActor();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> FoodMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Food", meta = (ClampMin = "1"))
	int32 PowerValue = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Food", meta = (ClampMin = "0", Units = "cm"))
	float ConsumeRadius = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Food")
	bool bSimulatePhysicsOnSpawn = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Food|Physics")
	FVector FoodScale = FVector(0.28f, 0.28f, 0.28f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Food|Physics", meta = (ClampMin = "0"))
	float LinearDamping = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Food|Physics", meta = (ClampMin = "0"))
	float AngularDamping = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Food")
	bool bRespawnAfterConsumed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Food", meta = (ClampMin = "0.1", Units = "s", EditCondition = "bRespawnAfterConsumed"))
	float RespawnDelay = 3.0f;

	FTimerHandle RespawnTimer;

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void ApplyFoodTuning();
	void Consume(ASquirrelTrainingPawn* Squirrel);
	void Respawn();

public:
	UFUNCTION(BlueprintCallable, Category = "Food")
	bool TryConsume(ASquirrelTrainingPawn* Squirrel);

	UFUNCTION(BlueprintCallable, Category = "Food")
	void SetConsumed(bool bConsumed);

	UFUNCTION(BlueprintCallable, Category = "Food")
	void SetRespawnAfterConsumed(bool bShouldRespawn) { bRespawnAfterConsumed = bShouldRespawn; }

	UFUNCTION(BlueprintCallable, Category = "Food")
	void LaunchFromDispenser(const FVector& Impulse);

	UFUNCTION(BlueprintPure, Category = "Food")
	int32 GetPowerValue() const { return PowerValue; }

	UFUNCTION(BlueprintPure, Category = "Food")
	float GetConsumeRadius() const { return ConsumeRadius; }

	UFUNCTION(BlueprintPure, Category = "Food")
	bool IsAvailableToConsume() const { return !IsHidden(); }

	UFUNCTION(BlueprintImplementableEvent, Category = "Food")
	void OnConsumed(ASquirrelTrainingPawn* Squirrel);

	UFUNCTION(BlueprintImplementableEvent, Category = "Food")
	void OnRespawned();
};
