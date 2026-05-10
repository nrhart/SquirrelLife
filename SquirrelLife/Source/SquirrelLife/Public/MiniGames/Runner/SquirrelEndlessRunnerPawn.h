// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MiniGames/SquirrelMiniGamePawnBase.h"
#include "SquirrelEndlessRunnerPawn.generated.h"

UCLASS(Blueprintable)
class SQUIRRELLIFE_API ASquirrelEndlessRunnerPawn : public ASquirrelMiniGamePawnBase
{
	GENERATED_BODY()

public:
	ASquirrelEndlessRunnerPawn();

	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner|Movement")
	bool bAutoRun = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner|Movement")
	bool bRunInPlace = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner|Movement")
	FVector AutoRunDirection = FVector(1.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner|Death", meta = (ClampMin = "0", Units = "cm/s"))
	float DeathHopVelocity = 620.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner|Death", meta = (ClampMin = "0", Units = "cm/s"))
	float DeathTowardCameraVelocity = 520.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner|Death")
	FVector DeathTowardCameraDirection = FVector(0.0f, 1.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner|Death", meta = (ClampMin = "0", Units = "deg/s"))
	float DeathSpinSpeed = 540.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner|Death")
	bool bDisableCollisionOnDeath = true;

	UPROPERTY(BlueprintReadOnly, Category = "Runner|Death")
	bool bIsDeathSequencePlaying = false;

	virtual float GetMiniGameSpeed() const override;

public:
	UFUNCTION(BlueprintCallable, Category = "Runner|Death")
	void PlayDeathSequence();

	UFUNCTION(BlueprintPure, Category = "Runner|Death")
	bool IsDeathSequencePlaying() const { return bIsDeathSequencePlaying; }

	UFUNCTION(BlueprintCallable, Category = "Runner|Rewards")
	void AwardRunScore(int32 ScoreUnits);

	UFUNCTION(BlueprintPure, Category = "Runner|Animation")
	float GetRunnerSpeed() const;

	UFUNCTION(BlueprintPure, Category = "Runner|Animation")
	bool IsRunnerMoving() const { return GetRunnerSpeed() > 5.0f; }
};
