// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Progress/SquirrelProgressTypes.h"
#include "SquirrelGameInstance.generated.h"

class AActor;
class USquirrelProgressTuningData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSquirrelCoinsChangedSignature, int32, NewCoins);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSquirrelStatChangedSignature, ESquirrelProgressStat, Stat, int32, NewLevel, int32, NewProgressPoints, int32, NewMaxLevel);

UCLASS(Blueprintable)
class SQUIRRELLIFE_API USquirrelGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress|Save")
	FString SaveSlotName = TEXT("SquirrelLifeProgress");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress|Save")
	int32 SaveUserIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress|Save")
	bool bAutoLoadOnInit = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress|Save")
	bool bAutoSaveOnChange = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress|Defaults", meta = (ClampMin = "0"))
	int32 StartingCoins = 25;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress|Defaults")
	TObjectPtr<USquirrelProgressTuningData> ProgressTuning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress|Defaults", meta = (ClampMin = "0"))
	int32 StartingEnergyLevel = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress|Defaults", meta = (ClampMin = "1"))
	int32 EnergyMaxLevel = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress|Defaults", meta = (ClampMin = "1"))
	int32 MiniGameMaxLevel = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress|Defaults", meta = (ClampMin = "1"))
	int32 PointsPerLevel = 5;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Progress|Economy")
	int32 Coins = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Progress|Stats")
	TMap<ESquirrelProgressStat, FSquirrelLevelProgress> ProgressStats;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Progress|Park")
	bool bHasParkSquirrelTransform = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Progress|Park")
	FTransform ParkSquirrelTransform = FTransform::Identity;

public:
	UPROPERTY(BlueprintAssignable, Category = "Progress|Economy")
	FSquirrelCoinsChangedSignature OnCoinsChanged;

	UPROPERTY(BlueprintAssignable, Category = "Progress|Stats")
	FSquirrelStatChangedSignature OnStatChanged;

	UFUNCTION(BlueprintCallable, Category = "Progress")
	void InitializeNewProgress();

	UFUNCTION(BlueprintCallable, Category = "Progress|Save")
	bool SaveProgress();

	UFUNCTION(BlueprintCallable, Category = "Progress|Save")
	bool LoadProgress();

	UFUNCTION(BlueprintCallable, Category = "Progress|Save")
	bool ResetSavedProgress();

	UFUNCTION(BlueprintCallable, Category = "Progress|Economy")
	void AddCoins(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Progress|Economy")
	bool TrySpendCoins(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Progress|Economy")
	int32 GetCoins() const { return Coins; }

	UFUNCTION(BlueprintCallable, Category = "Progress|Stats")
	bool AddProgressToStat(ESquirrelProgressStat Stat, int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Progress|Stats")
	bool TrySpendStatLevels(ESquirrelProgressStat Stat, int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Progress|Stats")
	FSquirrelLevelProgress GetProgressStat(ESquirrelProgressStat Stat) const;

	UFUNCTION(BlueprintPure, Category = "Progress|Stats")
	int32 GetStatLevel(ESquirrelProgressStat Stat) const;

	UFUNCTION(BlueprintPure, Category = "Progress|Stats")
	int32 GetStatProgressPoints(ESquirrelProgressStat Stat) const;

	UFUNCTION(BlueprintPure, Category = "Progress|Stats")
	int32 GetStatPointsPerLevel(ESquirrelProgressStat Stat) const;

	UFUNCTION(BlueprintPure, Category = "Progress|Stats")
	int32 GetStatMaxLevel(ESquirrelProgressStat Stat) const;

	UFUNCTION(BlueprintPure, Category = "Progress|Stats")
	float GetStatProgressPercent(ESquirrelProgressStat Stat) const;

	UFUNCTION(BlueprintPure, Category = "Progress|Stats")
	float GetStatLevelPercent(ESquirrelProgressStat Stat) const;

	UFUNCTION(BlueprintPure, Category = "Progress|Training")
	int32 GetEnergyPointsPerFood() const;

	UFUNCTION(BlueprintPure, Category = "Progress|Tuning")
	USquirrelProgressTuningData* GetProgressTuning() const { return ProgressTuning; }

	UFUNCTION(BlueprintCallable, Category = "Progress|Park")
	void CaptureParkSquirrelTransform(AActor* SquirrelActor);

	UFUNCTION(BlueprintCallable, Category = "Progress|Park")
	void ClearParkSquirrelTransform();

	UFUNCTION(BlueprintCallable, Category = "Progress|Park")
	bool RestoreParkSquirrelTransform(AActor* SquirrelActor) const;

protected:
	void InitializeZeroProgress();
	void EnsureDefaultStats();
	void ApplyTuningToProgress(ESquirrelProgressStat Stat, FSquirrelLevelProgress& Progress, bool bApplyStartingLevel) const;
	int32 GetConfiguredStartingCoins() const;
	void BroadcastAllProgress();
	void BroadcastStat(ESquirrelProgressStat Stat);
	void AutoSaveIfNeeded();
};
