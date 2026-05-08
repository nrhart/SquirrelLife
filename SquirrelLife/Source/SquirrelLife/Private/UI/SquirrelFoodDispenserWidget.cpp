// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/SquirrelFoodDispenserWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Training/SquirrelFoodDispenserActor.h"

void USquirrelFoodDispenserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!WidgetTree || DispenseButton)
	{
		Refresh();
		return;
	}

	UVerticalBox* RootBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("DispenserRoot"));
	WidgetTree->RootWidget = RootBox;

	DispenseButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("DispenseButton"));
	ButtonText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ButtonText"));

	ButtonText->SetJustification(ETextJustify::Center);
	DispenseButton->SetContent(ButtonText);
	RootBox->AddChildToVerticalBox(DispenseButton);

	DispenseButton->OnClicked.AddDynamic(this, &USquirrelFoodDispenserWidget::OnDispenseClicked);
	Refresh();
}

void USquirrelFoodDispenserWidget::SetDispenser(ASquirrelFoodDispenserActor* NewDispenser)
{
	Dispenser = NewDispenser;
	Refresh();
}

void USquirrelFoodDispenserWidget::Refresh()
{
	if (!ButtonText)
	{
		return;
	}

	if (!Dispenser)
	{
		ButtonText->SetText(FText::FromString(TEXT("Dispense")));
		return;
	}

	ButtonText->SetText(Dispenser->GetButtonText());
}

void USquirrelFoodDispenserWidget::OnDispenseClicked()
{
	if (Dispenser)
	{
		Dispenser->TryDispenseFood();
		Refresh();
	}
}
