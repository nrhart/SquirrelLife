// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "SquirrelTrainingCameraActor.generated.h"

/**
 * Placeable perspective camera for ParkLevel.
 * The player scrolls it left/right by moving the mouse near the screen edges.
 */
UCLASS(Blueprintable)
class SQUIRRELLIFE_API ASquirrelTrainingCameraActor : public ACameraActor
{
	GENERATED_BODY()

public:
	ASquirrelTrainingCameraActor();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Camera", meta = (ClampMin = "0"))
	float EdgeScrollSpeed = 650.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Camera")
	bool bLockCameraY = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Camera")
	bool bUseRelativeScrollLimits = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Camera", meta = (EditCondition = "bUseRelativeScrollLimits", ClampMin = "0", Units = "cm"))
	float MaxScrollLeft = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Camera", meta = (EditCondition = "bUseRelativeScrollLimits", ClampMin = "0", Units = "cm"))
	float MaxScrollRight = 1200.0f;

	FVector InitialCameraLocation = FVector::ZeroVector;

	virtual void BeginPlay() override;

	FVector ClampCameraLocation(const FVector& Location) const;

public:
	UFUNCTION(BlueprintCallable, Category = "Training|Camera")
	void ScrollHorizontal(float Direction, float DeltaSeconds);

	UFUNCTION(BlueprintPure, Category = "Training|Camera")
	float GetEdgeScrollSpeed() const { return EdgeScrollSpeed; }
};
