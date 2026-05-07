// Copyright Epic Games, Inc. All Rights Reserved.

#include "Training/SquirrelTrainingCameraActor.h"

#include "Camera/CameraComponent.h"

ASquirrelTrainingCameraActor::ASquirrelTrainingCameraActor()
{
	PrimaryActorTick.bCanEverTick = false;

	UCameraComponent* Camera = GetCameraComponent();
	Camera->ProjectionMode = ECameraProjectionMode::Perspective;
	Camera->FieldOfView = 50.0f;
}

void ASquirrelTrainingCameraActor::BeginPlay()
{
	Super::BeginPlay();

	InitialCameraLocation = GetActorLocation();
}

void ASquirrelTrainingCameraActor::ScrollHorizontal(float Direction, float DeltaSeconds)
{
	if (FMath::IsNearlyZero(Direction) || DeltaSeconds <= 0.0f)
	{
		return;
	}

	FVector DesiredCameraLocation = GetActorLocation();
	DesiredCameraLocation.X += FMath::Clamp(Direction, -1.0f, 1.0f) * EdgeScrollSpeed * DeltaSeconds;

	if (bLockCameraY)
	{
		DesiredCameraLocation.Y = InitialCameraLocation.Y;
	}

	SetActorLocation(ClampCameraLocation(DesiredCameraLocation));
}

FVector ASquirrelTrainingCameraActor::ClampCameraLocation(const FVector& Location) const
{
	if (!bUseRelativeScrollLimits)
	{
		return Location;
	}

	const float MinCameraX = InitialCameraLocation.X - MaxScrollLeft;
	const float MaxCameraX = InitialCameraLocation.X + MaxScrollRight;

	return FVector(
		FMath::Clamp(Location.X, MinCameraX, MaxCameraX),
		Location.Y,
		Location.Z);
}
