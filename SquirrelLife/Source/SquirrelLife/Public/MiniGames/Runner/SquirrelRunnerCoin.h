// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SquirrelRunnerCoin.generated.h"

class USphereComponent;
class UStaticMesh;
class UStaticMeshComponent;

UCLASS(Blueprintable)
class SQUIRRELLIFE_API ASquirrelRunnerCoin : public AActor
{
	GENERATED_BODY()

public:
	ASquirrelRunnerCoin();

	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> CoinMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Coin", meta = (ClampMin = "1"))
	int32 CoinValue = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Coin", meta = (ClampMin = "0", Units = "deg/s"))
	float SpinDegreesPerSecond = 220.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Coin")
	bool bUseBuiltInSpin = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Coin|Cleanup")
	bool bDestroyWhenBehindPlayer = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Coin|Cleanup", meta = (ClampMin = "0", Units = "cm"))
	float DestroyBehindDistance = 1200.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Runner Coin")
	TObjectPtr<AActor> TargetActor;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Runner Coin|Pooling")
	bool bIsPooledInactive = false;

	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleCoinOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool ShouldDestroyBehindPlayer() const;
	void CollectCoin(AActor* Collector);

public:
	UFUNCTION(BlueprintCallable, Category = "Runner Coin")
	void SetTargetActor(AActor* NewTargetActor) { TargetActor = NewTargetActor; }

	UFUNCTION(BlueprintCallable, Category = "Runner Coin")
	void SetCoinValue(int32 NewCoinValue) { CoinValue = FMath::Max(NewCoinValue, 1); }

	UFUNCTION(BlueprintCallable, Category = "Runner Coin")
	void SetCoinMesh(UStaticMesh* NewMesh);

	UFUNCTION(BlueprintCallable, Category = "Runner Coin|Cleanup")
	void SetDestroyWhenBehindPlayer(bool bShouldDestroy) { bDestroyWhenBehindPlayer = bShouldDestroy; }

	UFUNCTION(BlueprintCallable, Category = "Runner Coin|Pooling")
	void ActivateFromPool(const FVector& NewLocation, const FRotator& NewRotation, AActor* NewTargetActor);

	UFUNCTION(BlueprintCallable, Category = "Runner Coin|Pooling")
	void DeactivateForPool();

	UFUNCTION(BlueprintPure, Category = "Runner Coin|Pooling")
	bool IsPooledInactive() const { return bIsPooledInactive; }

	UFUNCTION(BlueprintPure, Category = "Runner Coin|Pooling")
	bool HasPassedTarget(float BehindDistance) const;
};
