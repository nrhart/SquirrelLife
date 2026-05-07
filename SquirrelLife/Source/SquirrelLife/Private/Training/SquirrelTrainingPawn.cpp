// Copyright Epic Games, Inc. All Rights Reserved.

#include "Training/SquirrelTrainingPawn.h"

#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Training/SquirrelFoodActor.h"

ASquirrelTrainingPawn::ASquirrelTrainingPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	Collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	Collision->InitCapsuleSize(38.0f, 58.0f);
	Collision->SetCapsuleHalfHeight(58.0f);
	Collision->SetCapsuleRadius(38.0f);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collision->SetCollisionObjectType(ECC_Pawn);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Collision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Collision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	Collision->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Collision->SetGenerateOverlapEvents(true);

	VisualRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VisualRoot"));
	VisualRoot->SetupAttachment(RootComponent);

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(VisualRoot);
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BodyMesh->SetRelativeScale3D(FVector(0.85f, 0.36f, 0.62f));

	TailMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TailMesh"));
	TailMesh->SetupAttachment(VisualRoot);
	TailMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TailMesh->SetRelativeLocation(FVector(-42.0f, 0.0f, 22.0f));
	TailMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 32.0f));
	TailMesh->SetRelativeScale3D(FVector(0.42f, 0.25f, 0.92f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		BodyMesh->SetStaticMesh(SphereMesh.Object);
		TailMesh->SetStaticMesh(SphereMesh.Object);
	}
}

void ASquirrelTrainingPawn::BeginPlay()
{
	Super::BeginPlay();

	if (bUseSpawnYAsTrainingPlane)
	{
		TrainingPlaneY = GetActorLocation().Y;
	}

	PatrolCenterX = GetActorLocation().X;
	PatrolCenterY = GetActorLocation().Y;
	PatrolDirection = 1.0f;
	ChooseNextRandomWanderTarget();
	bHasRandomWanderTarget = false;
	RandomWanderIdleTimeRemaining = FMath::FRandRange(RandomWanderMinIdleTime, RandomWanderMaxIdleTime);

	SetActorLocation(ClampToTrainingArea(GetActorLocation()));
	FHitResult GroundHit;
	if (FindGround(GroundProbeDistance, GroundHit))
	{
		SnapToGround(GroundHit);
	}

	PatrolCenterX = GetActorLocation().X;
	PatrolCenterY = GetActorLocation().Y;
}

void ASquirrelTrainingPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsDragging)
	{
		if (!bSnapToMouseWhileDragging)
		{
			MoveToward(DragTarget, DragFollowSpeed, DeltaSeconds);
		}

		VerticalVelocity = 0.0f;
		bIsGrounded = false;
		return;
	}

	if (FoodEatCooldownRemaining > 0.0f)
	{
		FoodEatCooldownRemaining = FMath::Max(FoodEatCooldownRemaining - DeltaSeconds, 0.0f);
	}

	if (!UpdateAutoFoodSeeking(DeltaSeconds))
	{
		UpdateRandomWander(DeltaSeconds);
	}

	ApplyGravity(DeltaSeconds);
	UpdateDropFoodConsumption(DeltaSeconds);
}

void ASquirrelTrainingPawn::ChooseNextRandomWanderTarget()
{
	const float WanderMin = FMath::Min(RandomWanderMinDistance, RandomWanderMaxDistance);
	const float WanderMax = FMath::Max(RandomWanderMinDistance, RandomWanderMaxDistance);
	const float WanderDistance = FMath::FRandRange(WanderMin, WanderMax);
	const float DepthWanderMin = FMath::Min(RandomWanderMinDepthDistance, RandomWanderMaxDepthDistance);
	const float DepthWanderMax = FMath::Max(RandomWanderMinDepthDistance, RandomWanderMaxDepthDistance);
	const float DepthWanderDistance = FMath::FRandRange(DepthWanderMin, DepthWanderMax);
	const float DepthDirection = FMath::RandBool() ? 1.0f : -1.0f;
	PatrolDirection = FMath::RandBool() ? 1.0f : -1.0f;
	PatrolTargetX = GetActorLocation().X + PatrolDirection * WanderDistance;
	PatrolTargetY = GetActorLocation().Y + DepthDirection * DepthWanderDistance;
	PatrolTargetY = FMath::Clamp(
		PatrolTargetY,
		PatrolCenterY - RandomWanderMaxDepthOffset,
		PatrolCenterY + RandomWanderMaxDepthOffset);

	if (bConstrainToTrainingBounds)
	{
		PatrolTargetX = FMath::Clamp(PatrolTargetX, TrainingBoundsX.X, TrainingBoundsX.Y);
	}

	bHasRandomWanderTarget = true;
}

void ASquirrelTrainingPawn::MoveToward(const FVector& Target, float Speed, float DeltaSeconds)
{
	const FVector CurrentLocation = GetActorLocation();
	const FVector ClampedTarget = ClampToTrainingArea(Target);
	FVector NewLocation = CurrentLocation;
	NewLocation.X = FMath::FInterpConstantTo(CurrentLocation.X, ClampedTarget.X, DeltaSeconds, Speed);
	NewLocation.Y = FMath::FInterpConstantTo(CurrentLocation.Y, ClampedTarget.Y, DeltaSeconds, Speed);

	if (bIsDragging)
	{
		NewLocation.Z = FMath::FInterpConstantTo(CurrentLocation.Z, ClampDragLocation(Target).Z, DeltaSeconds, Speed);
	}

	SetActorLocation(ClampToTrainingArea(NewLocation), false);

	const float DeltaX = NewLocation.X - CurrentLocation.X;
	const float DeltaY = NewLocation.Y - CurrentLocation.Y;
	if (!FMath::IsNearlyZero(DeltaX, 0.1f) || !FMath::IsNearlyZero(DeltaY, 0.1f))
	{
		const float FacingYaw = DeltaX >= 0.0f ? 0.0f : 180.0f;
		VisualRoot->SetRelativeRotation(FRotator(0.0f, FacingYaw, 0.0f));
	}
}

void ASquirrelTrainingPawn::ApplyGravity(float DeltaSeconds)
{
	FHitResult GroundHit;
	const bool bFoundGround = FindGround(GroundProbeDistance, GroundHit);

	if (bFoundGround)
	{
		const float GroundedActorZ = GroundHit.ImpactPoint.Z + Collision->GetScaledCapsuleHalfHeight();
		const float DistanceToGround = GetActorLocation().Z - GroundedActorZ;
		if (DistanceToGround <= GroundSnapDistance && VerticalVelocity <= 0.0f)
		{
			SnapToGround(GroundHit);
			return;
		}
	}

	bIsGrounded = false;
	VerticalVelocity = FMath::Max(VerticalVelocity - GravityAcceleration * DeltaSeconds, -TerminalFallSpeed);

	FVector NewLocation = GetActorLocation();
	NewLocation.Z += VerticalVelocity * DeltaSeconds;

	if (bFoundGround)
	{
		const float GroundedActorZ = GroundHit.ImpactPoint.Z + Collision->GetScaledCapsuleHalfHeight();
		if (NewLocation.Z <= GroundedActorZ)
		{
			NewLocation.Z = GroundedActorZ;
			SetActorLocation(ClampToTrainingArea(NewLocation), false);
			SnapToGround(GroundHit);
			return;
		}
	}

	SetActorLocation(ClampToTrainingArea(NewLocation), false);
}

bool ASquirrelTrainingPawn::UpdateAutoFoodSeeking(float DeltaSeconds)
{
	if (FoodEatCooldownRemaining > 0.0f)
	{
		return false;
	}

	ASquirrelFoodActor* Food = FindNearestConsumableFood(FoodSeekRadius);
	if (!Food)
	{
		return false;
	}

	if (IsFoodCloseEnoughToEat(*Food))
	{
		TryConsumeFood(Food);
		return true;
	}

	MoveToward(FVector(Food->GetActorLocation().X, Food->GetActorLocation().Y, GetActorLocation().Z), GetCurrentMoveSpeed(), DeltaSeconds);
	return true;
}

void ASquirrelTrainingPawn::UpdateRandomWander(float DeltaSeconds)
{
	if (!bHasRandomWanderTarget)
	{
		RandomWanderIdleTimeRemaining -= DeltaSeconds;
		if (RandomWanderIdleTimeRemaining > 0.0f)
		{
			return;
		}

		ChooseNextRandomWanderTarget();
	}

	MoveToward(FVector(PatrolTargetX, PatrolTargetY, GetActorLocation().Z), GetCurrentMoveSpeed(), DeltaSeconds);

	if (FVector2D::Distance(
		FVector2D(GetActorLocation().X, GetActorLocation().Y),
		FVector2D(PatrolTargetX, PatrolTargetY)) <= PatrolAcceptanceRadius)
	{
		bHasRandomWanderTarget = false;
		RandomWanderIdleTimeRemaining = FMath::FRandRange(RandomWanderMinIdleTime, RandomWanderMaxIdleTime);
	}
}

void ASquirrelTrainingPawn::UpdateDropFoodConsumption(float DeltaSeconds)
{
	if (!bWaitingToConsumeDroppedFood)
	{
		return;
	}

	DropConsumeTimeRemaining -= DeltaSeconds;
	if (DropConsumeTimeRemaining <= 0.0f)
	{
		bWaitingToConsumeDroppedFood = false;
		return;
	}

	if (bIsGrounded)
	{
		TryConsumeNearbyFood();
		bWaitingToConsumeDroppedFood = false;
	}
}

bool ASquirrelTrainingPawn::TryConsumeNearbyFood()
{
	ASquirrelFoodActor* Food = FindNearestConsumableFood(FoodConsumeRadius);
	if (!Food)
	{
		return false;
	}

	return TryConsumeFood(Food);
}

bool ASquirrelTrainingPawn::TryConsumeFood(ASquirrelFoodActor* Food)
{
	if (!Food || FoodEatCooldownRemaining > 0.0f)
	{
		return false;
	}

	if (!Food->TryConsume(this))
	{
		return false;
	}

	FoodEatCooldownRemaining = FoodEatCooldown;
	return true;
}

ASquirrelFoodActor* ASquirrelTrainingPawn::FindNearestConsumableFood(float SearchRadius) const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	ASquirrelFoodActor* BestFood = nullptr;
	float BestDistanceSquared = TNumericLimits<float>::Max();
	const FVector SquirrelLocation = GetActorLocation();

	for (TActorIterator<ASquirrelFoodActor> It(World); It; ++It)
	{
		ASquirrelFoodActor* Food = *It;
		if (!Food || !Food->IsAvailableToConsume())
		{
			continue;
		}

		const float AllowedDistance = SearchRadius + Food->GetConsumeRadius();
		const float DistanceSquared = FVector::DistSquared(SquirrelLocation, Food->GetActorLocation());
		if (DistanceSquared <= FMath::Square(AllowedDistance) && DistanceSquared < BestDistanceSquared)
		{
			BestDistanceSquared = DistanceSquared;
			BestFood = Food;
		}
	}

	return BestFood;
}

bool ASquirrelTrainingPawn::IsFoodCloseEnoughToEat(const ASquirrelFoodActor& Food) const
{
	const float AllowedDistance = FoodConsumeRadius + Food.GetConsumeRadius();
	return FVector::DistSquared(GetActorLocation(), Food.GetActorLocation()) <= FMath::Square(AllowedDistance);
}

bool ASquirrelTrainingPawn::FindGround(float TraceDistance, FHitResult& OutHit) const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const FVector Start = GetActorLocation();
	const FVector End = Start - FVector::UpVector * TraceDistance;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SquirrelGroundTrace), false);
	QueryParams.AddIgnoredActor(this);

	return World->LineTraceSingleByChannel(OutHit, Start, End, GroundTraceChannel, QueryParams);
}

void ASquirrelTrainingPawn::SnapToGround(const FHitResult& GroundHit)
{
	FVector GroundedLocation = GetActorLocation();
	GroundedLocation.Z = GroundHit.ImpactPoint.Z + Collision->GetScaledCapsuleHalfHeight();
	SetActorLocation(ClampToTrainingArea(GroundedLocation), false);

	VerticalVelocity = 0.0f;
	bIsGrounded = true;
}

FVector ASquirrelTrainingPawn::ClampToTrainingArea(const FVector& Location) const
{
	if (!bConstrainToTrainingBounds)
	{
		return Location;
	}

	return FVector(
		FMath::Clamp(Location.X, TrainingBoundsX.X, TrainingBoundsX.Y),
		Location.Y,
		FMath::Clamp(Location.Z, TrainingBoundsZ.X, TrainingBoundsZ.Y));
}

FVector ASquirrelTrainingPawn::ClampDragLocation(const FVector& Location) const
{
	if (!bConstrainToTrainingBounds)
	{
		return FVector(Location.X, TrainingPlaneY, Location.Z);
	}

	return FVector(
		FMath::Clamp(Location.X, TrainingBoundsX.X, TrainingBoundsX.Y),
		TrainingPlaneY,
		FMath::Clamp(Location.Z, TrainingBoundsZ.X, TrainingBoundsZ.Y));
}

float ASquirrelTrainingPawn::GetCurrentMoveSpeed() const
{
	return BaseMoveSpeed + (PowerLevel * SpeedPerPower);
}

void ASquirrelTrainingPawn::BeginDrag()
{
	bIsDragging = true;
	bWaitingToConsumeDroppedFood = false;
	bHasRandomWanderTarget = false;
	DropConsumeTimeRemaining = 0.0f;
	DragTarget = GetActorLocation();
}

void ASquirrelTrainingPawn::DragToWorldLocation(const FVector& WorldLocation)
{
	if (!bIsDragging)
	{
		return;
	}

	DragTarget = ClampDragLocation(WorldLocation);

	if (bSnapToMouseWhileDragging)
	{
		const FVector PreviousLocation = GetActorLocation();
		SetActorLocation(DragTarget, false);

		const float DeltaX = DragTarget.X - PreviousLocation.X;
		if (!FMath::IsNearlyZero(DeltaX, 0.1f))
		{
			const float FacingYaw = DeltaX >= 0.0f ? 0.0f : 180.0f;
			VisualRoot->SetRelativeRotation(FRotator(0.0f, FacingYaw, 0.0f));
		}
	}
}

void ASquirrelTrainingPawn::EndDrag()
{
	bIsDragging = false;
	bWaitingToConsumeDroppedFood = true;
	DropConsumeTimeRemaining = DropConsumeWindow;
	if (TryConsumeNearbyFood())
	{
		bWaitingToConsumeDroppedFood = false;
	}
	PatrolCenterX = GetActorLocation().X;
	PatrolCenterY = GetActorLocation().Y;
	PatrolDirection = FMath::IsNearlyZero(PatrolDirection) ? 1.0f : PatrolDirection;
	bHasRandomWanderTarget = false;
	RandomWanderIdleTimeRemaining = FMath::FRandRange(RandomWanderMinIdleTime, RandomWanderMaxIdleTime);
}

void ASquirrelTrainingPawn::AddPower(int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	PowerLevel += Amount;
	OnPowerChanged.Broadcast(PowerLevel, GetCurrentMoveSpeed());

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			1.6f,
			FColor::Green,
			FString::Printf(TEXT("Power +%d  |  Total Power: %d  |  Speed: %.0f"), Amount, PowerLevel, GetCurrentMoveSpeed()));
	}
}
