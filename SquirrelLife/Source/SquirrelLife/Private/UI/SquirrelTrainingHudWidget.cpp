// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/SquirrelTrainingHudWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Widget.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Progress/SquirrelGameInstance.h"
#include "Progress/SquirrelProgressTuningData.h"
#include "Training/SquirrelTrainingGameMode.h"
#include "Training/SquirrelTrainingPawn.h"
#include "Training/SquirrelTrainingPlayerController.h"

void USquirrelTrainingHudWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BuildHud();
	BindEventSources();
	BindMiniGameMenu();
	SetMiniGameMenuVisible(false);
	Refresh();
}

void USquirrelTrainingHudWidget::NativeDestruct()
{
	UnbindMiniGameMenu();
	UnbindEventSources();

	Super::NativeDestruct();
}

void USquirrelTrainingHudWidget::BuildHud()
{
	if (EnergyText || EnergyValueText || CoinsText || CoinsValueText || EnergyBar || EnergyProgressBar || EnergyOverallBar)
	{
		return;
	}

	EnsureWidgetTree();

	if (!WidgetTree || WidgetTree->RootWidget)
	{
		return;
	}

	UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("TrainingHudRoot"));
	WidgetTree->RootWidget = RootCanvas;

	UBorder* StatsPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("StatsPanel"));
	StatsPanel->SetBrushColor(FLinearColor(0.08f, 0.11f, 0.13f, 0.78f));
	StatsPanel->SetPadding(FMargin(12.0f, 9.0f));

	UCanvasPanelSlot* PanelSlot = RootCanvas->AddChildToCanvas(StatsPanel);
	PanelSlot->SetAnchors(FAnchors(0.0f, 0.0f));
	PanelSlot->SetAlignment(FVector2D(0.0f, 0.0f));
	PanelSlot->SetPosition(FVector2D(18.0f, 18.0f));
	PanelSlot->SetAutoSize(true);

	UVerticalBox* PanelStack = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("StatsStack"));
	StatsPanel->SetContent(PanelStack);

	UHorizontalBox* EnergyRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("EnergyRow"));
	UVerticalBoxSlot* EnergyRowSlot = PanelStack->AddChildToVerticalBox(EnergyRow);
	EnergyRowSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));

	EnergyText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("EnergyText"));
	EnergyText->SetText(FText::FromString(TEXT("Energy: Lvl 0")));
	EnergyText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	EnergyText->SetShadowOffset(FVector2D(1.0f, 1.0f));

	UHorizontalBoxSlot* EnergyTextSlot = EnergyRow->AddChildToHorizontalBox(EnergyText);
	EnergyTextSlot->SetPadding(FMargin(0.0f, 0.0f, 10.0f, 0.0f));
	EnergyTextSlot->SetVerticalAlignment(VAlign_Center);

	USizeBox* EnergyBarBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("EnergyBarBox"));
	EnergyBarBox->SetWidthOverride(180.0f);
	EnergyBarBox->SetHeightOverride(14.0f);

	EnergyBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("EnergyBar"));
	EnergyBar->SetFillColorAndOpacity(FLinearColor(1.0f, 0.9f, 0.05f, 1.0f));
	EnergyBarBox->SetContent(EnergyBar);

	UHorizontalBoxSlot* EnergyBarSlot = EnergyRow->AddChildToHorizontalBox(EnergyBarBox);
	EnergyBarSlot->SetVerticalAlignment(VAlign_Center);

	UBorder* CoinsPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("CoinsPanel"));
	CoinsPanel->SetBrushColor(FLinearColor(0.12f, 0.21f, 0.30f, 0.82f));
	CoinsPanel->SetPadding(FMargin(10.0f, 6.0f));

	CoinsText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CoinsText"));
	CoinsText->SetText(FText::FromString(TEXT("Coins: 0")));
	CoinsText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	CoinsText->SetShadowOffset(FVector2D(1.0f, 1.0f));
	CoinsPanel->SetContent(CoinsText);

	PanelStack->AddChildToVerticalBox(CoinsPanel);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, TEXT("Training HUD layout built."));
	}
}

void USquirrelTrainingHudWidget::Refresh()
{
	ASquirrelTrainingPlayerController* TrainingController = Cast<ASquirrelTrainingPlayerController>(GetOwningPlayer());
	ASquirrelTrainingPawn* Squirrel = TrainingController ? Cast<ASquirrelTrainingPawn>(TrainingController->GetPawn()) : nullptr;

	const int32 EnergyLevel = Squirrel ? Squirrel->GetEnergyLevel() : 0;
	const int32 MaxEnergyLevel = Squirrel ? Squirrel->GetMaxEnergyLevel() : 1;
	const int32 EnergyProgressPoints = Squirrel ? Squirrel->GetEnergyProgressPoints() : 0;
	const int32 EnergyPointsPerLevel = Squirrel ? Squirrel->GetEnergyPointsPerLevel() : 1;
	const int32 Coins = TrainingController ? TrainingController->GetMoney() : 0;

	UpdateEnergyDisplay(EnergyLevel, EnergyProgressPoints, MaxEnergyLevel, EnergyPointsPerLevel);
	UpdateCoinsDisplay(Coins);
	RefreshRunFromProgress();
}

void USquirrelTrainingHudWidget::EnsureWidgetTree()
{
	if (!WidgetTree)
	{
		WidgetTree = NewObject<UWidgetTree>(this, TEXT("WidgetTree"));
	}
}

void USquirrelTrainingHudWidget::BindEventSources()
{
	UnbindEventSources();

	BoundTrainingController = Cast<ASquirrelTrainingPlayerController>(GetOwningPlayer());
	BoundSquirrel = BoundTrainingController ? Cast<ASquirrelTrainingPawn>(BoundTrainingController->GetPawn()) : nullptr;
	BoundProgressGameInstance = GetGameInstance<USquirrelGameInstance>();

	if (BoundTrainingController)
	{
		BoundTrainingController->OnMoneyChanged.AddUniqueDynamic(this, &USquirrelTrainingHudWidget::HandleMoneyChanged);
	}

	if (BoundSquirrel)
	{
		BoundSquirrel->OnEnergyChanged.AddUniqueDynamic(this, &USquirrelTrainingHudWidget::HandleEnergyChanged);
	}

	if (BoundProgressGameInstance)
	{
		BoundProgressGameInstance->OnCoinsChanged.AddUniqueDynamic(this, &USquirrelTrainingHudWidget::HandleCoinsChanged);
		BoundProgressGameInstance->OnStatChanged.AddUniqueDynamic(this, &USquirrelTrainingHudWidget::HandleStatChanged);
	}
}

void USquirrelTrainingHudWidget::UnbindEventSources()
{
	if (BoundTrainingController)
	{
		BoundTrainingController->OnMoneyChanged.RemoveDynamic(this, &USquirrelTrainingHudWidget::HandleMoneyChanged);
	}

	if (BoundSquirrel)
	{
		BoundSquirrel->OnEnergyChanged.RemoveDynamic(this, &USquirrelTrainingHudWidget::HandleEnergyChanged);
	}

	if (BoundProgressGameInstance)
	{
		BoundProgressGameInstance->OnCoinsChanged.RemoveDynamic(this, &USquirrelTrainingHudWidget::HandleCoinsChanged);
		BoundProgressGameInstance->OnStatChanged.RemoveDynamic(this, &USquirrelTrainingHudWidget::HandleStatChanged);
	}

	BoundTrainingController = nullptr;
	BoundSquirrel = nullptr;
	BoundProgressGameInstance = nullptr;
}

void USquirrelTrainingHudWidget::BindMiniGameMenu()
{
	if (MiniGameMenuButton)
	{
		MiniGameMenuButton->OnClicked.AddUniqueDynamic(this, &USquirrelTrainingHudWidget::HandleMiniGameMenuClicked);
	}

	if (CloseMiniGameMenuButton)
	{
		CloseMiniGameMenuButton->OnClicked.AddUniqueDynamic(this, &USquirrelTrainingHudWidget::HandleCloseMiniGameMenuClicked);
	}

	if (RunMiniGameButton)
	{
		RunMiniGameButton->OnClicked.AddUniqueDynamic(this, &USquirrelTrainingHudWidget::HandleRunMiniGameClicked);
	}
}

void USquirrelTrainingHudWidget::UnbindMiniGameMenu()
{
	if (MiniGameMenuButton)
	{
		MiniGameMenuButton->OnClicked.RemoveDynamic(this, &USquirrelTrainingHudWidget::HandleMiniGameMenuClicked);
	}

	if (CloseMiniGameMenuButton)
	{
		CloseMiniGameMenuButton->OnClicked.RemoveDynamic(this, &USquirrelTrainingHudWidget::HandleCloseMiniGameMenuClicked);
	}

	if (RunMiniGameButton)
	{
		RunMiniGameButton->OnClicked.RemoveDynamic(this, &USquirrelTrainingHudWidget::HandleRunMiniGameClicked);
	}
}

void USquirrelTrainingHudWidget::SetMiniGameMenuVisible(bool bVisible)
{
	if (MiniGameMenuPanel)
	{
		MiniGameMenuPanel->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void USquirrelTrainingHudWidget::HandleMoneyChanged(int32 NewMoney)
{
	UpdateCoinsDisplay(NewMoney);
}

void USquirrelTrainingHudWidget::HandleEnergyChanged(int32 NewEnergyLevel, int32 NewEnergyProgressPoints, int32 NewMaxEnergyLevel)
{
	const int32 PointsPerLevel = BoundSquirrel ? BoundSquirrel->GetEnergyPointsPerLevel() : 1;
	UpdateEnergyDisplay(NewEnergyLevel, NewEnergyProgressPoints, NewMaxEnergyLevel, PointsPerLevel);
}

void USquirrelTrainingHudWidget::HandleCoinsChanged(int32 NewCoins)
{
	UpdateCoinsDisplay(NewCoins);
}

void USquirrelTrainingHudWidget::HandleStatChanged(ESquirrelProgressStat Stat, int32 NewLevel, int32 NewProgressPoints, int32 NewMaxLevel)
{
	if (Stat == ESquirrelProgressStat::Energy)
	{
		const int32 PointsPerLevel = BoundProgressGameInstance ? BoundProgressGameInstance->GetStatPointsPerLevel(Stat) : 1;
		UpdateEnergyDisplay(NewLevel, NewProgressPoints, NewMaxLevel, PointsPerLevel);
		return;
	}

	if (Stat == ESquirrelProgressStat::Running)
	{
		const int32 PointsPerLevel = BoundProgressGameInstance ? BoundProgressGameInstance->GetStatPointsPerLevel(Stat) : 1;
		UpdateRunDisplay(NewLevel, NewProgressPoints, NewMaxLevel, PointsPerLevel);
		return;
	}
}

void USquirrelTrainingHudWidget::HandleMiniGameMenuClicked()
{
	if (!MiniGameMenuPanel)
	{
		return;
	}

	const bool bIsVisible = MiniGameMenuPanel->GetVisibility() != ESlateVisibility::Collapsed
		&& MiniGameMenuPanel->GetVisibility() != ESlateVisibility::Hidden;
	SetMiniGameMenuVisible(!bIsVisible);
}

void USquirrelTrainingHudWidget::HandleCloseMiniGameMenuClicked()
{
	SetMiniGameMenuVisible(false);
}

void USquirrelTrainingHudWidget::HandleRunMiniGameClicked()
{
	OpenRunningMiniGame();
}

void USquirrelTrainingHudWidget::OpenRunningMiniGame()
{
	if (!RunningMiniGameLevelName.IsNone())
	{
		UGameplayStatics::OpenLevel(this, RunningMiniGameLevelName);
	}
}

void USquirrelTrainingHudWidget::UpdateCoinsDisplay(int32 NewCoins)
{
	if (CoinsText)
	{
		CoinsText->SetText(FText::Format(FText::FromString(TEXT("Coins: {0}")), FText::AsNumber(NewCoins)));
	}

	if (CoinsValueText)
	{
		CoinsValueText->SetText(FText::AsNumber(NewCoins));
	}
}

void USquirrelTrainingHudWidget::UpdateEnergyDisplay(int32 NewEnergyLevel, int32 NewEnergyProgressPoints, int32 NewMaxEnergyLevel, int32 PointsPerLevel)
{
	const int32 SafePointsPerLevel = FMath::Max(PointsPerLevel, 1);
	const int32 SafeMaxEnergyLevel = FMath::Max(NewMaxEnergyLevel, 1);
	const float EnergyProgressPercent = static_cast<float>(FMath::Clamp(NewEnergyProgressPoints, 0, SafePointsPerLevel)) / static_cast<float>(SafePointsPerLevel);
	const float EnergyLevelPercent = static_cast<float>(FMath::Clamp(NewEnergyLevel, 0, SafeMaxEnergyLevel)) / static_cast<float>(SafeMaxEnergyLevel);

	if (EnergyText)
	{
		EnergyText->SetText(FText::Format(FText::FromString(TEXT("Energy: Lvl {0}")), FText::AsNumber(NewEnergyLevel)));
	}

	if (EnergyValueText)
	{
		EnergyValueText->SetText(FText::AsNumber(NewEnergyLevel));
	}

	if (MaxEnergyValueText)
	{
		MaxEnergyValueText->SetText(FText::AsNumber(NewMaxEnergyLevel));
	}

	if (EnergyBar)
	{
		EnergyBar->SetPercent(EnergyProgressPercent);
	}

	if (EnergyProgressBar)
	{
		EnergyProgressBar->SetPercent(EnergyProgressPercent);
	}

	if (EnergyOverallBar)
	{
		EnergyOverallBar->SetPercent(EnergyLevelPercent);
	}

	if (EnergyProgressText)
	{
		EnergyProgressText->SetText(FText::Format(
			FText::FromString(TEXT("{0}/{1}")),
			FText::AsNumber(NewEnergyProgressPoints),
			FText::AsNumber(SafePointsPerLevel)));
	}
}

void USquirrelTrainingHudWidget::UpdateRunDisplay(int32 NewRunLevel, int32 NewRunProgressPoints, int32 NewRunMaxLevel, int32 PointsPerLevel)
{
	const int32 SafePointsPerLevel = FMath::Max(PointsPerLevel, 1);
	const int32 SafeMaxRunLevel = FMath::Max(NewRunMaxLevel, 1);
	const float RunProgressPercent = static_cast<float>(FMath::Clamp(NewRunProgressPoints, 0, SafePointsPerLevel)) / static_cast<float>(SafePointsPerLevel);
	const float RunLevelPercent = static_cast<float>(FMath::Clamp(NewRunLevel, 0, SafeMaxRunLevel)) / static_cast<float>(SafeMaxRunLevel);

	if (RunText)
	{
		RunText->SetText(FText::Format(FText::FromString(TEXT("Run: Lvl {0}")), FText::AsNumber(NewRunLevel)));
	}

	if (RunProgressBar)
	{
		RunProgressBar->SetPercent(RunProgressPercent);
	}

	if (RunOverallBar)
	{
		RunOverallBar->SetPercent(RunLevelPercent);
	}
}

void USquirrelTrainingHudWidget::RefreshRunFromProgress()
{
	if (const USquirrelGameInstance* ProgressGameInstance = GetGameInstance<USquirrelGameInstance>())
	{
		UpdateRunDisplay(
			ProgressGameInstance->GetStatLevel(ESquirrelProgressStat::Running),
			ProgressGameInstance->GetStatProgressPoints(ESquirrelProgressStat::Running),
			ProgressGameInstance->GetStatMaxLevel(ESquirrelProgressStat::Running),
			ProgressGameInstance->GetStatPointsPerLevel(ESquirrelProgressStat::Running));
		return;
	}

	FSquirrelStatTuning RunTuning;
	if (const ASquirrelTrainingGameMode* TrainingGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ASquirrelTrainingGameMode>() : nullptr)
	{
		if (const USquirrelProgressTuningData* ProgressTuning = TrainingGameMode->GetProgressTuning())
		{
			RunTuning = ProgressTuning->GetTuningForStat(ESquirrelProgressStat::Running);
		}
	}

	UpdateRunDisplay(RunTuning.StartingLevel, 0, RunTuning.MaxLevel, RunTuning.PointsPerLevel);
}
