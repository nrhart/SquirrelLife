// Copyright Epic Games, Inc. All Rights Reserved.

#include "MiniGames/Runner/SquirrelRunnerObstacle.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "MiniGames/SquirrelMiniGameModeBase.h"
#include "MiniGames/Runner/SquirrelEndlessRunnerPawn.h"

ASquirrelRunnerObstacle::ASquirrelRunnerObstacle()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	ObstacleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObstacleMesh"));
	ObstacleMesh->SetupAttachment(RootComponent);
	ObstacleMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ObstacleMesh->SetCollisionObjectType(ECC_WorldDynamic);
	ObstacleMesh->SetCollisionResponseToAllChannels(ECR_Block);
	ObstacleMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	ObstacleMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ObstacleMesh->SetGenerateOverlapEvents(true);
	ObstacleMesh->OnComponentBeginOverlap.AddDynamic(this, &ASquirrelRunnerObstacle::HandleObstacleOverlap);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		ObstacleMesh->SetStaticMesh(SphereMesh.Object);
		DefaultObstacleMesh = SphereMesh.Object;
	}
}

void ASquirrelRunnerObstacle::BeginPlay()
{
	Super::BeginPlay();

	if (ObstacleMesh && !DefaultObstacleMesh)
	{
		DefaultObstacleMesh = ObstacleMesh->GetStaticMesh();
	}

	if (bIsPooledInactive)
	{
		return;
	}

	BaseZ = GetActorLocation().Z;

	if (bAutoFindPlayer && !TargetActor)
	{
		FindPlayerTarget();
	}
}

void ASquirrelRunnerObstacle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsPooledInactive)
	{
		return;
	}

	ElapsedTime += DeltaSeconds;

	if (bAutoFindPlayer && !TargetActor)
	{
		FindPlayerTarget();
	}

	if (bAutoMove)
	{
		UpdateMovement(DeltaSeconds);
	}

	if (ShouldDestroyBehindPlayer())
	{
		Destroy();
	}
}

void ASquirrelRunnerObstacle::FindPlayerTarget()
{
	TargetActor = UGameplayStatics::GetPlayerPawn(this, 0);
}

void ASquirrelRunnerObstacle::UpdateMovement(float DeltaSeconds)
{
	const FVector Direction = MoveDirection.GetSafeNormal();
	if (Direction.IsNearlyZero())
	{
		return;
	}

	FVector NewLocation = GetActorLocation() + Direction * MoveSpeed * DeltaSeconds;

	if (MovementPattern == ESquirrelRunnerObstaclePattern::BouncingArc)
	{
		const float BounceAlpha = FMath::Abs(FMath::Sin(ElapsedTime * BounceFrequency * UE_TWO_PI));
		NewLocation.Z = BaseZ + BounceAlpha * BounceHeight;
	}
	else if (MovementPattern == ESquirrelRunnerObstaclePattern::Flying)
	{
		NewLocation.Z = BaseZ;
	}

	SetActorLocation(NewLocation, false);
	UpdateRollingVisual(DeltaSeconds);
}

void ASquirrelRunnerObstacle::UpdateRollingVisual(float DeltaSeconds)
{
	if (!ObstacleMesh || MovementPattern != ESquirrelRunnerObstaclePattern::Rolling)
	{
		return;
	}

	const float DirectionSign = MoveDirection.X < 0.0f ? -1.0f : 1.0f;
	ObstacleMesh->AddLocalRotation(FRotator(0.0f, 0.0f, DirectionSign * RollDegreesPerSecond * DeltaSeconds));
}

bool ASquirrelRunnerObstacle::ShouldDestroyBehindPlayer() const
{
	if (!bDestroyWhenBehindPlayer)
	{
		return false;
	}

	return HasPassedTarget(DestroyBehindDistance);
}

bool ASquirrelRunnerObstacle::HasPassedTarget(float BehindDistance) const
{
	if (!TargetActor)
	{
		return false;
	}

	const FVector Direction = (-MoveDirection).GetSafeNormal();
	if (Direction.IsNearlyZero())
	{
		return false;
	}

	const float ObstaclePosition = FVector::DotProduct(GetActorLocation(), Direction);
	const float TargetPosition = FVector::DotProduct(TargetActor->GetActorLocation(), Direction);
	return ObstaclePosition < TargetPosition - BehindDistance;
}

void ASquirrelRunnerObstacle::HandleObstacleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsPooledInactive || !OtherActor || OtherActor != TargetActor)
	{
		return;
	}

	if (ASquirrelMiniGameModeBase* MiniGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ASquirrelMiniGameModeBase>() : nullptr)
	{
		if (ASquirrelEndlessRunnerPawn* RunnerPawn = Cast<ASquirrelEndlessRunnerPawn>(OtherActor))
		{
			RunnerPawn->PlayDeathSequence();
		}

		MiniGameMode->EndMiniGameSession();
	}
}

void ASquirrelRunnerObstacle::InitializeObstacle(ESquirrelRunnerObstaclePattern NewPattern, AActor* NewTargetActor)
{
	SetMovementPattern(NewPattern);
	SetTargetActor(NewTargetActor);
	BaseZ = GetActorLocation().Z;
	ElapsedTime = 0.0f;
}

void ASquirrelRunnerObstacle::SetTargetActor(AActor* NewTargetActor)
{
	TargetActor = NewTargetActor;
}

void ASquirrelRunnerObstacle::SetMovementPattern(ESquirrelRunnerObstaclePattern NewPattern)
{
	MovementPattern = NewPattern;
}

void ASquirrelRunnerObstacle::SetMoveSpeed(float NewMoveSpeed)
{
	MoveSpeed = FMath::Max(NewMoveSpeed, 0.0f);
}

void ASquirrelRunnerObstacle::SetBounceArc(float NewBounceHeight, float NewBounceFrequency)
{
	BounceHeight = FMath::Max(NewBounceHeight, 0.0f);
	BounceFrequency = FMath::Max(NewBounceFrequency, 0.0f);
}

void ASquirrelRunnerObstacle::SetObstacleMesh(UStaticMesh* NewMesh)
{
	if (!ObstacleMesh)
	{
		return;
	}

	if (NewMesh)
	{
		ObstacleMesh->SetStaticMesh(NewMesh);
		return;
	}

	if (DefaultObstacleMesh)
	{
		ObstacleMesh->SetStaticMesh(DefaultObstacleMesh);
	}
}

void ASquirrelRunnerObstacle::ActivateFromPool(const FVector& NewLocation, const FRotator& NewRotation, ESquirrelRunnerObstaclePattern NewPattern, AActor* NewTargetActor)
{
	bIsPooledInactive = false;
	SetActorLocationAndRotation(NewLocation, NewRotation, false, nullptr, ETeleportType::TeleportPhysics);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	bAutoMove = true;
	InitializeObstacle(NewPattern, NewTargetActor);
}

void ASquirrelRunnerObstacle::DeactivateForPool()
{
	bIsPooledInactive = true;
	bAutoMove = false;
	TargetActor = nullptr;
	ElapsedTime = 0.0f;

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	if (ObstacleMesh)
	{
		ObstacleMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
		ObstacleMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	}
}
