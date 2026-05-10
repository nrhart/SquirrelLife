// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SquirrelRunnerCoinSpawner.generated.h"

class ASquirrelRunnerCoin;
class UStaticMesh;

UCLASS(Blueprintable)
class SQUIRRELLIFE_API ASquirrelRunnerCoinSpawner : public AActor
{
	GENERATED_BODY()

public:
	ASquirrelRunnerCoinSpawner();

	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Coin Spawner")
	bool bAutoFindPlayer = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Coin Spawner")
	bool bAutoSpawn = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Coin Spawner")
	TSubclassOf<ASquirrelRunnerCoin> CoinClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Coin Spawner")
	TObjectPtr<UStaticMesh> CoinMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Coin Spawner", meta = (ClampMin = "1"))
	int32 CoinValue = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Coin Spawner", meta = (ClampMin = "1"))
	int32 DesiredActiveCoinCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Coin Spawner", meta = (ClampMin = "0", Units = "s"))
	float MinSpawnDelay = 1.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Coin Spawner", meta = (ClampMin = "0", Units = "s"))
	float MaxSpawnDelay = 3.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Coin Spawner", meta = (Units = "cm"))
	FVector SpawnOffsetFromPlayer = FVector(1100.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Coin Spawner", meta = (ClampMin = "0", Units = "cm"))
	float CoinSpacingX = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Coin Spawner", meta = (Units = "cm"))
	float MinSpawnZ = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Coin Spawner", meta = (Units = "cm"))
	float MaxSpawnZ = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Coin Spawner", meta = (ClampMin = "0", Units = "cm"))
	float SpawnYRange = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Coin Spawner")
	FRotator CoinSpawnRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Coin Spawner")
	bool bUseRandomSpawnYaw = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Coin Spawner|Pooling")
	bool bUseCoinPooling = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Coin Spawner|Pooling", meta = (EditCondition = "bUseCoinPooling", ClampMin = "1"))
	int32 MaxPooledCoins = 24;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Coin Spawner|Pooling", meta = (EditCondition = "bUseCoinPooling", ClampMin = "0", Units = "cm"))
	float PoolRecycleBehindDistance = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Coin Spawner|Recovery", meta = (ClampMin = "0", Units = "s"))
	float FailedSpawnRetryDelay = 0.15f;

	UPROPERTY(BlueprintReadOnly, Category = "Runner Coin Spawner")
	TObjectPtr<AActor> TargetActor;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Runner Coin Spawner|Pooling")
	TArray<TObjectPtr<ASquirrelRunnerCoin>> PooledCoins;

	float SpawnTimeRemaining = 0.0f;

	virtual void BeginPlay() override;

	void FindPlayerTarget();
	void ResetSpawnTimer();
	void RetrySpawnSoon();
	FVector GetCoinSpawnLocation(int32 CoinSlotIndex) const;
	ASquirrelRunnerCoin* CreateCoin(const FVector& SpawnLocation, const FRotator& SpawnRotation);
	ASquirrelRunnerCoin* GetOrCreateCoin(const FVector& SpawnLocation, const FRotator& SpawnRotation);
	void UpdatePooledCoins();
	void PrunePooledCoins();
	int32 GetActiveCoinCount() const;
	ASquirrelRunnerCoin* SpawnCoinForSlot(int32 CoinSlotIndex);

public:
	UFUNCTION(BlueprintCallable, Category = "Runner Coin Spawner")
	void SetTargetActor(AActor* NewTargetActor) { TargetActor = NewTargetActor; }

	UFUNCTION(BlueprintCallable, Category = "Runner Coin Spawner")
	ASquirrelRunnerCoin* SpawnCoin();
};
