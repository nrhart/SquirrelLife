// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SquirrelRunnerObstacle.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UStaticMesh;

UENUM(BlueprintType)
enum class ESquirrelRunnerObstaclePattern : uint8
{
	Rolling,
	BouncingArc,
	Flying
};

UCLASS(Blueprintable)
class SQUIRRELLIFE_API ASquirrelRunnerObstacle : public AActor
{
	GENERATED_BODY()

public:
	ASquirrelRunnerObstacle();

	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ObstacleMesh;

	UPROPERTY(Transient)
	TObjectPtr<UStaticMesh> DefaultObstacleMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle")
	ESquirrelRunnerObstaclePattern MovementPattern = ESquirrelRunnerObstaclePattern::Rolling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle")
	bool bAutoFindPlayer = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle")
	bool bAutoMove = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle", meta = (ClampMin = "0", Units = "cm/s"))
	float MoveSpeed = 325.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle")
	FVector MoveDirection = FVector(-1.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle|Rolling", meta = (ClampMin = "0", Units = "deg/s"))
	float RollDegreesPerSecond = 540.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle|Bouncing", meta = (ClampMin = "0", Units = "cm"))
	float BounceHeight = 220.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle|Bouncing", meta = (ClampMin = "0"))
	float BounceFrequency = 0.65f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle|Cleanup")
	bool bDestroyWhenBehindPlayer = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle|Cleanup", meta = (ClampMin = "0", Units = "cm"))
	float DestroyBehindDistance = 1200.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Runner Obstacle")
	TObjectPtr<AActor> TargetActor;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Runner Obstacle|Pooling")
	bool bIsPooledInactive = false;

	float ElapsedTime = 0.0f;
	float BaseZ = 0.0f;

	virtual void BeginPlay() override;

	void FindPlayerTarget();
	void UpdateMovement(float DeltaSeconds);
	void UpdateRollingVisual(float DeltaSeconds);
	bool ShouldDestroyBehindPlayer() const;

	UFUNCTION()
	void HandleObstacleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	UFUNCTION(BlueprintCallable, Category = "Runner Obstacle")
	void InitializeObstacle(ESquirrelRunnerObstaclePattern NewPattern, AActor* NewTargetActor);

	UFUNCTION(BlueprintCallable, Category = "Runner Obstacle")
	void SetTargetActor(AActor* NewTargetActor);

	UFUNCTION(BlueprintCallable, Category = "Runner Obstacle")
	void SetMovementPattern(ESquirrelRunnerObstaclePattern NewPattern);

	UFUNCTION(BlueprintCallable, Category = "Runner Obstacle")
	void SetMoveSpeed(float NewMoveSpeed);

	UFUNCTION(BlueprintCallable, Category = "Runner Obstacle")
	void SetBounceArc(float NewBounceHeight, float NewBounceFrequency);

	UFUNCTION(BlueprintCallable, Category = "Runner Obstacle")
	void SetObstacleMesh(UStaticMesh* NewMesh);

	UFUNCTION(BlueprintCallable, Category = "Runner Obstacle|Pooling")
	void ActivateFromPool(const FVector& NewLocation, const FRotator& NewRotation, ESquirrelRunnerObstaclePattern NewPattern, AActor* NewTargetActor);

	UFUNCTION(BlueprintCallable, Category = "Runner Obstacle|Pooling")
	void DeactivateForPool();

	UFUNCTION(BlueprintCallable, Category = "Runner Obstacle|Pooling")
	void SetDestroyWhenBehindPlayer(bool bShouldDestroy) { bDestroyWhenBehindPlayer = bShouldDestroy; }

	UFUNCTION(BlueprintPure, Category = "Runner Obstacle|Pooling")
	bool IsPooledInactive() const { return bIsPooledInactive; }

	UFUNCTION(BlueprintPure, Category = "Runner Obstacle|Pooling")
	bool HasPassedTarget(float BehindDistance) const;

	UFUNCTION(BlueprintPure, Category = "Runner Obstacle")
	ESquirrelRunnerObstaclePattern GetMovementPattern() const { return MovementPattern; }

	UFUNCTION(BlueprintPure, Category = "Runner Obstacle")
	UStaticMeshComponent* GetObstacleMesh() const { return ObstacleMesh; }
};
