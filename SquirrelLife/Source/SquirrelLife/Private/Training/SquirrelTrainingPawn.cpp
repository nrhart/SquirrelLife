// Copyright Epic Games, Inc. All Rights Reserved.

#include "Training/SquirrelTrainingPawn.h"

#include "Animation/AnimInstance.h"
#include "Audio/SquirrelAudioManager.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Progress/SquirrelGameInstance.h"
#include "Progress/SquirrelProgressTuningData.h"
#include "Progress/SquirrelTuningComponent.h"
#include "Training/SquirrelFoodActor.h"
#include "Training/SquirrelTrainingGameMode.h"

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

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = Collision;
	MovementComponent->MaxSpeed = 0.0f;
	MovementComponent->Acceleration = 0.0f;
	MovementComponent->Deceleration = 0.0f;
	MovementComponent->TurningBoost = 0.0f;

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

	SquirrelMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SquirrelMesh"));
	SquirrelMesh->SetupAttachment(VisualRoot);
	SquirrelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SquirrelMesh->SetVisibility(false);

	TuningComponent = CreateDefaultSubobject<USquirrelTuningComponent>(TEXT("TuningComponent"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		BodyMesh->SetStaticMesh(SphereMesh.Object);
		TailMesh->SetStaticMesh(SphereMesh.Object);
	}

	ApplyVisualTuning();
}

void ASquirrelTrainingPawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ApplyVisualTuning();
}

void ASquirrelTrainingPawn::BeginPlay()
{
	Super::BeginPlay();
	ApplyVisualTuning();
	TargetVisualYaw = VisualRoot ? VisualRoot->GetRelativeRotation().Yaw : 0.0f;
	PreviousActorLocation = GetActorLocation();
	SpawnLocation = GetActorLocation();
	LastSafeGroundedLocation = SpawnLocation;
	if (GetProgressGameInstance())
	{
		SyncEnergyFromProgress();
	}
	else
	{
		ApplyLocalProgressTuning();
		EnergyLevel = FMath::Clamp(StartingEnergyLevel, 0, MaxEnergyLevel);
		EnergyProgressPoints = 0;
	}
	OnEnergyChanged.Broadcast(EnergyLevel, EnergyProgressPoints, MaxEnergyLevel);

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

	if (DeltaSeconds > 0.0f)
	{
		VisualMovementSpeed = FVector::Dist(GetActorLocation(), PreviousActorLocation) / DeltaSeconds;
		PreviousActorLocation = GetActorLocation();
	}

	if (bIsDragging)
	{
		if (!bSnapToMouseWhileDragging)
		{
			MoveToward(DragTarget, DragFollowSpeed, DeltaSeconds);
		}

		VerticalVelocity = 0.0f;
		bIsGrounded = false;
		UpdateVisualFacing(DeltaSeconds);
		return;
	}

	if (bIsEating)
	{
		bIsSeekingFood = false;
		UpdateEating(DeltaSeconds);
		ApplyGravity(DeltaSeconds);
		UpdateVisualFacing(DeltaSeconds);
		return;
	}

	if (FoodEatCooldownRemaining > 0.0f)
	{
		FoodEatCooldownRemaining = FMath::Max(FoodEatCooldownRemaining - DeltaSeconds, 0.0f);
	}

	if (PostEatPatrolCooldownRemaining > 0.0f)
	{
		PostEatPatrolCooldownRemaining = FMath::Max(PostEatPatrolCooldownRemaining - DeltaSeconds, 0.0f);
		ApplyGravity(DeltaSeconds);
		UpdateDropFoodConsumption(DeltaSeconds);
		UpdateVisualFacing(DeltaSeconds);
		return;
	}

	bIsSeekingFood = false;
	if (!UpdateAutoFoodSeeking(DeltaSeconds))
	{
		UpdateRandomWander(DeltaSeconds);
	}

	ApplyGravity(DeltaSeconds);
	UpdateDropFoodConsumption(DeltaSeconds);
	UpdateVisualFacing(DeltaSeconds);
}

void ASquirrelTrainingPawn::ApplyVisualTuning()
{
	const bool bHasSkeletalMesh = SquirrelMesh && SquirrelMesh->GetSkeletalMeshAsset();
	const bool bShowSkeletalMesh = bUseSkeletalSquirrelVisual && bHasSkeletalMesh;

	if (SquirrelMesh)
	{
		FVector MeshRelativeLocation = SkeletalMeshRelativeLocation;
		if (bAutoAlignSkeletalMeshToCapsuleBottom && Collision && SquirrelMesh->GetSkeletalMeshAsset())
		{
			const FBoxSphereBounds LocalBounds = SquirrelMesh->GetLocalBounds();
			const FVector LocalMin = LocalBounds.Origin - LocalBounds.BoxExtent;
			const float MeshBottomZ = LocalMin.Z * SkeletalMeshRelativeScale.Z;
			MeshRelativeLocation.Z = -Collision->GetUnscaledCapsuleHalfHeight() - MeshBottomZ + SkeletalMeshGroundOffset;
		}

		SquirrelMesh->SetRelativeLocation(MeshRelativeLocation);
		SquirrelMesh->SetRelativeRotation(SkeletalMeshRelativeRotation);
		SquirrelMesh->SetRelativeScale3D(SkeletalMeshRelativeScale);
		if (SquirrelAnimClass)
		{
			SquirrelMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
			SquirrelMesh->SetAnimInstanceClass(SquirrelAnimClass);
		}
		SquirrelMesh->SetVisibility(bShowSkeletalMesh);
		SquirrelMesh->SetHiddenInGame(!bShowSkeletalMesh);
	}

	const bool bShowPlaceholder = !bShowSkeletalMesh || !bHidePlaceholderMeshesWhenUsingSkeletalMesh;
	if (BodyMesh)
	{
		BodyMesh->SetVisibility(bShowPlaceholder);
		BodyMesh->SetHiddenInGame(!bShowPlaceholder);
	}

	if (TailMesh)
	{
		TailMesh->SetVisibility(bShowPlaceholder);
		TailMesh->SetHiddenInGame(!bShowPlaceholder);
	}
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

	SetActorLocation(ClampToTrainingArea(NewLocation), bIsDragging && bSweepDragMovement);

	const FVector MovedLocation = GetActorLocation();
	const float DeltaX = MovedLocation.X - CurrentLocation.X;
	const float DeltaY = MovedLocation.Y - CurrentLocation.Y;
	if (!FMath::IsNearlyZero(DeltaX, 0.1f) || !FMath::IsNearlyZero(DeltaY, 0.1f))
	{
		FaceMovementDirection(DeltaX);
	}
}

void ASquirrelTrainingPawn::FaceMovementDirection(float DeltaX)
{
	if (!FMath::IsNearlyZero(DeltaX, 0.1f))
	{
		TargetVisualYaw = DeltaX >= 0.0f ? 0.0f : 180.0f;
	}
}

void ASquirrelTrainingPawn::UpdateVisualFacing(float DeltaSeconds)
{
	if (!VisualRoot)
	{
		return;
	}

	const FRotator CurrentRotation = VisualRoot->GetRelativeRotation();
	const FRotator TargetRotation(0.0f, TargetVisualYaw, 0.0f);
	const FRotator NewRotation = VisualTurnInterpSpeed > 0.0f
		? FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, VisualTurnInterpSpeed)
		: TargetRotation;
	VisualRoot->SetRelativeRotation(NewRotation);
}

USquirrelGameInstance* ASquirrelTrainingPawn::GetProgressGameInstance() const
{
	return GetGameInstance<USquirrelGameInstance>();
}

void ASquirrelTrainingPawn::SyncEnergyFromProgress()
{
	if (USquirrelGameInstance* SquirrelGameInstance = GetProgressGameInstance())
	{
		EnergyLevel = SquirrelGameInstance->GetStatLevel(ESquirrelProgressStat::Energy);
		EnergyProgressPoints = SquirrelGameInstance->GetStatProgressPoints(ESquirrelProgressStat::Energy);
		MaxEnergyLevel = SquirrelGameInstance->GetStatMaxLevel(ESquirrelProgressStat::Energy);
		EnergyPointsPerLevel = SquirrelGameInstance->GetStatPointsPerLevel(ESquirrelProgressStat::Energy);
		EnergyPointsPerFood = SquirrelGameInstance->GetEnergyPointsPerFood();
	}
}

void ASquirrelTrainingPawn::ApplyGravity(float DeltaSeconds)
{
	if (RecoverFromLongFall())
	{
		return;
	}

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

	bIsSeekingFood = true;
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

void ASquirrelTrainingPawn::UpdateEating(float DeltaSeconds)
{
	EatTimeRemaining -= DeltaSeconds;
	if (EatTimeRemaining > 0.0f)
	{
		return;
	}

	CompleteEatingFood();
}

void ASquirrelTrainingPawn::ApplyLocalProgressTuning()
{
	const USquirrelProgressTuningData* ResolvedTuning = GetResolvedProgressTuning();
	if (!ResolvedTuning)
	{
		return;
	}

	const FSquirrelStatTuning EnergyTuning = ResolvedTuning->GetTuningForStat(ESquirrelProgressStat::Energy);
	StartingEnergyLevel = EnergyTuning.StartingLevel;
	MaxEnergyLevel = EnergyTuning.MaxLevel;
	EnergyPointsPerLevel = EnergyTuning.PointsPerLevel;
	EnergyPointsPerFood = FMath::Max(ResolvedTuning->EnergyPointsPerFood, 0);
}

USquirrelProgressTuningData* ASquirrelTrainingPawn::GetResolvedProgressTuning() const
{
	if (TuningComponent)
	{
		return TuningComponent->ResolveProgressTuning();
	}

	if (const USquirrelGameInstance* SquirrelGameInstance = GetProgressGameInstance())
	{
		if (USquirrelProgressTuningData* GameInstanceTuning = SquirrelGameInstance->GetProgressTuning())
		{
			return GameInstanceTuning;
		}
	}

	if (const ASquirrelTrainingGameMode* TrainingGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ASquirrelTrainingGameMode>() : nullptr)
	{
		return TrainingGameMode->GetProgressTuning();
	}

	return nullptr;
}

bool ASquirrelTrainingPawn::TryConsumeNearbyFood()
{
	ASquirrelFoodActor* Food = FindNearestConsumableFood(FoodSeekRadius);
	if (!Food || !IsFoodCloseEnoughToEat(*Food))
	{
		return false;
	}

	return TryConsumeFood(Food);
}

bool ASquirrelTrainingPawn::TryConsumeFood(ASquirrelFoodActor* Food)
{
	if (!Food || FoodEatCooldownRemaining > 0.0f || bIsEating)
	{
		return false;
	}

	BeginEatingFood(Food);
	return true;
}

void ASquirrelTrainingPawn::BeginEatingFood(ASquirrelFoodActor* Food)
{
	if (!Food)
	{
		return;
	}

	bIsEating = true;
	bIsSeekingFood = false;
	bHasRandomWanderTarget = false;
	FoodBeingEaten = Food;
	EatTimeRemaining = EatDuration;
	Food->BeginHeldForEating();
	ASquirrelAudioManager::PlaySquirrelAudioEvent(this, ESquirrelAudioEvent::EatingStarted, Food->GetActorLocation(), this);
	OnEatingStarted(Food);

	const float DeltaX = Food->GetActorLocation().X - GetActorLocation().X;
	if (!FMath::IsNearlyZero(DeltaX, 0.1f))
	{
		FaceMovementDirection(DeltaX);
	}
}

void ASquirrelTrainingPawn::CompleteEatingFood()
{
	ASquirrelFoodActor* Food = FoodBeingEaten.Get();
	bIsEating = false;
	FoodBeingEaten = nullptr;
	EatTimeRemaining = 0.0f;

	if (Food)
	{
		OnEatingFinished(Food);
		if (Food->TryConsume(this))
		{
			FoodEatCooldownRemaining = FoodEatCooldown;
			PostEatPatrolCooldownRemaining = PostEatPatrolCooldown;
			bHasRandomWanderTarget = false;
			RandomWanderIdleTimeRemaining = PostEatPatrolCooldown;
			ASquirrelAudioManager::PlaySquirrelAudioEvent(this, ESquirrelAudioEvent::EatingFinished, GetActorLocation(), this);
		}
	}
	else
	{
		OnEatingFinished(nullptr);
	}
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
	const FVector SquirrelLocation = GetActorLocation();
	const FVector FoodLocation = Food.GetActorLocation();
	const float GroundPlaneDistanceSquared = FVector2D::DistSquared(
		FVector2D(SquirrelLocation.X, SquirrelLocation.Y),
		FVector2D(FoodLocation.X, FoodLocation.Y));
	return GroundPlaneDistanceSquared <= FMath::Square(FoodConsumeRadius);
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

	if (bUseCapsuleGroundSweep && Collision)
	{
		const FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(
			Collision->GetScaledCapsuleRadius() * 0.92f,
			Collision->GetScaledCapsuleHalfHeight());
		const FCollisionObjectQueryParams ObjectQueryParams(ECC_TO_BITFIELD(ECC_WorldStatic));
		if (World->SweepSingleByObjectType(OutHit, Start, End, FQuat::Identity, ObjectQueryParams, CapsuleShape, QueryParams))
		{
			return true;
		}
	}

	return World->LineTraceSingleByChannel(OutHit, Start, End, GroundTraceChannel, QueryParams);
}

void ASquirrelTrainingPawn::SnapToGround(const FHitResult& GroundHit)
{
	FVector GroundedLocation = GetActorLocation();
	GroundedLocation.Z = GroundHit.ImpactPoint.Z + Collision->GetScaledCapsuleHalfHeight();
	SetActorLocation(ClampToTrainingArea(GroundedLocation), false);

	VerticalVelocity = 0.0f;
	bIsGrounded = true;
	LastSafeGroundedLocation = GetActorLocation();
	bHasLastSafeGroundedLocation = true;
}

bool ASquirrelTrainingPawn::RecoverFromLongFall()
{
	if (!bRecoverFromLongFall || GetActorLocation().Z > FallRecoveryZ)
	{
		return false;
	}

	FVector RecoveryLocation = bHasLastSafeGroundedLocation ? LastSafeGroundedLocation : SpawnLocation;
	SetActorLocation(ClampToTrainingArea(RecoveryLocation), false, nullptr, ETeleportType::TeleportPhysics);
	VerticalVelocity = 0.0f;
	bIsGrounded = false;

	FHitResult GroundHit;
	if (FindGround(GroundProbeDistance, GroundHit))
	{
		SnapToGround(GroundHit);
	}

	return true;
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

int32 ASquirrelTrainingPawn::GetResolvedEnergyPointsPerFood() const
{
	if (const USquirrelGameInstance* SquirrelGameInstance = GetProgressGameInstance())
	{
		return SquirrelGameInstance->GetEnergyPointsPerFood();
	}

	if (const USquirrelProgressTuningData* ResolvedTuning = GetResolvedProgressTuning())
	{
		return FMath::Max(ResolvedTuning->EnergyPointsPerFood, 0);
	}

	return EnergyPointsPerFood;
}

void ASquirrelTrainingPawn::BeginDrag()
{
	if (bIsEating)
	{
		return;
	}

	bIsDragging = true;
	bIsSeekingFood = false;
	FoodBeingEaten = nullptr;
	EatTimeRemaining = 0.0f;
	PostEatPatrolCooldownRemaining = 0.0f;
	bWaitingToConsumeDroppedFood = false;
	bHasRandomWanderTarget = false;
	DropConsumeTimeRemaining = 0.0f;
	DragTarget = GetActorLocation();
	ASquirrelAudioManager::PlaySquirrelAudioEvent(this, ESquirrelAudioEvent::DragStarted, GetActorLocation(), this);
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
		SetActorLocation(DragTarget, bSweepDragMovement);

		const float DeltaX = GetActorLocation().X - PreviousLocation.X;
		if (!FMath::IsNearlyZero(DeltaX, 0.1f))
		{
			FaceMovementDirection(DeltaX);
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
	ASquirrelAudioManager::PlaySquirrelAudioEvent(this, ESquirrelAudioEvent::DragReleased, GetActorLocation(), this);
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

void ASquirrelTrainingPawn::AddEnergyProgress(int32 Amount)
{
	if (Amount <= 0 || EnergyLevel >= MaxEnergyLevel)
	{
		return;
	}

	const int32 PreviousEnergyLevel = EnergyLevel;
	if (USquirrelGameInstance* SquirrelGameInstance = GetProgressGameInstance())
	{
		SquirrelGameInstance->AddProgressToStat(ESquirrelProgressStat::Energy, Amount);
		SyncEnergyFromProgress();
		OnEnergyChanged.Broadcast(EnergyLevel, EnergyProgressPoints, MaxEnergyLevel);

		if (EnergyLevel > PreviousEnergyLevel)
		{
			ASquirrelAudioManager::PlaySquirrelAudioEvent(this, ESquirrelAudioEvent::EnergyLevelUp, GetActorLocation(), this);
		}

		return;
	}

	EnergyProgressPoints += Amount;

	while (EnergyProgressPoints >= EnergyPointsPerLevel && EnergyLevel < MaxEnergyLevel)
	{
		EnergyProgressPoints -= EnergyPointsPerLevel;
		EnergyLevel++;
	}

	if (EnergyLevel >= MaxEnergyLevel)
	{
		EnergyLevel = MaxEnergyLevel;
		EnergyProgressPoints = 0;
	}

	OnEnergyChanged.Broadcast(EnergyLevel, EnergyProgressPoints, MaxEnergyLevel);

	if (EnergyLevel > PreviousEnergyLevel)
	{
		ASquirrelAudioManager::PlaySquirrelAudioEvent(this, ESquirrelAudioEvent::EnergyLevelUp, GetActorLocation(), this);
	}
}

void ASquirrelTrainingPawn::AddEnergyFromFood(int32 FoodAmount)
{
	if (FoodAmount <= 0)
	{
		return;
	}

	AddEnergyProgress(FoodAmount * GetResolvedEnergyPointsPerFood());
}

bool ASquirrelTrainingPawn::TrySpendEnergy(int32 Amount)
{
	if (Amount <= 0)
	{
		return true;
	}

	if (USquirrelGameInstance* SquirrelGameInstance = GetProgressGameInstance())
	{
		if (!SquirrelGameInstance->TrySpendStatLevels(ESquirrelProgressStat::Energy, Amount))
		{
			return false;
		}

		SyncEnergyFromProgress();
		OnEnergyChanged.Broadcast(EnergyLevel, EnergyProgressPoints, MaxEnergyLevel);
		return true;
	}

	if (EnergyLevel < Amount)
	{
		return false;
	}

	EnergyLevel -= Amount;
	OnEnergyChanged.Broadcast(EnergyLevel, EnergyProgressPoints, MaxEnergyLevel);
	return true;
}

float ASquirrelTrainingPawn::GetEnergyProgressPercent() const
{
	return EnergyPointsPerLevel > 0 ? static_cast<float>(EnergyProgressPoints) / static_cast<float>(EnergyPointsPerLevel) : 0.0f;
}

float ASquirrelTrainingPawn::GetEnergyLevelPercent() const
{
	return MaxEnergyLevel > 0 ? static_cast<float>(EnergyLevel) / static_cast<float>(MaxEnergyLevel) : 0.0f;
}
