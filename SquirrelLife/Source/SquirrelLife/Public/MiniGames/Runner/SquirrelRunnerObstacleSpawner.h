// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MiniGames/Runner/SquirrelRunnerObstacle.h"
#include "SquirrelRunnerObstacleSpawner.generated.h"

class UStaticMesh;

USTRUCT(BlueprintType)
struct FSquirrelRunnerObstacleSpawnOption
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle")
	TSubclassOf<ASquirrelRunnerObstacle> ObstacleClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle")
	ESquirrelRunnerObstaclePattern Pattern = ESquirrelRunnerObstaclePattern::Rolling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle", meta = (ClampMin = "0"))
	float Weight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle")
	FVector SpawnOffset = FVector::ZeroVector;
};

UCLASS(Blueprintable)
class SQUIRRELLIFE_API ASquirrelRunnerObstacleSpawner : public AActor
{
	GENERATED_BODY()

public:
	ASquirrelRunnerObstacleSpawner();

	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle Spawner|Fixed Types")
	bool bUseFixedThreeObstacleTypes = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle Spawner|Fixed Types", meta = (EditCondition = "bUseFixedThreeObstacleTypes"))
	TSubclassOf<ASquirrelRunnerObstacle> RollingObstacleClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle Spawner|Fixed Types", meta = (EditCondition = "bUseFixedThreeObstacleTypes"))
	TSubclassOf<ASquirrelRunnerObstacle> BouncingObstacleClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle Spawner|Fixed Types", meta = (EditCondition = "bUseFixedThreeObstacleTypes"))
	TSubclassOf<ASquirrelRunnerObstacle> FlyingObstacleClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle Spawner|Meshes", meta = (EditCondition = "bUseFixedThreeObstacleTypes"))
	TObjectPtr<UStaticMesh> RollingObstacleMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle Spawner|Meshes", meta = (EditCondition = "bUseFixedThreeObstacleTypes"))
	TObjectPtr<UStaticMesh> BouncingObstacleMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle Spawner|Meshes", meta = (EditCondition = "bUseFixedThreeObstacleTypes"))
	TObjectPtr<UStaticMesh> FlyingObstacleMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle Spawner")
	bool bAutoFindPlayer = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle Spawner")
	bool bAutoSpawn = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle Spawner", meta = (ClampMin = "0", Units = "s"))
	float MinSpawnDelay = 1.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle Spawner", meta = (ClampMin = "0", Units = "s"))
	float MaxSpawnDelay = 2.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle Spawner", meta = (Units = "cm"))
	FVector SpawnOffsetFromPlayer = FVector(1200.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle Spawner", meta = (ClampMin = "0", Units = "cm/s"))
	float SpawnedObstacleSpeed = 325.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle Spawner|Bouncing", meta = (ClampMin = "0", Units = "cm"))
	float SpawnedBounceHeight = 220.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle Spawner|Bouncing", meta = (ClampMin = "0"))
	float SpawnedBounceFrequency = 0.65f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle Spawner|Lanes")
	bool bUseFixedLaneZ = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle Spawner|Lanes", meta = (EditCondition = "bUseFixedLaneZ", Units = "cm"))
	float GroundObstacleZ = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle Spawner|Lanes", meta = (EditCondition = "bUseFixedLaneZ", Units = "cm"))
	float BouncingObstacleBaseZ = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle Spawner|Lanes", meta = (EditCondition = "bUseFixedLaneZ", Units = "cm"))
	float FlyingObstacleZ = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle Spawner|Pooling")
	bool bUseObstaclePooling = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle Spawner|Pooling", meta = (EditCondition = "bUseObstaclePooling", ClampMin = "1"))
	int32 MaxPooledObstacles = 18;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle Spawner|Pooling", meta = (EditCondition = "bUseObstaclePooling", ClampMin = "0", Units = "cm"))
	float PoolRecycleBehindDistance = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Obstacle Spawner")
	TArray<FSquirrelRunnerObstacleSpawnOption> SpawnOptions;

	UPROPERTY(BlueprintReadOnly, Category = "Runner Obstacle Spawner")
	TObjectPtr<AActor> TargetActor;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Runner Obstacle Spawner|Pooling")
	TArray<TObjectPtr<ASquirrelRunnerObstacle>> PooledObstacles;

	float SpawnTimeRemaining = 0.0f;

	virtual void BeginPlay() override;

	void FindPlayerTarget();
	void ResetSpawnTimer();
	const FSquirrelRunnerObstacleSpawnOption* ChooseSpawnOption() const;
	ESquirrelRunnerObstaclePattern ChooseFixedPattern() const;
	TSubclassOf<ASquirrelRunnerObstacle> GetObstacleClassForPattern(ESquirrelRunnerObstaclePattern Pattern) const;
	UStaticMesh* GetObstacleMeshForPattern(ESquirrelRunnerObstaclePattern Pattern) const;
	float GetSpawnZForPattern(ESquirrelRunnerObstaclePattern Pattern) const;
	ASquirrelRunnerObstacle* GetOrCreateObstacle(TSubclassOf<ASquirrelRunnerObstacle> ObstacleClass, const FVector& SpawnLocation, const FRotator& SpawnRotation);
	ASquirrelRunnerObstacle* CreateObstacle(TSubclassOf<ASquirrelRunnerObstacle> ObstacleClass, const FVector& SpawnLocation, const FRotator& SpawnRotation);
	void UpdatePooledObstacles();
	void PrunePooledObstacles();

public:
	UFUNCTION(BlueprintCallable, Category = "Runner Obstacle Spawner")
	void SetTargetActor(AActor* NewTargetActor);

	UFUNCTION(BlueprintCallable, Category = "Runner Obstacle Spawner")
	ASquirrelRunnerObstacle* SpawnObstacle();
};
