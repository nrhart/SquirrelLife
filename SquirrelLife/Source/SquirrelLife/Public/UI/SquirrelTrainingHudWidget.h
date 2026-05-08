// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SquirrelTrainingHudWidget.generated.h"

class UProgressBar;
class UTextBlock;

/**
 * Training HUD data bridge. Widget Blueprints can own the layout and expose
 * named text/progress widgets for this class to update.
 */
UCLASS(Blueprintable)
class SQUIRRELLIFE_API USquirrelTrainingHudWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Training|HUD")
	TObjectPtr<UTextBlock> EnergyText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Training|HUD")
	TObjectPtr<UTextBlock> EnergyValueText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Training|HUD")
	TObjectPtr<UTextBlock> MaxEnergyValueText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Training|HUD")
	TObjectPtr<UProgressBar> EnergyBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Training|HUD")
	TObjectPtr<UProgressBar> EnergyProgressBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Training|HUD")
	TObjectPtr<UProgressBar> EnergyOverallBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Training|HUD")
	TObjectPtr<UTextBlock> EnergyProgressText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Training|HUD")
	TObjectPtr<UTextBlock> CoinsText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Training|HUD")
	TObjectPtr<UTextBlock> CoinsValueText;

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Training|HUD")
	void Refresh();

	void BuildHud();
	void EnsureWidgetTree();
};
