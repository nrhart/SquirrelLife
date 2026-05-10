// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Progress/SquirrelProgressTypes.h"
#include "SquirrelTrainingHudWidget.generated.h"

class UProgressBar;
class USquirrelGameInstance;
class UButton;
class UTextBlock;
class UWidget;
class ASquirrelTrainingPawn;
class ASquirrelTrainingPlayerController;

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

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Training|HUD")
	TObjectPtr<UTextBlock> RunText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Training|HUD")
	TObjectPtr<UProgressBar> RunProgressBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Training|HUD")
	TObjectPtr<UProgressBar> RunOverallBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Training|Mini Games")
	TObjectPtr<UButton> MiniGameMenuButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Training|Mini Games")
	TObjectPtr<UWidget> MiniGameMenuPanel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Training|Mini Games")
	TObjectPtr<UButton> RunMiniGameButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Training|Mini Games")
	TObjectPtr<UButton> CloseMiniGameMenuButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Mini Games")
	FName RunningMiniGameLevelName = TEXT("RunningMiniGame");

	UPROPERTY(Transient)
	TObjectPtr<ASquirrelTrainingPlayerController> BoundTrainingController;

	UPROPERTY(Transient)
	TObjectPtr<ASquirrelTrainingPawn> BoundSquirrel;

	UPROPERTY(Transient)
	TObjectPtr<USquirrelGameInstance> BoundProgressGameInstance;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "Training|HUD")
	void Refresh();

	UFUNCTION()
	void HandleMoneyChanged(int32 NewMoney);

	UFUNCTION()
	void HandleEnergyChanged(int32 NewEnergyLevel, int32 NewEnergyProgressPoints, int32 NewMaxEnergyLevel);

	UFUNCTION()
	void HandleCoinsChanged(int32 NewCoins);

	UFUNCTION()
	void HandleStatChanged(ESquirrelProgressStat Stat, int32 NewLevel, int32 NewProgressPoints, int32 NewMaxLevel);

	UFUNCTION()
	void HandleMiniGameMenuClicked();

	UFUNCTION()
	void HandleCloseMiniGameMenuClicked();

	UFUNCTION()
	void HandleRunMiniGameClicked();

	void BuildHud();
	void EnsureWidgetTree();
	void BindEventSources();
	void UnbindEventSources();
	void BindMiniGameMenu();
	void UnbindMiniGameMenu();
	void SetMiniGameMenuVisible(bool bVisible);
	void UpdateCoinsDisplay(int32 NewCoins);
	void UpdateEnergyDisplay(int32 NewEnergyLevel, int32 NewEnergyProgressPoints, int32 NewMaxEnergyLevel, int32 PointsPerLevel);
	void UpdateRunDisplay(int32 NewRunLevel, int32 NewRunProgressPoints, int32 NewRunMaxLevel, int32 PointsPerLevel);
	void RefreshRunFromProgress();

public:
	UFUNCTION(BlueprintCallable, Category = "Training|Mini Games")
	void OpenRunningMiniGame();
};
