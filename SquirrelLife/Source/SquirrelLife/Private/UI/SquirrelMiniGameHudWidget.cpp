// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/SquirrelMiniGameHudWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Kismet/GameplayStatics.h"
#include "MiniGames/SquirrelMiniGameModeBase.h"

namespace
{
	constexpr float CentimetersPerSecondPerMilePerHour = 44.704f;
}

void USquirrelMiniGameHudWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BuildHud();
	BindMiniGameMode();

	if (BoundMiniGameMode)
	{
		UpdateLiveStats(BoundMiniGameMode->GetDisplayDistance(), BoundMiniGameMode->GetCurrentRunSpeed());
	}
}

void USquirrelMiniGameHudWidget::NativeDestruct()
{
	UnbindMiniGameMode();

	Super::NativeDestruct();
}

void USquirrelMiniGameHudWidget::SetMiniGameMode(ASquirrelMiniGameModeBase* NewMiniGameMode)
{
	if (BoundMiniGameMode == NewMiniGameMode)
	{
		return;
	}

	UnbindMiniGameMode();
	BoundMiniGameMode = NewMiniGameMode;
	BindMiniGameMode();

	if (BoundMiniGameMode)
	{
		UpdateLiveStats(BoundMiniGameMode->GetDisplayDistance(), BoundMiniGameMode->GetCurrentRunSpeed());
	}
}

void USquirrelMiniGameHudWidget::BuildHud()
{
	if (DistanceText || SpeedText || !WidgetTree || WidgetTree->RootWidget)
	{
		return;
	}

	UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("MiniGameHudRoot"));
	WidgetTree->RootWidget = RootCanvas;

	UBorder* Panel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("MiniGameHudPanel"));
	Panel->SetBrushColor(FLinearColor(0.06f, 0.09f, 0.12f, 0.78f));
	Panel->SetPadding(FMargin(14.0f, 10.0f));

	UCanvasPanelSlot* PanelSlot = RootCanvas->AddChildToCanvas(Panel);
	PanelSlot->SetAnchors(FAnchors(0.0f, 0.0f));
	PanelSlot->SetAlignment(FVector2D(0.0f, 0.0f));
	PanelSlot->SetPosition(FVector2D(18.0f, 18.0f));
	PanelSlot->SetAutoSize(true);

	UVerticalBox* Stack = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("MiniGameHudStack"));
	Panel->SetContent(Stack);

	DistanceText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DistanceText"));
	DistanceText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	DistanceText->SetShadowOffset(FVector2D(1.0f, 1.0f));
	Stack->AddChildToVerticalBox(DistanceText);

	SpeedText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SpeedText"));
	SpeedText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	SpeedText->SetShadowOffset(FVector2D(1.0f, 1.0f));
	UVerticalBoxSlot* SpeedSlot = Stack->AddChildToVerticalBox(SpeedText);
	SpeedSlot->SetPadding(FMargin(0.0f, 5.0f, 0.0f, 0.0f));
}

void USquirrelMiniGameHudWidget::BindMiniGameMode()
{
	if (!BoundMiniGameMode)
	{
		BoundMiniGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ASquirrelMiniGameModeBase>() : nullptr;
	}

	if (BoundMiniGameMode)
	{
		BoundMiniGameMode->OnLiveStatsChanged.AddUniqueDynamic(this, &USquirrelMiniGameHudWidget::HandleLiveStatsChanged);
	}
}

void USquirrelMiniGameHudWidget::UnbindMiniGameMode()
{
	if (BoundMiniGameMode)
	{
		BoundMiniGameMode->OnLiveStatsChanged.RemoveDynamic(this, &USquirrelMiniGameHudWidget::HandleLiveStatsChanged);
	}
}

void USquirrelMiniGameHudWidget::HandleLiveStatsChanged(float Distance, float Speed)
{
	UpdateLiveStats(Distance, Speed);
}

void USquirrelMiniGameHudWidget::UpdateLiveStats(float Distance, float Speed)
{
	if (DistanceText)
	{
		DistanceText->SetText(FText::Format(
			FText::FromString(TEXT("Distance: {0}")),
			FText::AsNumber(FMath::FloorToInt(Distance))));
	}

	if (SpeedText)
	{
		const int32 SpeedMilesPerHour = FMath::RoundToInt(Speed / CentimetersPerSecondPerMilePerHour);
		SpeedText->SetText(FText::Format(
			FText::FromString(TEXT("Speed: {0} mph")),
			FText::AsNumber(SpeedMilesPerHour)));
	}
}
