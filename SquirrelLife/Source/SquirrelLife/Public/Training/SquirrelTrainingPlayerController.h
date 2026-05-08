// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SquirrelTrainingPlayerController.generated.h"

class ASquirrelTrainingPawn;
class ASquirrelTrainingCameraActor;
class USquirrelTrainingHudWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSquirrelMoneyChangedSignature, int32, NewMoney);

/**
 * Handles mouse/touch dragging for the training pawn without requiring input assets.
 */
UCLASS(Blueprintable)
class SQUIRRELLIFE_API ASquirrelTrainingPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASquirrelTrainingPlayerController();

	virtual void PlayerTick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Drag")
	TEnumAsByte<ECollisionChannel> DragTraceChannel = ECC_Visibility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Camera", meta = (ClampMin = "0"))
	float EdgeScrollMargin = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|Economy", meta = (ClampMin = "0"))
	int32 StartingMoney = 25;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Training|Economy")
	int32 Money = 0;

	UPROPERTY(BlueprintAssignable, Category = "Training|Economy")
	FSquirrelMoneyChangedSignature OnMoneyChanged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|UI")
	bool bShowTrainingHud = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|UI")
	bool bDebugTrainingHud = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training|UI")
	TSubclassOf<USquirrelTrainingHudWidget> TrainingHudWidgetClass;

	UPROPERTY()
	TObjectPtr<USquirrelTrainingHudWidget> TrainingHudWidget;

	UPROPERTY()
	TObjectPtr<ASquirrelTrainingPawn> DraggedSquirrel;

	UPROPERTY()
	TObjectPtr<ASquirrelTrainingCameraActor> TrainingCamera;

	bool bDraggingWithTouch = false;
	bool bWasPrimaryMouseDown = false;
	ETouchIndex::Type ActiveTouchIndex = ETouchIndex::Touch1;
	FVector2D DragStartScreenPosition = FVector2D::ZeroVector;
	FVector DragStartWorldLocation = FVector::ZeroVector;
	float DragWorldUnitsPerScreenPixel = 1.0f;
	bool bReportedTrainingHudMissingClass = false;
	bool bReportedTrainingHudCreated = false;

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

	void OnPrimaryPressed();
	void OnPrimaryReleased();
	void OnTouchPressed(const ETouchIndex::Type FingerIndex, const FVector Location);
	void OnTouchReleased(const ETouchIndex::Type FingerIndex, const FVector Location);

	bool TryStartDragAtScreenPosition(float ScreenX, float ScreenY);
	void StartScreenDrag(ASquirrelTrainingPawn& Squirrel, float ScreenX, float ScreenY);
	float CalculateDragWorldUnitsPerScreenPixel(const FVector& DraggedWorldLocation) const;
	bool GetDragWorldLocation(float ScreenX, float ScreenY, FVector& OutWorldLocation) const;
	void SetupTrainingCamera();
	void SetupTrainingHud();
	void UpdatePolledMouseDrag();
	void UpdateEdgeScrollCamera(float DeltaSeconds);
	void UpdateDraggedSquirrel();
	void FinishDrag();

public:
	UFUNCTION(BlueprintPure, Category = "Training|Economy")
	int32 GetMoney() const { return Money; }

	UFUNCTION(BlueprintCallable, Category = "Training|Economy")
	void AddMoney(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Training|Economy")
	bool TrySpendMoney(int32 Amount);
};
