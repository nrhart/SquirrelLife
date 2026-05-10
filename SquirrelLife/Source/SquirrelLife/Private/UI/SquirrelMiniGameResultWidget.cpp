// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/SquirrelMiniGameResultWidget.h"

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
#include "Kismet/GameplayStatics.h"
#include "MiniGames/SquirrelMiniGameModeBase.h"

void USquirrelMiniGameResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BuildResultPanel();
	BindButtons();
	RefreshResult();
}

void USquirrelMiniGameResultWidget::SetMiniGameMode(ASquirrelMiniGameModeBase* NewMiniGameMode)
{
	MiniGameMode = NewMiniGameMode;
}

void USquirrelMiniGameResultWidget::SetResult(const FSquirrelMiniGameResult& NewResult)
{
	Result = NewResult;
	RefreshResult();
}

void USquirrelMiniGameResultWidget::BuildResultPanel()
{
	if (TitleText || !WidgetTree || WidgetTree->RootWidget)
	{
		return;
	}

	UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("MiniGameResultRoot"));
	WidgetTree->RootWidget = RootCanvas;

	UVerticalBox* RootStack = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ResultStack"));
	UCanvasPanelSlot* RootSlot = RootCanvas->AddChildToCanvas(RootStack);
	RootSlot->SetAnchors(FAnchors(0.5f, 0.5f));
	RootSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	RootSlot->SetSize(FVector2D(620.0f, 430.0f));

	UBorder* TitlePanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("TitlePanel"));
	TitlePanel->SetBrushColor(FLinearColor(0.11f, 0.21f, 0.38f, 0.92f));
	TitlePanel->SetPadding(FMargin(18.0f, 14.0f));
	UVerticalBoxSlot* TitlePanelSlot = RootStack->AddChildToVerticalBox(TitlePanel);
	TitlePanelSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 10.0f));

	TitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TitleText"));
	TitleText->SetText(FText::FromString(TEXT("TRAINING COMPLETE")));
	TitleText->SetJustification(ETextJustify::Center);
	TitleText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	TitleText->SetShadowOffset(FVector2D(1.0f, 1.0f));
	TitlePanel->SetContent(TitleText);

	UHorizontalBox* BodyRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("BodyRow"));
	UVerticalBoxSlot* BodySlot = RootStack->AddChildToVerticalBox(BodyRow);
	BodySlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	BodySlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));

	UBorder* SummaryPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("SummaryPanel"));
	SummaryPanel->SetBrushColor(FLinearColor(0.07f, 0.10f, 0.13f, 0.78f));
	SummaryPanel->SetPadding(FMargin(20.0f, 16.0f));
	UHorizontalBoxSlot* SummarySlot = BodyRow->AddChildToHorizontalBox(SummaryPanel);
	SummarySlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	SummarySlot->SetPadding(FMargin(0.0f, 0.0f, 10.0f, 0.0f));

	UVerticalBox* SummaryStack = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("SummaryStack"));
	SummaryPanel->SetContent(SummaryStack);
	DistanceText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DistanceText"));
	XPGainedText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("XPGainedText"));
	CoinsCollectedText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CoinsCollectedText"));
	TotalCoinsText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TotalCoinsText"));

	for (UTextBlock* Text : {DistanceText.Get(), XPGainedText.Get(), CoinsCollectedText.Get(), TotalCoinsText.Get()})
	{
		Text->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		Text->SetShadowOffset(FVector2D(1.0f, 1.0f));
		UVerticalBoxSlot* TextSlot = SummaryStack->AddChildToVerticalBox(Text);
		TextSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 14.0f));
	}

	UBorder* StatPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("StatPanel"));
	StatPanel->SetBrushColor(FLinearColor(0.07f, 0.10f, 0.13f, 0.78f));
	StatPanel->SetPadding(FMargin(20.0f, 16.0f));
	UHorizontalBoxSlot* StatSlot = BodyRow->AddChildToHorizontalBox(StatPanel);
	StatSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

	UVerticalBox* StatStack = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("StatStack"));
	StatPanel->SetContent(StatStack);

	StatLevelText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("StatLevelText"));
	StatLevelText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	StatLevelText->SetJustification(ETextJustify::Center);
	StatStack->AddChildToVerticalBox(StatLevelText);

	StatLevelBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("StatLevelBar"));
	StatLevelBar->SetFillColorAndOpacity(FLinearColor(0.92f, 0.87f, 0.03f, 1.0f));
	USizeBox* LevelBarBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("StatLevelBarBox"));
	LevelBarBox->SetHeightOverride(16.0f);
	LevelBarBox->SetContent(StatLevelBar);
	UVerticalBoxSlot* LevelBarSlot = StatStack->AddChildToVerticalBox(LevelBarBox);
	LevelBarSlot->SetPadding(FMargin(0.0f, 14.0f, 0.0f, 12.0f));

	StatProgressText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("StatProgressText"));
	StatProgressText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	StatProgressText->SetJustification(ETextJustify::Center);
	StatStack->AddChildToVerticalBox(StatProgressText);

	StatProgressBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("StatProgressBar"));
	StatProgressBar->SetFillColorAndOpacity(FLinearColor(0.92f, 0.87f, 0.03f, 1.0f));
	USizeBox* ProgressBarBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("StatProgressBarBox"));
	ProgressBarBox->SetHeightOverride(16.0f);
	ProgressBarBox->SetContent(StatProgressBar);
	UVerticalBoxSlot* ProgressBarSlot = StatStack->AddChildToVerticalBox(ProgressBarBox);
	ProgressBarSlot->SetPadding(FMargin(0.0f, 10.0f, 0.0f, 0.0f));

	UHorizontalBox* ButtonRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("ButtonRow"));
	RootStack->AddChildToVerticalBox(ButtonRow);

	RetryButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("RetryButton"));
	MenuButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("MenuButton"));

	UTextBlock* RetryText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("RetryButtonText"));
	RetryText->SetText(FText::FromString(TEXT("RETRY")));
	RetryText->SetJustification(ETextJustify::Center);
	RetryButton->AddChild(RetryText);

	UTextBlock* MenuText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("MenuButtonText"));
	MenuText->SetText(FText::FromString(TEXT("MENU")));
	MenuText->SetJustification(ETextJustify::Center);
	MenuButton->AddChild(MenuText);

	UHorizontalBoxSlot* RetrySlot = ButtonRow->AddChildToHorizontalBox(RetryButton);
	RetrySlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	RetrySlot->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));

	UHorizontalBoxSlot* MenuSlot = ButtonRow->AddChildToHorizontalBox(MenuButton);
	MenuSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	MenuSlot->SetPadding(FMargin(8.0f, 0.0f, 0.0f, 0.0f));
}

void USquirrelMiniGameResultWidget::BindButtons()
{
	if (RetryButton)
	{
		RetryButton->OnClicked.AddUniqueDynamic(this, &USquirrelMiniGameResultWidget::HandleRetryClicked);
	}

	if (MenuButton)
	{
		MenuButton->OnClicked.AddUniqueDynamic(this, &USquirrelMiniGameResultWidget::HandleMenuClicked);
	}
}

void USquirrelMiniGameResultWidget::RefreshResult()
{
	if (DistanceText)
	{
		DistanceText->SetText(FText::Format(FText::FromString(TEXT("Distance: {0}")), FText::AsNumber(FMath::FloorToInt(Result.DisplayDistance))));
	}

	if (XPGainedText)
	{
		XPGainedText->SetText(FText::Format(FText::FromString(TEXT("XP Gained: {0}")), FText::AsNumber(Result.XPGained)));
	}

	if (CoinsCollectedText)
	{
		CoinsCollectedText->SetText(FText::Format(FText::FromString(TEXT("Coins Collected: {0}")), FText::AsNumber(Result.CoinsCollected)));
	}

	if (TotalCoinsText)
	{
		TotalCoinsText->SetText(FText::Format(FText::FromString(TEXT("Total Coins: {0}")), FText::AsNumber(Result.TotalCoins)));
	}

	if (StatLevelText)
	{
		StatLevelText->SetText(FText::Format(
			FText::FromString(TEXT("{0}: Lvl {1}")),
			GetStatDisplayName(),
			FText::AsNumber(Result.StatLevel)));
	}

	if (StatLevelBar)
	{
		const float Percent = Result.StatMaxLevel > 0 ? static_cast<float>(Result.StatLevel) / static_cast<float>(Result.StatMaxLevel) : 0.0f;
		StatLevelBar->SetPercent(FMath::Clamp(Percent, 0.0f, 1.0f));
	}

	if (StatProgressText)
	{
		StatProgressText->SetText(FText::Format(
			FText::FromString(TEXT("{0}: Lvl {1}")),
			GetStatDisplayName(),
			FText::AsNumber(Result.StatLevel)));
	}

	if (StatProgressBar)
	{
		const float Percent = Result.StatPointsPerLevel > 0 ? static_cast<float>(Result.StatProgressPoints) / static_cast<float>(Result.StatPointsPerLevel) : 0.0f;
		StatProgressBar->SetPercent(FMath::Clamp(Percent, 0.0f, 1.0f));
	}
}

FText USquirrelMiniGameResultWidget::GetStatDisplayName() const
{
	switch (Result.TrainedStat)
	{
	case ESquirrelProgressStat::Energy:
		return FText::FromString(TEXT("Energy"));
	case ESquirrelProgressStat::Running:
		return FText::FromString(TEXT("Run"));
	case ESquirrelProgressStat::Climbing:
		return FText::FromString(TEXT("Climb"));
	case ESquirrelProgressStat::Flying:
		return FText::FromString(TEXT("Fly"));
	case ESquirrelProgressStat::Swimming:
		return FText::FromString(TEXT("Swim"));
	default:
		return FText::FromString(TEXT("Stat"));
	}
}

void USquirrelMiniGameResultWidget::HandleRetryClicked()
{
	if (!MiniGameMode)
	{
		MiniGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ASquirrelMiniGameModeBase>() : nullptr;
	}

	if (MiniGameMode)
	{
		MiniGameMode->RetryMiniGame();
	}
}

void USquirrelMiniGameResultWidget::HandleMenuClicked()
{
	if (!MiniGameMode)
	{
		MiniGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ASquirrelMiniGameModeBase>() : nullptr;
	}

	if (MiniGameMode)
	{
		MiniGameMode->ReturnToParkLevel();
	}
}
