// Copyright Epic Games, Inc. All Rights Reserved.

#include "MiniGames/SquirrelMiniGameModeBase.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Progress/SquirrelGameInstance.h"
#include "Progress/SquirrelProgressTuningData.h"
#include "Progress/SquirrelTuningComponent.h"
#include "TimerManager.h"
#include "UI/SquirrelMiniGameHudWidget.h"
#include "UI/SquirrelMiniGameResultWidget.h"

ASquirrelMiniGameModeBase::ASquirrelMiniGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
	MiniGameHudWidgetClass = USquirrelMiniGameHudWidget::StaticClass();
	ResultWidgetClass = USquirrelMiniGameResultWidget::StaticClass();
}

void ASquirrelMiniGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SetupMiniGameHud();

	if (bAutoStartSession)
	{
		StartMiniGameSession();
	}
}

void ASquirrelMiniGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateSession(DeltaSeconds);
}

USquirrelGameInstance* ASquirrelMiniGameModeBase::GetProgressGameInstance() const
{
	return GetGameInstance<USquirrelGameInstance>();
}

USquirrelProgressTuningData* ASquirrelMiniGameModeBase::GetProgressTuning() const
{
	if (ProgressTuning)
	{
		return ProgressTuning;
	}

	if (const APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0))
	{
		if (const USquirrelTuningComponent* PawnTuningComponent = Pawn->FindComponentByClass<USquirrelTuningComponent>())
		{
			if (USquirrelProgressTuningData* PawnTuning = PawnTuningComponent->GetAssignedProgressTuning())
			{
				return PawnTuning;
			}
		}
	}

	if (const USquirrelGameInstance* SquirrelGameInstance = GetProgressGameInstance())
	{
		return SquirrelGameInstance->GetProgressTuning();
	}

	return nullptr;
}

FSquirrelMiniGameTuning ASquirrelMiniGameModeBase::GetResolvedMiniGameTuning() const
{
	if (const USquirrelProgressTuningData* TuningData = GetProgressTuning())
	{
		return TuningData->GetMiniGameTuningForStat(TrainedStat);
	}

	FSquirrelMiniGameTuning Tuning;
	Tuning.CoinsPerScoreUnit = CoinsPerScoreUnit;
	Tuning.ProgressPerScoreUnit = ProgressPerScoreUnit;
	Tuning.MinimumProgressPerRun = 1;
	Tuning.DistancePerScoreUnit = DistancePerScoreUnit;
	Tuning.DistancePerCoin = DistancePerCoin;
	Tuning.DistanceDisplayDivisor = DistanceDisplayDivisor;
	Tuning.BaseScrollSpeed = BaseScrollSpeed;
	Tuning.ScrollSpeedPerLevel = ScrollSpeedPerLevel;
	return Tuning;
}

APawn* ASquirrelMiniGameModeBase::GetSessionPawn()
{
	if (!SessionPawn)
	{
		SessionPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	}

	return SessionPawn;
}

void ASquirrelMiniGameModeBase::SetupMiniGameHud()
{
	if (!bShowMiniGameHud || MiniGameHudWidget || !MiniGameHudWidgetClass)
	{
		return;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!PlayerController)
	{
		return;
	}

	MiniGameHudWidget = CreateWidget<USquirrelMiniGameHudWidget>(PlayerController, MiniGameHudWidgetClass);
	if (MiniGameHudWidget)
	{
		MiniGameHudWidget->SetMiniGameMode(this);
		MiniGameHudWidget->AddToPlayerScreen(5);
	}
}

void ASquirrelMiniGameModeBase::ShowResultWidget(const FSquirrelMiniGameResult& Result)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!PlayerController || !ResultWidgetClass)
	{
		return;
	}

	if (MiniGameHudWidget)
	{
		MiniGameHudWidget->RemoveFromParent();
		MiniGameHudWidget = nullptr;
	}

	if (!ResultWidget)
	{
		ResultWidget = CreateWidget<USquirrelMiniGameResultWidget>(PlayerController, ResultWidgetClass);
	}

	if (ResultWidget)
	{
		ResultWidget->SetMiniGameMode(this);
		ResultWidget->SetResult(Result);
		ResultWidget->AddToPlayerScreen(20);
	}

	PlayerController->SetPause(true);
	PlayerController->bShowMouseCursor = true;
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(InputMode);
}

void ASquirrelMiniGameModeBase::UpdateSession(float DeltaSeconds)
{
	if (!bSessionActive || bSessionEnded)
	{
		return;
	}

	APawn* Pawn = GetSessionPawn();
	if (!Pawn)
	{
		return;
	}

	const FVector CurrentLocation = Pawn->GetActorLocation();
	DistanceTravelled = FMath::Max(CurrentLocation.X - SessionStartLocation.X, 0.0f);
	CurrentRunSpeed = Pawn->GetVelocity().Size2D();
	OnLiveStatsChanged.Broadcast(GetDisplayDistance(), CurrentRunSpeed);
}

FSquirrelMiniGameResult ASquirrelMiniGameModeBase::BuildResult() const
{
	FSquirrelMiniGameResult Result;
	Result.DistanceTravelled = DistanceTravelled;
	Result.DisplayDistance = GetDisplayDistance();
	Result.TrainedStat = TrainedStat;

	const FSquirrelMiniGameTuning Tuning = GetResolvedMiniGameTuning();
	const int32 ScoreUnits = FMath::FloorToInt(DistanceTravelled / FMath::Max(Tuning.DistancePerScoreUnit, 1.0f));
	Result.XPGained = FMath::Max(ScoreUnits * Tuning.ProgressPerScoreUnit, Tuning.MinimumProgressPerRun);
	Result.CoinsCollected = CoinsCollectedThisRun;
	if (bAwardCoinsFromDistance)
	{
		Result.CoinsCollected += FMath::FloorToInt(DistanceTravelled / FMath::Max(Tuning.DistancePerCoin, 1.0f)) * Tuning.CoinsPerScoreUnit;
	}

	if (const USquirrelGameInstance* SquirrelGameInstance = GetProgressGameInstance())
	{
		Result.TotalCoins = SquirrelGameInstance->GetCoins() + Result.CoinsCollected;
		Result.StatLevel = SquirrelGameInstance->GetStatLevel(TrainedStat);
		Result.StatProgressPoints = SquirrelGameInstance->GetStatProgressPoints(TrainedStat);
		Result.StatPointsPerLevel = SquirrelGameInstance->GetStatPointsPerLevel(TrainedStat);
		Result.StatMaxLevel = SquirrelGameInstance->GetStatMaxLevel(TrainedStat);
	}
	else
	{
		Result.TotalCoins = Result.CoinsCollected;
		const FSquirrelStatTuning StatTuning = GetProgressTuning()
			? GetProgressTuning()->GetTuningForStat(TrainedStat)
			: FSquirrelStatTuning();
		Result.StatPointsPerLevel = StatTuning.PointsPerLevel;
		Result.StatMaxLevel = StatTuning.MaxLevel;
		Result.StatLevel = FMath::Clamp(StatTuning.StartingLevel + Result.XPGained / Result.StatPointsPerLevel, 0, Result.StatMaxLevel);
		Result.StatProgressPoints = Result.XPGained % Result.StatPointsPerLevel;
	}

	return Result;
}

void ASquirrelMiniGameModeBase::StartMiniGameSession()
{
	bSessionActive = true;
	bSessionEnded = false;
	DistanceTravelled = 0.0f;
	CurrentRunSpeed = 0.0f;
	CoinsCollectedThisRun = 0;
	LastResult = FSquirrelMiniGameResult();
	SessionPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	SessionStartLocation = SessionPawn ? SessionPawn->GetActorLocation() : FVector::ZeroVector;
	OnLiveStatsChanged.Broadcast(0.0f, 0.0f);
}

void ASquirrelMiniGameModeBase::EndMiniGameSession()
{
	if (bSessionEnded)
	{
		return;
	}

	bSessionActive = false;
	bSessionEnded = true;

	LastResult = BuildResult();
	GetWorldTimerManager().SetTimer(ResultScreenTimer, this, &ASquirrelMiniGameModeBase::FinishMiniGameSession, ResultScreenDelay, false);
}

void ASquirrelMiniGameModeBase::FinishMiniGameSession()
{
	AwardCoins(LastResult.CoinsCollected);
	AwardStatProgress(LastResult.XPGained);

	if (const USquirrelGameInstance* SquirrelGameInstance = GetProgressGameInstance())
	{
		LastResult.TotalCoins = SquirrelGameInstance->GetCoins();
		LastResult.StatLevel = SquirrelGameInstance->GetStatLevel(TrainedStat);
		LastResult.StatProgressPoints = SquirrelGameInstance->GetStatProgressPoints(TrainedStat);
		LastResult.StatPointsPerLevel = SquirrelGameInstance->GetStatPointsPerLevel(TrainedStat);
		LastResult.StatMaxLevel = SquirrelGameInstance->GetStatMaxLevel(TrainedStat);
	}

	OnMiniGameEnded.Broadcast(LastResult);
	ShowResultWidget(LastResult);
}

float ASquirrelMiniGameModeBase::GetDisplayDistance() const
{
	return DistanceTravelled / FMath::Max(GetResolvedMiniGameTuning().DistanceDisplayDivisor, 1.0f);
}

bool ASquirrelMiniGameModeBase::TrySpendStartEnergy()
{
	return true;
}

void ASquirrelMiniGameModeBase::AwardCoins(int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	if (USquirrelGameInstance* SquirrelGameInstance = GetProgressGameInstance())
	{
		SquirrelGameInstance->AddCoins(Amount);
	}
}

void ASquirrelMiniGameModeBase::CollectMiniGameCoins(int32 Amount)
{
	if (Amount <= 0 || bSessionEnded)
	{
		return;
	}

	CoinsCollectedThisRun += Amount;
}

bool ASquirrelMiniGameModeBase::AwardStatProgress(int32 Amount)
{
	if (Amount <= 0)
	{
		return false;
	}

	if (USquirrelGameInstance* SquirrelGameInstance = GetProgressGameInstance())
	{
		return SquirrelGameInstance->AddProgressToStat(TrainedStat, Amount);
	}

	return false;
}

void ASquirrelMiniGameModeBase::AwardScoreUnits(int32 ScoreUnits)
{
	if (ScoreUnits <= 0)
	{
		return;
	}

	const FSquirrelMiniGameTuning Tuning = GetResolvedMiniGameTuning();
	AwardCoins(ScoreUnits * Tuning.CoinsPerScoreUnit);
	AwardStatProgress(ScoreUnits * Tuning.ProgressPerScoreUnit);
}

int32 ASquirrelMiniGameModeBase::GetTrainedStatLevel() const
{
	if (const USquirrelGameInstance* SquirrelGameInstance = GetProgressGameInstance())
	{
		return SquirrelGameInstance->GetStatLevel(TrainedStat);
	}

	return GetProgressTuning() ? GetProgressTuning()->GetTuningForStat(TrainedStat).StartingLevel : 1;
}

int32 ASquirrelMiniGameModeBase::GetTrainedStatProgressPoints() const
{
	if (const USquirrelGameInstance* SquirrelGameInstance = GetProgressGameInstance())
	{
		return SquirrelGameInstance->GetStatProgressPoints(TrainedStat);
	}

	return 0;
}

float ASquirrelMiniGameModeBase::GetCurrentScrollSpeed() const
{
	const FSquirrelMiniGameTuning Tuning = GetResolvedMiniGameTuning();
	return Tuning.BaseScrollSpeed + static_cast<float>(GetTrainedStatLevel()) * Tuning.ScrollSpeedPerLevel;
}

void ASquirrelMiniGameModeBase::ReturnToParkLevel()
{
	GetWorldTimerManager().ClearTimer(ResultScreenTimer);

	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
	{
		PlayerController->SetPause(false);
	}

	if (USquirrelGameInstance* SquirrelGameInstance = GetProgressGameInstance())
	{
		SquirrelGameInstance->SaveProgress();
	}

	UGameplayStatics::OpenLevel(this, ParkLevelName);
}

void ASquirrelMiniGameModeBase::RetryMiniGame()
{
	GetWorldTimerManager().ClearTimer(ResultScreenTimer);

	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
	{
		PlayerController->SetPause(false);
	}

	const FName CurrentLevelName(*UGameplayStatics::GetCurrentLevelName(this, true));
	UGameplayStatics::OpenLevel(this, CurrentLevelName);
}
