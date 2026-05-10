// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SquirrelMiniGameHudWidget.generated.h"

class ASquirrelMiniGameModeBase;
class UTextBlock;

UCLASS(Blueprintable)
class SQUIRRELLIFE_API USquirrelMiniGameHudWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Mini Game|HUD")
	void SetMiniGameMode(ASquirrelMiniGameModeBase* NewMiniGameMode);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Mini Game|HUD")
	TObjectPtr<UTextBlock> DistanceText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional), Category = "Mini Game|HUD")
	TObjectPtr<UTextBlock> SpeedText;

	UPROPERTY(Transient)
	TObjectPtr<ASquirrelMiniGameModeBase> BoundMiniGameMode;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleLiveStatsChanged(float Distance, float Speed);

	void BuildHud();
	void BindMiniGameMode();
	void UnbindMiniGameMode();
	void UpdateLiveStats(float Distance, float Speed);
};
