// Copyright Epic Games, Inc. All Rights Reserved.

#include "Training/SquirrelTrainingPlayerController.h"

#include "EngineUtils.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "InputCoreTypes.h"
#include "GameFramework/Pawn.h"
#include "Progress/SquirrelGameInstance.h"
#include "Progress/SquirrelProgressTuningData.h"
#include "Progress/SquirrelTuningComponent.h"
#include "Training/SquirrelFoodDispenserActor.h"
#include "Training/SquirrelTrainingCameraActor.h"
#include "Training/SquirrelTrainingGameMode.h"
#include "Training/SquirrelTrainingPawn.h"
#include "UI/SquirrelTrainingHudWidget.h"

ASquirrelTrainingPlayerController::ASquirrelTrainingPlayerController()
{
	bAutoManageActiveCameraTarget = false;
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	bEnableTouchEvents = true;
	DefaultMouseCursor = EMouseCursor::GrabHand;
	TrainingHudWidgetClass = USquirrelTrainingHudWidget::StaticClass();
}

void ASquirrelTrainingPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
	if (USquirrelGameInstance* SquirrelGameInstance = GetProgressGameInstance())
	{
		Money = SquirrelGameInstance->GetCoins();
	}
	else
	{
		const USquirrelProgressTuningData* ResolvedTuning = GetResolvedProgressTuning();
		Money = ResolvedTuning ? FMath::Max(ResolvedTuning->StartingCoins, 0) : StartingMoney;
	}
	OnMoneyChanged.Broadcast(Money);
	SetupTrainingCamera();
	SetupTrainingHud();
}

void ASquirrelTrainingPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &ASquirrelTrainingPlayerController::OnPrimaryPressed);
	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &ASquirrelTrainingPlayerController::OnPrimaryReleased);
	if (bEnableDebugResetProgressKey && DebugResetProgressKey.IsValid())
	{
		InputComponent->BindKey(DebugResetProgressKey, IE_Pressed, this, &ASquirrelTrainingPlayerController::HandleDebugResetProgress);
	}
	InputComponent->BindTouch(IE_Pressed, this, &ASquirrelTrainingPlayerController::OnTouchPressed);
	InputComponent->BindTouch(IE_Released, this, &ASquirrelTrainingPlayerController::OnTouchReleased);
}

void ASquirrelTrainingPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	SetupTrainingCamera();
}

void ASquirrelTrainingPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	UpdatePolledMouseDrag();
	UpdateEdgeScrollCamera(DeltaTime);
	UpdateDraggedSquirrel();

	if (bShowTrainingHud && !TrainingHudWidget)
	{
		SetupTrainingHud();
	}
}

void ASquirrelTrainingPlayerController::OnPrimaryPressed()
{
	bWasPrimaryMouseDown = true;

	float ScreenX = 0.0f;
	float ScreenY = 0.0f;
	if (GetMousePosition(ScreenX, ScreenY))
	{
		bDraggingWithTouch = false;
		TryStartDragAtScreenPosition(ScreenX, ScreenY);
	}
}

void ASquirrelTrainingPlayerController::OnPrimaryReleased()
{
	FinishDrag();
	bWasPrimaryMouseDown = false;
}

void ASquirrelTrainingPlayerController::OnTouchPressed(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (DraggedSquirrel)
	{
		return;
	}

	bDraggingWithTouch = true;
	ActiveTouchIndex = FingerIndex;
	TryStartDragAtScreenPosition(Location.X, Location.Y);
}

void ASquirrelTrainingPlayerController::OnTouchReleased(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (bDraggingWithTouch && FingerIndex == ActiveTouchIndex)
	{
		FinishDrag();
	}
}

bool ASquirrelTrainingPlayerController::TryStartDragAtScreenPosition(float ScreenX, float ScreenY)
{
	FVector WorldOrigin = FVector::ZeroVector;
	FVector WorldDirection = FVector::ForwardVector;
	if (!DeprojectScreenPositionToWorld(ScreenX, ScreenY, WorldOrigin, WorldDirection))
	{
		return false;
	}

	FHitResult Hit;
	const FVector TraceEnd = WorldOrigin + WorldDirection * 10000.0f;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SquirrelDragTrace), false);

	ASquirrelTrainingPawn* Squirrel = nullptr;
	if (GetWorld()->LineTraceSingleByChannel(Hit, WorldOrigin, TraceEnd, DragTraceChannel, QueryParams))
	{
		if (ASquirrelFoodDispenserActor* Dispenser = Cast<ASquirrelFoodDispenserActor>(Hit.GetActor()))
		{
			Dispenser->TryDispenseFood();
			return false;
		}

		Squirrel = Cast<ASquirrelTrainingPawn>(Hit.GetActor());
	}

	if (!Squirrel || Squirrel->IsEating())
	{
		return false;
	}

	DraggedSquirrel = Squirrel;
	StartScreenDrag(*DraggedSquirrel, ScreenX, ScreenY);
	UpdateDraggedSquirrel();
	DefaultMouseCursor = EMouseCursor::GrabHandClosed;
	return true;
}

void ASquirrelTrainingPlayerController::StartScreenDrag(ASquirrelTrainingPawn& Squirrel, float ScreenX, float ScreenY)
{
	DragStartWorldLocation = Squirrel.GetActorLocation();
	if (!ProjectWorldLocationToScreen(DragStartWorldLocation, DragStartScreenPosition, false))
	{
		DragStartScreenPosition = FVector2D(ScreenX, ScreenY);
	}

	DragWorldUnitsPerScreenPixel = CalculateDragWorldUnitsPerScreenPixel(DragStartWorldLocation);
	Squirrel.BeginDrag();
}

float ASquirrelTrainingPlayerController::CalculateDragWorldUnitsPerScreenPixel(const FVector& DraggedWorldLocation) const
{
	if (!PlayerCameraManager)
	{
		return 1.0f;
	}

	int32 ViewportSizeX = 0;
	int32 ViewportSizeY = 0;
	GetViewportSize(ViewportSizeX, ViewportSizeY);
	if (ViewportSizeY <= 0)
	{
		return 1.0f;
	}

	const FVector CameraLocation = PlayerCameraManager->GetCameraLocation();
	const FVector CameraForward = PlayerCameraManager->GetCameraRotation().Vector().GetSafeNormal();
	const float Depth = FMath::Max(FVector::DotProduct(DraggedWorldLocation - CameraLocation, CameraForward), 1.0f);
	const float HalfVerticalFovRadians = FMath::DegreesToRadians(PlayerCameraManager->GetFOVAngle() * 0.5f);
	return (2.0f * Depth * FMath::Tan(HalfVerticalFovRadians)) / static_cast<float>(ViewportSizeY);
}

bool ASquirrelTrainingPlayerController::GetDragWorldLocation(float ScreenX, float ScreenY, FVector& OutWorldLocation) const
{
	const ASquirrelTrainingPawn* Squirrel = DraggedSquirrel.Get();
	if (!Squirrel)
	{
		return false;
	}

	FVector WorldOrigin = FVector::ZeroVector;
	FVector WorldDirection = FVector::ForwardVector;
	if (DeprojectScreenPositionToWorld(ScreenX, ScreenY, WorldOrigin, WorldDirection))
	{
		const FPlane TrainingPlane(FVector(0.0f, Squirrel->GetTrainingPlaneY(), 0.0f), FVector::YAxisVector);
		OutWorldLocation = FMath::LinePlaneIntersection(WorldOrigin, WorldOrigin + WorldDirection * 100000.0f, TrainingPlane);
		OutWorldLocation.Y = Squirrel->GetTrainingPlaneY();
		return true;
	}

	const FVector2D ScreenDelta = FVector2D(ScreenX, ScreenY) - DragStartScreenPosition;
	OutWorldLocation = DragStartWorldLocation;
	OutWorldLocation.X += ScreenDelta.X * DragWorldUnitsPerScreenPixel;
	OutWorldLocation.Z -= ScreenDelta.Y * DragWorldUnitsPerScreenPixel;
	OutWorldLocation.Y = Squirrel->GetTrainingPlaneY();
	return true;
}

void ASquirrelTrainingPlayerController::SetupTrainingCamera()
{
	UWorld* World = GetWorld();
	ASquirrelTrainingPawn* Squirrel = Cast<ASquirrelTrainingPawn>(GetPawn());
	if (!World || !Squirrel)
	{
		return;
	}

	if (!TrainingCamera)
	{
		for (TActorIterator<ASquirrelTrainingCameraActor> It(World); It; ++It)
		{
			TrainingCamera = *It;
			break;
		}
	}

	if (!TrainingCamera)
	{
		const FVector CameraSpawnLocation = Squirrel->GetActorLocation() + FVector(0.0f, -1200.0f, 260.0f);
		const FRotator CameraSpawnRotation = FRotator(0.0f, 90.0f, 0.0f);
		TrainingCamera = World->SpawnActor<ASquirrelTrainingCameraActor>(ASquirrelTrainingCameraActor::StaticClass(), CameraSpawnLocation, CameraSpawnRotation);
	}

	if (TrainingCamera)
	{
		SetViewTarget(TrainingCamera);
	}
}

void ASquirrelTrainingPlayerController::SetupTrainingHud()
{
	if (!bShowTrainingHud || !IsLocalController() || TrainingHudWidget)
	{
		return;
	}

	if (!TrainingHudWidgetClass)
	{
		if (bDebugTrainingHud && !bReportedTrainingHudMissingClass && GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, TEXT("Training HUD class is not set on the player controller."));
		}

		bReportedTrainingHudMissingClass = true;
		return;
	}

	TrainingHudWidget = CreateWidget<USquirrelTrainingHudWidget>(this, TrainingHudWidgetClass);
	if (TrainingHudWidget)
	{
		TrainingHudWidget->SetVisibility(ESlateVisibility::Visible);
		TrainingHudWidget->AddToPlayerScreen(10);

		if (bDebugTrainingHud && !bReportedTrainingHudCreated && GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Training HUD created."));
		}

		bReportedTrainingHudCreated = true;
	}
}

USquirrelGameInstance* ASquirrelTrainingPlayerController::GetProgressGameInstance() const
{
	return GetGameInstance<USquirrelGameInstance>();
}

USquirrelProgressTuningData* ASquirrelTrainingPlayerController::GetResolvedProgressTuning() const
{
	if (ProgressTuning)
	{
		return ProgressTuning;
	}

	if (const APawn* CurrentPawn = GetPawn())
	{
		if (const USquirrelTuningComponent* PawnTuningComponent = CurrentPawn->FindComponentByClass<USquirrelTuningComponent>())
		{
			if (USquirrelProgressTuningData* PawnTuning = PawnTuningComponent->GetAssignedProgressTuning())
			{
				return PawnTuning;
			}
		}
	}

	if (const USquirrelGameInstance* SquirrelGameInstance = GetProgressGameInstance())
	{
		if (USquirrelProgressTuningData* GameInstanceTuning = SquirrelGameInstance->GetProgressTuning())
		{
			return GameInstanceTuning;
		}
	}

	if (const ASquirrelTrainingGameMode* TrainingGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ASquirrelTrainingGameMode>() : nullptr)
	{
		return TrainingGameMode->GetProgressTuning();
	}

	return nullptr;
}

void ASquirrelTrainingPlayerController::UpdatePolledMouseDrag()
{
	const bool bPrimaryMouseDown = IsInputKeyDown(EKeys::LeftMouseButton);

	if (bPrimaryMouseDown && !bWasPrimaryMouseDown && !DraggedSquirrel)
	{
		float ScreenX = 0.0f;
		float ScreenY = 0.0f;
		if (GetMousePosition(ScreenX, ScreenY))
		{
			bDraggingWithTouch = false;
			TryStartDragAtScreenPosition(ScreenX, ScreenY);
		}
	}
	else if (!bPrimaryMouseDown && bWasPrimaryMouseDown && !bDraggingWithTouch)
	{
		FinishDrag();
	}

	bWasPrimaryMouseDown = bPrimaryMouseDown;
}

void ASquirrelTrainingPlayerController::UpdateEdgeScrollCamera(float DeltaSeconds)
{
	if (!TrainingCamera)
	{
		SetupTrainingCamera();
	}

	if (!TrainingCamera)
	{
		return;
	}

	float MouseX = 0.0f;
	float MouseY = 0.0f;
	int32 ViewportSizeX = 0;
	int32 ViewportSizeY = 0;
	GetViewportSize(ViewportSizeX, ViewportSizeY);

	if (ViewportSizeX <= 0 || !GetMousePosition(MouseX, MouseY))
	{
		return;
	}

	float ScrollDirection = 0.0f;

	if (MouseX <= EdgeScrollMargin)
	{
		ScrollDirection = -1.0f;
	}
	else if (MouseX >= ViewportSizeX - EdgeScrollMargin)
	{
		ScrollDirection = 1.0f;
	}

	TrainingCamera->ScrollHorizontal(ScrollDirection, DeltaSeconds);
}

void ASquirrelTrainingPlayerController::UpdateDraggedSquirrel()
{
	if (!DraggedSquirrel)
	{
		return;
	}

	float ScreenX = 0.0f;
	float ScreenY = 0.0f;

	if (bDraggingWithTouch)
	{
		bool bIsPressed = false;
		GetInputTouchState(ActiveTouchIndex, ScreenX, ScreenY, bIsPressed);
		if (!bIsPressed)
		{
			FinishDrag();
			return;
		}
	}
	else if (!GetMousePosition(ScreenX, ScreenY))
	{
		return;
	}

	FVector DragWorldLocation = FVector::ZeroVector;
	if (GetDragWorldLocation(ScreenX, ScreenY, DragWorldLocation))
	{
		DraggedSquirrel->DragToWorldLocation(DragWorldLocation);
	}
}

void ASquirrelTrainingPlayerController::FinishDrag()
{
	if (DraggedSquirrel)
	{
		DraggedSquirrel->EndDrag();
		DraggedSquirrel = nullptr;
	}

	DefaultMouseCursor = EMouseCursor::GrabHand;
}

void ASquirrelTrainingPlayerController::HandleDebugResetProgress()
{
	ResetAllProgressForDebug();
}

void ASquirrelTrainingPlayerController::AddMoney(int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	if (USquirrelGameInstance* SquirrelGameInstance = GetProgressGameInstance())
	{
		SquirrelGameInstance->AddCoins(Amount);
		Money = SquirrelGameInstance->GetCoins();
	}
	else
	{
		Money += Amount;
	}
	OnMoneyChanged.Broadcast(Money);
}

bool ASquirrelTrainingPlayerController::TrySpendMoney(int32 Amount)
{
	if (Amount <= 0)
	{
		return true;
	}

	if (USquirrelGameInstance* SquirrelGameInstance = GetProgressGameInstance())
	{
		if (!SquirrelGameInstance->TrySpendCoins(Amount))
		{
			return false;
		}

		Money = SquirrelGameInstance->GetCoins();
		OnMoneyChanged.Broadcast(Money);
		return true;
	}

	if (Money < Amount)
	{
		return false;
	}

	Money -= Amount;
	OnMoneyChanged.Broadcast(Money);
	return true;
}

void ASquirrelTrainingPlayerController::ResetAllProgressForDebug()
{
	if (USquirrelGameInstance* SquirrelGameInstance = GetProgressGameInstance())
	{
		const bool bReset = SquirrelGameInstance->ResetSavedProgress();
		Money = SquirrelGameInstance->GetCoins();
		OnMoneyChanged.Broadcast(Money);

		if (GEngine)
		{
			const FColor MessageColor = bReset ? FColor::Green : FColor::Yellow;
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, MessageColor, bReset
				? TEXT("Debug: reset squirrel progress.")
				: TEXT("Debug: reset squirrel progress, but save delete reported false."));
		}
		return;
	}

	const USquirrelProgressTuningData* ResolvedTuning = GetResolvedProgressTuning();
	Money = ResolvedTuning ? FMath::Max(ResolvedTuning->StartingCoins, 0) : StartingMoney;
	OnMoneyChanged.Broadcast(Money);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("Debug: reset local money only. SquirrelGameInstance is not active."));
	}
}
