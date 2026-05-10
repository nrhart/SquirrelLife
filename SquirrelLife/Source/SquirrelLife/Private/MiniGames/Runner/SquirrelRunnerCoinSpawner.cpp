// Copyright Epic Games, Inc. All Rights Reserved.

#include "MiniGames/Runner/SquirrelRunnerCoinSpawner.h"

#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "MiniGames/Runner/SquirrelRunnerCoin.h"

ASquirrelRunnerCoinSpawner::ASquirrelRunnerCoinSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	CoinClass = ASquirrelRunnerCoin::StaticClass();
}

void ASquirrelRunnerCoinSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoFindPlayer && !TargetActor)
	{
		FindPlayerTarget();
	}

	if (TargetActor)
	{
		const int32 InitialCoinCount = FMath::Max(DesiredActiveCoinCount, 0);
		for (int32 CoinIndex = 0; CoinIndex < InitialCoinCount; ++CoinIndex)
		{
			SpawnCoinForSlot(CoinIndex);
		}
	}

	ResetSpawnTimer();
}

void ASquirrelRunnerCoinSpawner::Tick(float DeltaSeconds)
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

	if (bUseCoinPooling)
	{
		UpdatePooledCoins();
	}

	const int32 ActiveCoinCount = GetActiveCoinCount();
	if (ActiveCoinCount >= DesiredActiveCoinCount)
	{
		SpawnTimeRemaining = 0.0f;
		return;
	}

	SpawnTimeRemaining -= DeltaSeconds;
	if (SpawnTimeRemaining <= 0.0f)
	{
		if (SpawnCoinForSlot(ActiveCoinCount))
		{
			ResetSpawnTimer();
		}
		else
		{
			RetrySpawnSoon();
		}
	}
}

void ASquirrelRunnerCoinSpawner::FindPlayerTarget()
{
	TargetActor = UGameplayStatics::GetPlayerPawn(this, 0);
}

void ASquirrelRunnerCoinSpawner::ResetSpawnTimer()
{
	const float DelayMin = FMath::Min(MinSpawnDelay, MaxSpawnDelay);
	const float DelayMax = FMath::Max(MinSpawnDelay, MaxSpawnDelay);
	SpawnTimeRemaining = FMath::FRandRange(DelayMin, DelayMax);
}

void ASquirrelRunnerCoinSpawner::RetrySpawnSoon()
{
	SpawnTimeRemaining = FMath::Max(FailedSpawnRetryDelay, 0.0f);
}

FVector ASquirrelRunnerCoinSpawner::GetCoinSpawnLocation(int32 CoinSlotIndex) const
{
	FVector SpawnLocation = TargetActor ? TargetActor->GetActorLocation() + SpawnOffsetFromPlayer : GetActorLocation();
	SpawnLocation.X += FMath::Max(CoinSlotIndex, 0) * CoinSpacingX;
	SpawnLocation.Z = FMath::FRandRange(FMath::Min(MinSpawnZ, MaxSpawnZ), FMath::Max(MinSpawnZ, MaxSpawnZ));
	SpawnLocation.Y += FMath::FRandRange(-SpawnYRange, SpawnYRange);
	return SpawnLocation;
}

ASquirrelRunnerCoin* ASquirrelRunnerCoinSpawner::CreateCoin(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	UWorld* World = GetWorld();
	if (!World || !CoinClass)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	return World->SpawnActor<ASquirrelRunnerCoin>(CoinClass, SpawnLocation, SpawnRotation, SpawnParams);
}

ASquirrelRunnerCoin* ASquirrelRunnerCoinSpawner::GetOrCreateCoin(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	PrunePooledCoins();

	for (ASquirrelRunnerCoin* Coin : PooledCoins)
	{
		if (Coin && Coin->IsPooledInactive())
		{
			return Coin;
		}
	}

	if (PooledCoins.Num() >= MaxPooledCoins)
	{
		return nullptr;
	}

	ASquirrelRunnerCoin* NewCoin = CreateCoin(SpawnLocation, SpawnRotation);
	if (NewCoin)
	{
		NewCoin->SetDestroyWhenBehindPlayer(false);
		NewCoin->DeactivateForPool();
		PooledCoins.Add(NewCoin);
	}

	return NewCoin;
}

void ASquirrelRunnerCoinSpawner::UpdatePooledCoins()
{
	for (ASquirrelRunnerCoin* Coin : PooledCoins)
	{
		if (!Coin || Coin->IsPooledInactive())
		{
			continue;
		}

		if (Coin->HasPassedTarget(PoolRecycleBehindDistance))
		{
			Coin->DeactivateForPool();
		}
	}
}

void ASquirrelRunnerCoinSpawner::PrunePooledCoins()
{
	PooledCoins.RemoveAllSwap(
		[](const TObjectPtr<ASquirrelRunnerCoin>& Coin)
		{
			return !Coin;
		});
}

int32 ASquirrelRunnerCoinSpawner::GetActiveCoinCount() const
{
	int32 ActiveCoinCount = 0;
	for (const ASquirrelRunnerCoin* Coin : PooledCoins)
	{
		if (Coin && !Coin->IsPooledInactive())
		{
			ActiveCoinCount++;
		}
	}

	return ActiveCoinCount;
}

ASquirrelRunnerCoin* ASquirrelRunnerCoinSpawner::SpawnCoinForSlot(int32 CoinSlotIndex)
{
	if (!TargetActor || !CoinClass)
	{
		return nullptr;
	}

	const FVector SpawnLocation = GetCoinSpawnLocation(CoinSlotIndex);
	FRotator SpawnRotation = CoinSpawnRotation;
	if (bUseRandomSpawnYaw)
	{
		SpawnRotation.Yaw = FMath::FRandRange(0.0f, 360.0f);
	}

	ASquirrelRunnerCoin* Coin = bUseCoinPooling
		? GetOrCreateCoin(SpawnLocation, SpawnRotation)
		: CreateCoin(SpawnLocation, SpawnRotation);

	if (Coin)
	{
		if (bUseCoinPooling)
		{
			Coin->SetDestroyWhenBehindPlayer(false);
			Coin->ActivateFromPool(SpawnLocation, SpawnRotation, TargetActor);
		}
		else
		{
			Coin->SetDestroyWhenBehindPlayer(true);
			Coin->SetTargetActor(TargetActor);
		}

		Coin->SetCoinValue(CoinValue);
		Coin->SetCoinMesh(CoinMesh);
	}

	return Coin;
}

ASquirrelRunnerCoin* ASquirrelRunnerCoinSpawner::SpawnCoin()
{
	return SpawnCoinForSlot(GetActiveCoinCount());
}
