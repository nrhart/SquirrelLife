// Copyright Epic Games, Inc. All Rights Reserved.

#include "MiniGames/Runner/SquirrelRunnerObstacleSpawner.h"

#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"

ASquirrelRunnerObstacleSpawner::ASquirrelRunnerObstacleSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	RollingObstacleClass = ASquirrelRunnerObstacle::StaticClass();
	BouncingObstacleClass = ASquirrelRunnerObstacle::StaticClass();
	FlyingObstacleClass = ASquirrelRunnerObstacle::StaticClass();

	SpawnOptions = {
		FSquirrelRunnerObstacleSpawnOption{ASquirrelRunnerObstacle::StaticClass(), ESquirrelRunnerObstaclePattern::Rolling, 1.0f, FVector::ZeroVector},
		FSquirrelRunnerObstacleSpawnOption{ASquirrelRunnerObstacle::StaticClass(), ESquirrelRunnerObstaclePattern::BouncingArc, 1.0f, FVector::ZeroVector},
		FSquirrelRunnerObstacleSpawnOption{ASquirrelRunnerObstacle::StaticClass(), ESquirrelRunnerObstaclePattern::Flying, 1.0f, FVector::ZeroVector}
	};
}

void ASquirrelRunnerObstacleSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoFindPlayer && !TargetActor)
	{
		FindPlayerTarget();
	}

	ResetSpawnTimer();
}

void ASquirrelRunnerObstacleSpawner::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bAutoSpawn)
	{
		return;
	}

	if (bAutoFindPlayer && !TargetActor)
	{
		FindPlayerTarget();
	}

	if (bUseObstaclePooling)
	{
		UpdatePooledObstacles();
	}

	SpawnTimeRemaining -= DeltaSeconds;
	if (SpawnTimeRemaining <= 0.0f)
	{
		SpawnObstacle();
		ResetSpawnTimer();
	}
}

void ASquirrelRunnerObstacleSpawner::FindPlayerTarget()
{
	TargetActor = UGameplayStatics::GetPlayerPawn(this, 0);
}

void ASquirrelRunnerObstacleSpawner::ResetSpawnTimer()
{
	const float DelayMin = FMath::Min(MinSpawnDelay, MaxSpawnDelay);
	const float DelayMax = FMath::Max(MinSpawnDelay, MaxSpawnDelay);
	SpawnTimeRemaining = FMath::FRandRange(DelayMin, DelayMax);
}

const FSquirrelRunnerObstacleSpawnOption* ASquirrelRunnerObstacleSpawner::ChooseSpawnOption() const
{
	float TotalWeight = 0.0f;
	for (const FSquirrelRunnerObstacleSpawnOption& Option : SpawnOptions)
	{
		if (Option.ObstacleClass && Option.Weight > 0.0f)
		{
			TotalWeight += Option.Weight;
		}
	}

	if (TotalWeight <= 0.0f)
	{
		return nullptr;
	}

	float Roll = FMath::FRandRange(0.0f, TotalWeight);
	for (const FSquirrelRunnerObstacleSpawnOption& Option : SpawnOptions)
	{
		if (!Option.ObstacleClass || Option.Weight <= 0.0f)
		{
			continue;
		}

		Roll -= Option.Weight;
		if (Roll <= 0.0f)
		{
			return &Option;
		}
	}

	return nullptr;
}

ESquirrelRunnerObstaclePattern ASquirrelRunnerObstacleSpawner::ChooseFixedPattern() const
{
	return static_cast<ESquirrelRunnerObstaclePattern>(FMath::RandRange(0, 2));
}

TSubclassOf<ASquirrelRunnerObstacle> ASquirrelRunnerObstacleSpawner::GetObstacleClassForPattern(ESquirrelRunnerObstaclePattern Pattern) const
{
	switch (Pattern)
	{
	case ESquirrelRunnerObstaclePattern::Rolling:
		return RollingObstacleClass ? RollingObstacleClass : TSubclassOf<ASquirrelRunnerObstacle>(ASquirrelRunnerObstacle::StaticClass());
	case ESquirrelRunnerObstaclePattern::BouncingArc:
		return BouncingObstacleClass ? BouncingObstacleClass : TSubclassOf<ASquirrelRunnerObstacle>(ASquirrelRunnerObstacle::StaticClass());
	case ESquirrelRunnerObstaclePattern::Flying:
		return FlyingObstacleClass ? FlyingObstacleClass : TSubclassOf<ASquirrelRunnerObstacle>(ASquirrelRunnerObstacle::StaticClass());
	default:
		return TSubclassOf<ASquirrelRunnerObstacle>(ASquirrelRunnerObstacle::StaticClass());
	}
}

UStaticMesh* ASquirrelRunnerObstacleSpawner::GetObstacleMeshForPattern(ESquirrelRunnerObstaclePattern Pattern) const
{
	switch (Pattern)
	{
	case ESquirrelRunnerObstaclePattern::Rolling:
		return RollingObstacleMesh;
	case ESquirrelRunnerObstaclePattern::BouncingArc:
		return BouncingObstacleMesh;
	case ESquirrelRunnerObstaclePattern::Flying:
		return FlyingObstacleMesh;
	default:
		return nullptr;
	}
}

float ASquirrelRunnerObstacleSpawner::GetSpawnZForPattern(ESquirrelRunnerObstaclePattern Pattern) const
{
	switch (Pattern)
	{
	case ESquirrelRunnerObstaclePattern::Rolling:
		return GroundObstacleZ;
	case ESquirrelRunnerObstaclePattern::BouncingArc:
		return BouncingObstacleBaseZ;
	case ESquirrelRunnerObstaclePattern::Flying:
		return FlyingObstacleZ;
	default:
		return GroundObstacleZ;
	}
}

ASquirrelRunnerObstacle* ASquirrelRunnerObstacleSpawner::CreateObstacle(TSubclassOf<ASquirrelRunnerObstacle> ObstacleClass, const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	UWorld* World = GetWorld();
	if (!World || !ObstacleClass)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	return World->SpawnActor<ASquirrelRunnerObstacle>(ObstacleClass, SpawnLocation, SpawnRotation, SpawnParams);
}

ASquirrelRunnerObstacle* ASquirrelRunnerObstacleSpawner::GetOrCreateObstacle(TSubclassOf<ASquirrelRunnerObstacle> ObstacleClass, const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	PrunePooledObstacles();

	for (ASquirrelRunnerObstacle* Obstacle : PooledObstacles)
	{
		if (Obstacle && Obstacle->IsPooledInactive() && Obstacle->IsA(ObstacleClass))
		{
			return Obstacle;
		}
	}

	if (PooledObstacles.Num() >= MaxPooledObstacles)
	{
		return nullptr;
	}

	ASquirrelRunnerObstacle* NewObstacle = CreateObstacle(ObstacleClass, SpawnLocation, SpawnRotation);
	if (NewObstacle)
	{
		NewObstacle->SetDestroyWhenBehindPlayer(false);
		PooledObstacles.Add(NewObstacle);
	}

	return NewObstacle;
}

void ASquirrelRunnerObstacleSpawner::UpdatePooledObstacles()
{
	for (ASquirrelRunnerObstacle* Obstacle : PooledObstacles)
	{
		if (!Obstacle || Obstacle->IsPooledInactive())
		{
			continue;
		}

		if (Obstacle->HasPassedTarget(PoolRecycleBehindDistance))
		{
			Obstacle->DeactivateForPool();
		}
	}
}

void ASquirrelRunnerObstacleSpawner::PrunePooledObstacles()
{
	PooledObstacles.RemoveAllSwap(
		[](const TObjectPtr<ASquirrelRunnerObstacle>& Obstacle)
		{
			return !Obstacle;
		});
}

void ASquirrelRunnerObstacleSpawner::SetTargetActor(AActor* NewTargetActor)
{
	TargetActor = NewTargetActor;
}

ASquirrelRunnerObstacle* ASquirrelRunnerObstacleSpawner::SpawnObstacle()
{
	UWorld* World = GetWorld();
	if (!World || !TargetActor)
	{
		return nullptr;
	}

	ESquirrelRunnerObstaclePattern Pattern = ESquirrelRunnerObstaclePattern::Rolling;
	TSubclassOf<ASquirrelRunnerObstacle> ObstacleClass;
	FVector OptionOffset = FVector::ZeroVector;

	if (bUseFixedThreeObstacleTypes)
	{
		Pattern = ChooseFixedPattern();
		ObstacleClass = GetObstacleClassForPattern(Pattern);
	}
	else
	{
		const FSquirrelRunnerObstacleSpawnOption* Option = ChooseSpawnOption();
		if (!Option || !Option->ObstacleClass)
		{
			return nullptr;
		}

		Pattern = Option->Pattern;
		ObstacleClass = Option->ObstacleClass;
		OptionOffset = Option->SpawnOffset;
	}

	if (!ObstacleClass)
	{
		return nullptr;
	}

	FVector SpawnLocation = TargetActor->GetActorLocation() + SpawnOffsetFromPlayer;
	if (bUseFixedLaneZ)
	{
		SpawnLocation.Z = GetSpawnZForPattern(Pattern);
		OptionOffset.Z = 0.0f;
	}
	SpawnLocation += OptionOffset;

	const FRotator SpawnRotation = GetActorRotation();

	ASquirrelRunnerObstacle* Obstacle = bUseObstaclePooling
		? GetOrCreateObstacle(ObstacleClass, SpawnLocation, SpawnRotation)
		: CreateObstacle(ObstacleClass, SpawnLocation, SpawnRotation);
	if (Obstacle)
	{
		if (bUseObstaclePooling)
		{
			Obstacle->SetDestroyWhenBehindPlayer(false);
			Obstacle->ActivateFromPool(SpawnLocation, SpawnRotation, Pattern, TargetActor);
		}
		else
		{
			Obstacle->InitializeObstacle(Pattern, TargetActor);
		}

		Obstacle->SetMoveSpeed(SpawnedObstacleSpeed);
		if (bUseFixedThreeObstacleTypes)
		{
			Obstacle->SetObstacleMesh(GetObstacleMeshForPattern(Pattern));
		}

		if (Pattern == ESquirrelRunnerObstaclePattern::BouncingArc)
		{
			Obstacle->SetBounceArc(SpawnedBounceHeight, SpawnedBounceFrequency);
		}
	}

	return Obstacle;
}
