// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SquirrelFoodDispenserWidget.generated.h"

class ASquirrelFoodDispenserActor;
class UButton;
class UTextBlock;

/**
 * Simple runtime-created button widget for a food dispenser.
 */
UCLASS()
class SQUIRRELLIFE_API USquirrelFoodDispenserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetDispenser(ASquirrelFoodDispenserActor* NewDispenser);
	void Refresh();

protected:
	UPROPERTY()
	TObjectPtr<ASquirrelFoodDispenserActor> Dispenser;

	UPROPERTY()
	TObjectPtr<UButton> DispenseButton;

	UPROPERTY()
	TObjectPtr<UTextBlock> ButtonText;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnDispenseClicked();
};
