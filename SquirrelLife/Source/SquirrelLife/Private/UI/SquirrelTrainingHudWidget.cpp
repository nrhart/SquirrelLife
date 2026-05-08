// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/SquirrelTrainingHudWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/Engine.h"
#include "Training/SquirrelTrainingPawn.h"
#include "Training/SquirrelTrainingPlayerController.h"

void USquirrelTrainingHudWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BuildHud();
	Refresh();
}

void USquirrelTrainingHudWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	Refresh();
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
	const float EnergyProgressPercent = Squirrel ? Squirrel->GetEnergyProgressPercent() : 0.0f;
	const float EnergyLevelPercent = Squirrel ? Squirrel->GetEnergyLevelPercent() : 0.0f;
	const int32 Coins = TrainingController ? TrainingController->GetMoney() : 0;

	if (EnergyText)
	{
		EnergyText->SetText(FText::Format(FText::FromString(TEXT("Energy: Lvl {0}")), FText::AsNumber(EnergyLevel)));
	}

	if (EnergyValueText)
	{
		EnergyValueText->SetText(FText::AsNumber(EnergyLevel));
	}

	if (MaxEnergyValueText)
	{
		MaxEnergyValueText->SetText(FText::AsNumber(MaxEnergyLevel));
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
			FText::AsNumber(EnergyProgressPoints),
			FText::AsNumber(EnergyPointsPerLevel)));
	}

	if (CoinsText)
	{
		CoinsText->SetText(FText::Format(FText::FromString(TEXT("Coins: {0}")), FText::AsNumber(Coins)));
	}

	if (CoinsValueText)
	{
		CoinsValueText->SetText(FText::AsNumber(Coins));
	}
}

void USquirrelTrainingHudWidget::EnsureWidgetTree()
{
	if (!WidgetTree)
	{
		WidgetTree = NewObject<UWidgetTree>(this, TEXT("WidgetTree"));
	}
}
