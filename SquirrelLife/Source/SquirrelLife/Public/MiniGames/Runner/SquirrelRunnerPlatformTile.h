// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SquirrelRunnerPlatformTile.generated.h"

class USceneComponent;
class UStaticMeshComponent;

UCLASS(Blueprintable)
class SQUIRRELLIFE_API ASquirrelRunnerPlatformTile : public AActor
{
	GENERATED_BODY()

public:
	ASquirrelRunnerPlatformTile();

	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PlatformMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Tile")
	bool bAutoFindPlayer = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Tile")
	bool bAutoRecycle = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Tile", meta = (ClampMin = "1", Units = "cm"))
	float TileLength = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Tile", meta = (ClampMin = "1"))
	int32 TileCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Tile", meta = (ClampMin = "0", Units = "cm"))
	float RecycleBehindDistance = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner Tile")
	FVector RecycleDirection = FVector(1.0f, 0.0f, 0.0f);

	UPROPERTY(BlueprintReadOnly, Category = "Runner Tile")
	TObjectPtr<AActor> TargetActor;

	virtual void BeginPlay() override;

	void FindPlayerTarget();
	bool ShouldRecycle() const;

public:
	UFUNCTION(BlueprintCallable, Category = "Runner Tile")
	void SetTargetActor(AActor* NewTargetActor);

	UFUNCTION(BlueprintCallable, Category = "Runner Tile")
	void RecycleTile();

	UFUNCTION(BlueprintPure, Category = "Runner Tile")
	float GetLoopDistance() const { return TileLength * static_cast<float>(FMath::Max(TileCount, 1)); }

	UFUNCTION(BlueprintPure, Category = "Runner Tile")
	UStaticMeshComponent* GetPlatformMesh() const { return PlatformMesh; }
};
