// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Progress/SquirrelProgressTuningData.h"
#include "Progress/SquirrelProgressTypes.h"
#include "SquirrelMiniGameModeBase.generated.h"

class USquirrelGameInstance;
class USquirrelMiniGameHudWidget;
class USquirrelMiniGameResultWidget;

USTRUCT(BlueprintType)
struct FSquirrelMiniGameResult
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Result")
	float DistanceTravelled = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Result")
	float DisplayDistance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Result")
	int32 XPGained = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Result")
	int32 CoinsCollected = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Result")
	int32 TotalCoins = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Result")
	ESquirrelProgressStat TrainedStat = ESquirrelProgressStat::Running;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Result")
	int32 StatLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Result")
	int32 StatProgressPoints = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Result")
	int32 StatPointsPerLevel = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Result")
	int32 StatMaxLevel = 50;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSquirrelMiniGameLiveStatsChangedSignature, float, Distance, float, Speed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSquirrelMiniGameEndedSignature, const FSquirrelMiniGameResult&, Result);

UCLASS(Blueprintable)
class SQUIRRELLIFE_API ASquirrelMiniGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASquirrelMiniGameModeBase();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Progress")
	ESquirrelProgressStat TrainedStat = ESquirrelProgressStat::Running;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Tuning")
	TObjectPtr<USquirrelProgressTuningData> ProgressTuning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Rewards", meta = (ClampMin = "0"))
	int32 CoinsPerScoreUnit = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Rewards", meta = (ClampMin = "0"))
	int32 ProgressPerScoreUnit = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Rewards", meta = (ClampMin = "1", Units = "cm"))
	float DistancePerScoreUnit = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Rewards", meta = (ClampMin = "1", Units = "cm"))
	float DistancePerCoin = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Rewards")
	bool bAwardCoinsFromDistance = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Rewards", meta = (ClampMin = "1", Units = "cm"))
	float DistanceDisplayDivisor = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Movement", meta = (ClampMin = "0", Units = "cm/s"))
	float BaseScrollSpeed = 280.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Movement", meta = (ClampMin = "0", Units = "cm/s"))
	float ScrollSpeedPerLevel = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Session")
	bool bAutoStartSession = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Session", meta = (ClampMin = "0", Units = "s"))
	float ResultScreenDelay = 0.85f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|UI")
	bool bShowMiniGameHud = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|UI", meta = (EditCondition = "bShowMiniGameHud"))
	TSubclassOf<USquirrelMiniGameHudWidget> MiniGameHudWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|UI")
	TSubclassOf<USquirrelMiniGameResultWidget> ResultWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Level")
	FName ParkLevelName = TEXT("ParkLevel");

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Mini Game|Session")
	bool bSessionActive = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Mini Game|Session")
	bool bSessionEnded = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Mini Game|Session")
	float DistanceTravelled = 0.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Mini Game|Session")
	float CurrentRunSpeed = 0.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Mini Game|Session")
	int32 CoinsCollectedThisRun = 0;

	UPROPERTY()
	TObjectPtr<USquirrelMiniGameHudWidget> MiniGameHudWidget;

	UPROPERTY()
	TObjectPtr<USquirrelMiniGameResultWidget> ResultWidget;

	UPROPERTY()
	TObjectPtr<APawn> SessionPawn;

	FVector SessionStartLocation = FVector::ZeroVector;
	FSquirrelMiniGameResult LastResult;
	FTimerHandle ResultScreenTimer;

	USquirrelGameInstance* GetProgressGameInstance() const;
	FSquirrelMiniGameTuning GetResolvedMiniGameTuning() const;
	APawn* GetSessionPawn();
	void SetupMiniGameHud();
	void ShowResultWidget(const FSquirrelMiniGameResult& Result);
	void UpdateSession(float DeltaSeconds);
	FSquirrelMiniGameResult BuildResult() const;
	void FinishMiniGameSession();

public:
	UPROPERTY(BlueprintAssignable, Category = "Mini Game|Session")
	FSquirrelMiniGameLiveStatsChangedSignature OnLiveStatsChanged;

	UPROPERTY(BlueprintAssignable, Category = "Mini Game|Session")
	FSquirrelMiniGameEndedSignature OnMiniGameEnded;

	UFUNCTION(BlueprintCallable, Category = "Mini Game|Session")
	void StartMiniGameSession();

	UFUNCTION(BlueprintCallable, Category = "Mini Game|Session")
	void EndMiniGameSession();

	UFUNCTION(BlueprintPure, Category = "Mini Game|Session")
	bool IsSessionActive() const { return bSessionActive; }

	UFUNCTION(BlueprintPure, Category = "Mini Game|Session")
	float GetDistanceTravelled() const { return DistanceTravelled; }

	UFUNCTION(BlueprintPure, Category = "Mini Game|Session")
	float GetDisplayDistance() const;

	UFUNCTION(BlueprintPure, Category = "Mini Game|Session")
	float GetCurrentRunSpeed() const { return CurrentRunSpeed; }

	UFUNCTION(BlueprintPure, Category = "Mini Game|Session")
	const FSquirrelMiniGameResult& GetLastResult() const { return LastResult; }

	UFUNCTION(BlueprintCallable, Category = "Mini Game|Start")
	bool TrySpendStartEnergy();

	UFUNCTION(BlueprintCallable, Category = "Mini Game|Rewards")
	void AwardCoins(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Mini Game|Rewards")
	void CollectMiniGameCoins(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Mini Game|Rewards")
	bool AwardStatProgress(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Mini Game|Rewards")
	void AwardScoreUnits(int32 ScoreUnits);

	UFUNCTION(BlueprintPure, Category = "Mini Game|Progress")
	ESquirrelProgressStat GetTrainedStat() const { return TrainedStat; }

	UFUNCTION(BlueprintPure, Category = "Mini Game|Tuning")
	USquirrelProgressTuningData* GetProgressTuning() const;

	UFUNCTION(BlueprintPure, Category = "Mini Game|Tuning")
	USquirrelProgressTuningData* GetAssignedProgressTuning() const { return ProgressTuning; }

	UFUNCTION(BlueprintPure, Category = "Mini Game|Progress")
	int32 GetTrainedStatLevel() const;

	UFUNCTION(BlueprintPure, Category = "Mini Game|Progress")
	int32 GetTrainedStatProgressPoints() const;

	UFUNCTION(BlueprintPure, Category = "Mini Game|Movement")
	float GetCurrentScrollSpeed() const;

	UFUNCTION(BlueprintCallable, Category = "Mini Game|Level")
	void ReturnToParkLevel();

	UFUNCTION(BlueprintCallable, Category = "Mini Game|Level")
	void RetryMiniGame();
};
