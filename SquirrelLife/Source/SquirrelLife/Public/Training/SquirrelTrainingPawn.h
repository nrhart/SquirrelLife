// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SquirrelTrainingPawn.generated.h"

class UCapsuleComponent;
class UFloatingPawnMovement;
class USceneComponent;
class USkeletalMeshComponent;
class UStaticMeshComponent;
class ASquirrelFoodActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSquirrelPowerChangedSignature, int32, NewPower, float, NewMoveSpeed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSquirrelEnergyChangedSignature, int32, NewEnergyLevel, int32, NewEnergyProgressPoints, int32, NewMaxEnergyLevel);

/**
 * Early Duck Life-inspired squirrel pawn.
 * It wanders on a 2D X/Z plane by itself, while the player can drag it onto food.
 */
UCLASS(Blueprintable)
class SQUIRRELLIFE_API ASquirrelTrainingPawn : public APawn
{
	GENERATED_BODY()

public:
	ASquirrelTrainingPawn();

	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCapsuleComponent> Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UFloatingPawnMovement> MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> VisualRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> TailMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> SquirrelMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Visuals")
	bool bUseSkeletalSquirrelVisual = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Visuals")
	FVector SkeletalMeshRelativeLocation = FVector(0.0f, 0.0f, -58.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Visuals")
	bool bAutoAlignSkeletalMeshToCapsuleBottom = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Visuals", meta = (EditCondition = "bAutoAlignSkeletalMeshToCapsuleBottom", Units = "cm"))
	float SkeletalMeshGroundOffset = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Visuals")
	FRotator SkeletalMeshRelativeRotation = FRotator(0.0f, -90.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Visuals")
	FVector SkeletalMeshRelativeScale = FVector(1.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Visuals")
	bool bHidePlaceholderMeshesWhenUsingSkeletalMesh = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Movement", meta = (ClampMin = "0"))
	float BaseMoveSpeed = 130.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Movement", meta = (ClampMin = "0"))
	float SpeedPerPower = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Movement", meta = (ClampMin = "0"))
	float DragFollowSpeed = 1800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Movement", meta = (ClampMin = "0"))
	float VisualTurnInterpSpeed = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Movement")
	bool bSnapToMouseWhileDragging = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Movement")
	bool bSweepDragMovement = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Movement", meta = (ClampMin = "0", Units = "cm"))
	float PatrolDistanceFromSpawn = 420.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Movement", meta = (ClampMin = "0", Units = "cm"))
	float PatrolAcceptanceRadius = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Movement", meta = (ClampMin = "0", Units = "cm"))
	float RandomWanderMinDistance = 140.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Movement", meta = (ClampMin = "0", Units = "cm"))
	float RandomWanderMaxDistance = 340.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Movement", meta = (ClampMin = "0", Units = "cm"))
	float RandomWanderMinDepthDistance = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Movement", meta = (ClampMin = "0", Units = "cm"))
	float RandomWanderMaxDepthDistance = 28.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Movement", meta = (ClampMin = "0", Units = "cm"))
	float RandomWanderMaxDepthOffset = 65.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Movement", meta = (ClampMin = "0", Units = "s"))
	float RandomWanderMinIdleTime = 1.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Movement", meta = (ClampMin = "0", Units = "s"))
	float RandomWanderMaxIdleTime = 3.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Movement", meta = (ClampMin = "0", Units = "cm/s^2"))
	float GravityAcceleration = 2400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Movement", meta = (ClampMin = "0", Units = "cm/s"))
	float TerminalFallSpeed = 2200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Movement", meta = (ClampMin = "0", Units = "cm"))
	float GroundProbeDistance = 10000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Movement", meta = (ClampMin = "0", Units = "cm"))
	float GroundSnapDistance = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Movement")
	TEnumAsByte<ECollisionChannel> GroundTraceChannel = ECC_Visibility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Movement")
	bool bConstrainToTrainingBounds = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Movement", meta = (EditCondition = "bConstrainToTrainingBounds"))
	FVector2D TrainingBoundsX = FVector2D(-2000.0f, 2000.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Movement", meta = (EditCondition = "bConstrainToTrainingBounds"))
	FVector2D TrainingBoundsZ = FVector2D(0.0f, 1200.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Movement")
	float TrainingPlaneY = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Movement")
	bool bUseSpawnYAsTrainingPlane = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Food", meta = (ClampMin = "0", Units = "cm"))
	float FoodConsumeRadius = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Food", meta = (ClampMin = "0", Units = "cm"))
	float FoodSeekRadius = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Food", meta = (ClampMin = "0", Units = "s"))
	float FoodEatCooldown = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Food", meta = (ClampMin = "0", Units = "s"))
	float PostEatPatrolCooldown = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Food", meta = (ClampMin = "0", Units = "s"))
	float EatDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Food", meta = (ClampMin = "0", Units = "s"))
	float DropConsumeWindow = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Stats", meta = (ClampMin = "0"))
	int32 StartingEnergyLevel = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Stats", meta = (ClampMin = "1"))
	int32 MaxEnergyLevel = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Stats", meta = (ClampMin = "1"))
	int32 EnergyPointsPerLevel = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Stats", meta = (ClampMin = "0"))
	int32 EnergyPointsPerFood = 3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Training|Stats")
	int32 EnergyLevel = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Training|Stats")
	int32 EnergyProgressPoints = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Training|Stats")
	int32 PowerLevel = 0;

	UPROPERTY(BlueprintAssignable, Category = "Training|Stats")
	FSquirrelPowerChangedSignature OnPowerChanged;

	UPROPERTY(BlueprintAssignable, Category = "Training|Stats")
	FSquirrelEnergyChangedSignature OnEnergyChanged;

	bool bIsDragging = false;
	bool bIsGrounded = false;
	float VerticalVelocity = 0.0f;
	float PatrolCenterX = 0.0f;
	float PatrolCenterY = 0.0f;
	float PatrolTargetX = 0.0f;
	float PatrolTargetY = 0.0f;
	float PatrolDirection = 1.0f;
	float DropConsumeTimeRemaining = 0.0f;
	float FoodEatCooldownRemaining = 0.0f;
	float PostEatPatrolCooldownRemaining = 0.0f;
	float RandomWanderIdleTimeRemaining = 0.0f;
	float EatTimeRemaining = 0.0f;
	FVector DragTarget = FVector::ZeroVector;
	FVector PreviousActorLocation = FVector::ZeroVector;
	float VisualMovementSpeed = 0.0f;
	float TargetVisualYaw = 0.0f;
	bool bWaitingToConsumeDroppedFood = false;
	bool bHasRandomWanderTarget = false;
	bool bIsEating = false;
	bool bIsSeekingFood = false;
	TObjectPtr<ASquirrelFoodActor> FoodBeingEaten;

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

	void ApplyVisualTuning();
	void ChooseNextRandomWanderTarget();
	void MoveToward(const FVector& Target, float Speed, float DeltaSeconds);
	void FaceMovementDirection(float DeltaX);
	void UpdateVisualFacing(float DeltaSeconds);
	void ApplyGravity(float DeltaSeconds);
	bool UpdateAutoFoodSeeking(float DeltaSeconds);
	void UpdateRandomWander(float DeltaSeconds);
	void UpdateDropFoodConsumption(float DeltaSeconds);
	void UpdateEating(float DeltaSeconds);
	bool TryConsumeNearbyFood();
	bool TryConsumeFood(ASquirrelFoodActor* Food);
	void BeginEatingFood(ASquirrelFoodActor* Food);
	void CompleteEatingFood();
	ASquirrelFoodActor* FindNearestConsumableFood(float SearchRadius) const;
	bool IsFoodCloseEnoughToEat(const ASquirrelFoodActor& Food) const;
	bool FindGround(float TraceDistance, FHitResult& OutHit) const;
	void SnapToGround(const FHitResult& GroundHit);
	FVector ClampToTrainingArea(const FVector& Location) const;
	FVector ClampDragLocation(const FVector& Location) const;
	float GetCurrentMoveSpeed() const;

public:
	UFUNCTION(BlueprintCallable, Category = "Training|Drag")
	void BeginDrag();

	UFUNCTION(BlueprintCallable, Category = "Training|Drag")
	void DragToWorldLocation(const FVector& WorldLocation);

	UFUNCTION(BlueprintCallable, Category = "Training|Drag")
	void EndDrag();

	UFUNCTION(BlueprintCallable, Category = "Training|Stats")
	void AddPower(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Training|Stats")
	void AddEnergyProgress(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Training|Stats")
	void AddEnergy(int32 Amount) { AddEnergyProgress(Amount); }

	UFUNCTION(BlueprintCallable, Category = "Training|Stats")
	void AddEnergyFromFood(int32 FoodAmount = 1);

	UFUNCTION(BlueprintCallable, Category = "Training|Stats")
	bool TrySpendEnergy(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Training|Stats")
	int32 GetPowerLevel() const { return PowerLevel; }

	UFUNCTION(BlueprintPure, Category = "Training|Stats")
	int32 GetEnergyLevel() const { return EnergyLevel; }

	UFUNCTION(BlueprintPure, Category = "Training|Stats")
	int32 GetEnergy() const { return EnergyLevel; }

	UFUNCTION(BlueprintPure, Category = "Training|Stats")
	int32 GetMaxEnergyLevel() const { return MaxEnergyLevel; }

	UFUNCTION(BlueprintPure, Category = "Training|Stats")
	int32 GetMaxEnergy() const { return MaxEnergyLevel; }

	UFUNCTION(BlueprintPure, Category = "Training|Stats")
	int32 GetEnergyProgressPoints() const { return EnergyProgressPoints; }

	UFUNCTION(BlueprintPure, Category = "Training|Stats")
	int32 GetEnergyPointsPerLevel() const { return EnergyPointsPerLevel; }

	UFUNCTION(BlueprintPure, Category = "Training|Stats")
	float GetEnergyProgressPercent() const;

	UFUNCTION(BlueprintPure, Category = "Training|Stats")
	float GetEnergyLevelPercent() const;

	UFUNCTION(BlueprintPure, Category = "Training|Stats")
	float GetMoveSpeed() const { return GetCurrentMoveSpeed(); }

	UFUNCTION(BlueprintPure, Category = "Training|Camera")
	float GetTrainingPlaneY() const { return TrainingPlaneY; }

	UFUNCTION(BlueprintPure, Category = "Training|Movement")
	bool IsGrounded() const { return bIsGrounded; }

	UFUNCTION(BlueprintPure, Category = "Training|Movement")
	bool IsDragging() const { return bIsDragging; }

	UFUNCTION(BlueprintPure, Category = "Training|Movement")
	bool IsMoving() const { return VisualMovementSpeed > 5.0f; }

	UFUNCTION(BlueprintPure, Category = "Training|Food")
	bool IsEating() const { return bIsEating; }

	UFUNCTION(BlueprintPure, Category = "Training|Food")
	bool IsSeekingFood() const { return bIsSeekingFood; }

	UFUNCTION(BlueprintPure, Category = "Training|Movement")
	float GetVisualMovementSpeed() const { return VisualMovementSpeed; }

	UFUNCTION(BlueprintImplementableEvent, Category = "Training|Animation")
	void OnEatingStarted(ASquirrelFoodActor* Food);

	UFUNCTION(BlueprintImplementableEvent, Category = "Training|Animation")
	void OnEatingFinished(ASquirrelFoodActor* Food);
};
