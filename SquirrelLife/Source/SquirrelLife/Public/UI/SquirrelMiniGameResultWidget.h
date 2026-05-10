// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MiniGames/SquirrelMiniGameModeBase.h"
#include "SquirrelMiniGameResultWidget.generated.h"

class ASquirrelMiniGameModeBase;
class UButton;
class UProgressBar;
class UTextBlock;

UCLASS(Blueprintable)
class SQUIRRELLIFE_API USquirrelMiniGameResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Mini Game|Result")
	void SetMiniGameMode(ASquirrelMiniGameModeBase* NewMiniGameMode);

	UFUNCTION(BlueprintCallable, Category = "Mini Game|Result")
	void SetResult(const FSquirrelMiniGameResult& NewResult);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Mini Game|Result")
	TObjectPtr<UTextBlock> TitleText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Mini Game|Result")
	TObjectPtr<UTextBlock> DistanceText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Mini Game|Result")
	TObjectPtr<UTextBlock> XPGainedText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Mini Game|Result")
	TObjectPtr<UTextBlock> CoinsCollectedText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Mini Game|Result")
	TObjectPtr<UTextBlock> TotalCoinsText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Mini Game|Result")
	TObjectPtr<UTextBlock> StatLevelText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Mini Game|Result")
	TObjectPtr<UTextBlock> StatProgressText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Mini Game|Result")
	TObjectPtr<UProgressBar> StatLevelBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Mini Game|Result")
	TObjectPtr<UProgressBar> StatProgressBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Mini Game|Result")
	TObjectPtr<UButton> RetryButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Mini Game|Result")
	TObjectPtr<UButton> MenuButton;

	UPROPERTY(Transient)
	TObjectPtr<ASquirrelMiniGameModeBase> MiniGameMode;

	FSquirrelMiniGameResult Result;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleRetryClicked();

	UFUNCTION()
	void HandleMenuClicked();

	void BuildResultPanel();
	void BindButtons();
	void RefreshResult();
	FText GetStatDisplayName() const;
};
