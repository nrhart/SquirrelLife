// Copyright Epic Games, Inc. All Rights Reserved.

#include "Progress/SquirrelGameInstance.h"

#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Progress/SquirrelProgressTuningData.h"
#include "Progress/SquirrelSaveGame.h"

void USquirrelGameInstance::Init()
{
	Super::Init();

	InitializeNewProgress();
	if (bAutoLoadOnInit)
	{
		LoadProgress();
	}
}

void USquirrelGameInstance::InitializeNewProgress()
{
	Coins = GetConfiguredStartingCoins();
	ProgressStats.Empty();
	EnsureDefaultStats();
	bHasParkSquirrelTransform = false;
	ParkSquirrelTransform = FTransform::Identity;
	BroadcastAllProgress();
}

bool USquirrelGameInstance::SaveProgress()
{
	USquirrelSaveGame* SaveGame = Cast<USquirrelSaveGame>(UGameplayStatics::CreateSaveGameObject(USquirrelSaveGame::StaticClass()));
	if (!SaveGame)
	{
		return false;
	}

	SaveGame->Coins = Coins;
	SaveGame->ProgressStats = ProgressStats;
	SaveGame->bHasParkSquirrelTransform = bHasParkSquirrelTransform;
	SaveGame->ParkSquirrelTransform = ParkSquirrelTransform;
	return UGameplayStatics::SaveGameToSlot(SaveGame, SaveSlotName, SaveUserIndex);
}

bool USquirrelGameInstance::LoadProgress()
{
	if (!UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex))
	{
		return false;
	}

	USquirrelSaveGame* SaveGame = Cast<USquirrelSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveUserIndex));
	if (!SaveGame)
	{
		return false;
	}

	Coins = FMath::Max(SaveGame->Coins, 0);
	ProgressStats = SaveGame->ProgressStats;
	bHasParkSquirrelTransform = SaveGame->bHasParkSquirrelTransform;
	ParkSquirrelTransform = SaveGame->ParkSquirrelTransform;
	EnsureDefaultStats();
	BroadcastAllProgress();
	return true;
}

bool USquirrelGameInstance::ResetSavedProgress()
{
	InitializeZeroProgress();
	const bool bDeleted = UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex)
		? UGameplayStatics::DeleteGameInSlot(SaveSlotName, SaveUserIndex)
		: true;
	AutoSaveIfNeeded();
	return bDeleted;
}

void USquirrelGameInstance::AddCoins(int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	Coins += Amount;
	OnCoinsChanged.Broadcast(Coins);
	AutoSaveIfNeeded();
}

bool USquirrelGameInstance::TrySpendCoins(int32 Amount)
{
	if (Amount <= 0)
	{
		return true;
	}

	if (Coins < Amount)
	{
		return false;
	}

	Coins -= Amount;
	OnCoinsChanged.Broadcast(Coins);
	AutoSaveIfNeeded();
	return true;
}

bool USquirrelGameInstance::AddProgressToStat(ESquirrelProgressStat Stat, int32 Amount)
{
	if (Amount <= 0)
	{
		return false;
	}

	EnsureDefaultStats();
	FSquirrelLevelProgress& Progress = ProgressStats.FindOrAdd(Stat);
	const bool bLeveledUp = Progress.AddProgress(Amount);
	BroadcastStat(Stat);
	AutoSaveIfNeeded();
	return bLeveledUp;
}

bool USquirrelGameInstance::TrySpendStatLevels(ESquirrelProgressStat Stat, int32 Amount)
{
	if (Amount <= 0)
	{
		return true;
	}

	EnsureDefaultStats();
	FSquirrelLevelProgress& Progress = ProgressStats.FindOrAdd(Stat);
	if (!Progress.TrySpendLevels(Amount))
	{
		return false;
	}

	BroadcastStat(Stat);
	AutoSaveIfNeeded();
	return true;
}

FSquirrelLevelProgress USquirrelGameInstance::GetProgressStat(ESquirrelProgressStat Stat) const
{
	if (const FSquirrelLevelProgress* Progress = ProgressStats.Find(Stat))
	{
		return *Progress;
	}

	FSquirrelLevelProgress DefaultProgress;
	ApplyTuningToProgress(Stat, DefaultProgress, true);
	DefaultProgress.Normalize();
	return DefaultProgress;
}

int32 USquirrelGameInstance::GetStatLevel(ESquirrelProgressStat Stat) const
{
	return GetProgressStat(Stat).Level;
}

int32 USquirrelGameInstance::GetStatProgressPoints(ESquirrelProgressStat Stat) const
{
	return GetProgressStat(Stat).ProgressPoints;
}

int32 USquirrelGameInstance::GetStatPointsPerLevel(ESquirrelProgressStat Stat) const
{
	return GetProgressStat(Stat).PointsPerLevel;
}

int32 USquirrelGameInstance::GetStatMaxLevel(ESquirrelProgressStat Stat) const
{
	return GetProgressStat(Stat).MaxLevel;
}

float USquirrelGameInstance::GetStatProgressPercent(ESquirrelProgressStat Stat) const
{
	return GetProgressStat(Stat).GetProgressPercent();
}

float USquirrelGameInstance::GetStatLevelPercent(ESquirrelProgressStat Stat) const
{
	return GetProgressStat(Stat).GetLevelPercent();
}

int32 USquirrelGameInstance::GetEnergyPointsPerFood() const
{
	return ProgressTuning ? FMath::Max(ProgressTuning->EnergyPointsPerFood, 0) : 3;
}

void USquirrelGameInstance::CaptureParkSquirrelTransform(AActor* SquirrelActor)
{
	if (!SquirrelActor)
	{
		return;
	}

	ParkSquirrelTransform = SquirrelActor->GetActorTransform();
	bHasParkSquirrelTransform = true;
	AutoSaveIfNeeded();
}

void USquirrelGameInstance::ClearParkSquirrelTransform()
{
	ParkSquirrelTransform = FTransform::Identity;
	bHasParkSquirrelTransform = false;
	AutoSaveIfNeeded();
}

bool USquirrelGameInstance::RestoreParkSquirrelTransform(AActor* SquirrelActor) const
{
	if (!SquirrelActor || !bHasParkSquirrelTransform)
	{
		return false;
	}

	SquirrelActor->SetActorTransform(ParkSquirrelTransform, false, nullptr, ETeleportType::TeleportPhysics);
	return true;
}

void USquirrelGameInstance::InitializeZeroProgress()
{
	Coins = 0;
	ProgressStats.Empty();

	for (const ESquirrelProgressStat Stat : {
		ESquirrelProgressStat::Energy,
		ESquirrelProgressStat::Running,
		ESquirrelProgressStat::Climbing,
		ESquirrelProgressStat::Flying,
		ESquirrelProgressStat::Swimming
	})
	{
		FSquirrelLevelProgress& Progress = ProgressStats.FindOrAdd(Stat);
		ApplyTuningToProgress(Stat, Progress, false);
		Progress.Level = 0;
		Progress.ProgressPoints = 0;
		Progress.Normalize();
	}

	bHasParkSquirrelTransform = false;
	ParkSquirrelTransform = FTransform::Identity;
	BroadcastAllProgress();
}

void USquirrelGameInstance::EnsureDefaultStats()
{
	const bool bHadEnergy = ProgressStats.Contains(ESquirrelProgressStat::Energy);
	FSquirrelLevelProgress& Energy = ProgressStats.FindOrAdd(ESquirrelProgressStat::Energy);
	ApplyTuningToProgress(ESquirrelProgressStat::Energy, Energy, !bHadEnergy);
	Energy.Normalize();

	for (const ESquirrelProgressStat Stat : {
		ESquirrelProgressStat::Running,
		ESquirrelProgressStat::Climbing,
		ESquirrelProgressStat::Flying,
		ESquirrelProgressStat::Swimming
	})
	{
		const bool bHadProgress = ProgressStats.Contains(Stat);
		FSquirrelLevelProgress& Progress = ProgressStats.FindOrAdd(Stat);
		ApplyTuningToProgress(Stat, Progress, !bHadProgress);
		Progress.Normalize();
	}
}

void USquirrelGameInstance::ApplyTuningToProgress(ESquirrelProgressStat Stat, FSquirrelLevelProgress& Progress, bool bApplyStartingLevel) const
{
	if (ProgressTuning)
	{
		const FSquirrelStatTuning Tuning = ProgressTuning->GetTuningForStat(Stat);
		if (bApplyStartingLevel)
		{
			Progress.Level = Tuning.StartingLevel;
		}
		Progress.MaxLevel = Tuning.MaxLevel;
		Progress.PointsPerLevel = Tuning.PointsPerLevel;
		return;
	}

	if (bApplyStartingLevel)
	{
		Progress.Level = Stat == ESquirrelProgressStat::Energy ? StartingEnergyLevel : 1;
	}
	Progress.MaxLevel = Stat == ESquirrelProgressStat::Energy ? EnergyMaxLevel : MiniGameMaxLevel;
	Progress.PointsPerLevel = PointsPerLevel;
}

int32 USquirrelGameInstance::GetConfiguredStartingCoins() const
{
	return ProgressTuning ? FMath::Max(ProgressTuning->StartingCoins, 0) : StartingCoins;
}

void USquirrelGameInstance::BroadcastAllProgress()
{
	OnCoinsChanged.Broadcast(Coins);
	for (const TPair<ESquirrelProgressStat, FSquirrelLevelProgress>& Pair : ProgressStats)
	{
		OnStatChanged.Broadcast(Pair.Key, Pair.Value.Level, Pair.Value.ProgressPoints, Pair.Value.MaxLevel);
	}
}

void USquirrelGameInstance::BroadcastStat(ESquirrelProgressStat Stat)
{
	const FSquirrelLevelProgress Progress = GetProgressStat(Stat);
	OnStatChanged.Broadcast(Stat, Progress.Level, Progress.ProgressPoints, Progress.MaxLevel);
}

void USquirrelGameInstance::AutoSaveIfNeeded()
{
	if (bAutoSaveOnChange)
	{
		SaveProgress();
	}
}
