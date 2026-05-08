// Copyright Epic Games, Inc. All Rights Reserved.

#include "Training/SquirrelFoodDispenserActor.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Training/SquirrelFoodActor.h"
#include "Training/SquirrelFoodDispenserWidget.h"
#include "Training/SquirrelTrainingPlayerController.h"

ASquirrelFoodDispenserActor::ASquirrelFoodDispenserActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	DispenserMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DispenserMesh"));
	DispenserMesh->SetupAttachment(RootComponent);

	FoodSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("FoodSpawnPoint"));
	FoodSpawnPoint->SetupAttachment(RootComponent);
	FoodSpawnPoint->SetRelativeLocation(FVector(95.0f, 0.0f, 40.0f));

	PurchaseWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PurchaseWidget"));
	PurchaseWidget->SetupAttachment(RootComponent);
	PurchaseWidget->SetWidgetSpace(EWidgetSpace::World);
	PurchaseWidget->SetWidgetClass(USquirrelFoodDispenserWidget::StaticClass());
	PurchaseWidget->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		DispenserMesh->SetStaticMesh(CubeMesh.Object);
	}

	DispenserMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DispenserMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	ApplyDispenserTuning();

	FoodClass = ASquirrelFoodActor::StaticClass();
}

void ASquirrelFoodDispenserActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ApplyDispenserTuning();
}

void ASquirrelFoodDispenserActor::BeginPlay()
{
	Super::BeginPlay();

	if (USquirrelFoodDispenserWidget* DispenserWidget = Cast<USquirrelFoodDispenserWidget>(PurchaseWidget->GetUserWidgetObject()))
	{
		DispenserWidget->SetDispenser(this);
	}
}

void ASquirrelFoodDispenserActor::ApplyDispenserTuning()
{
	if (DispenserMesh)
	{
		DispenserMesh->SetRelativeScale3D(DispenserMeshScale);
	}

	if (PurchaseWidget)
	{
		PurchaseWidget->SetRelativeLocation(PurchaseWidgetRelativeLocation);
		PurchaseWidget->SetRelativeRotation(PurchaseWidgetRelativeRotation);
		PurchaseWidget->SetDrawSize(PurchaseWidgetDrawSize);
	}
}

bool ASquirrelFoodDispenserActor::TryDispenseFood()
{
	ASquirrelTrainingPlayerController* TrainingController = GetTrainingPlayerController();
	if (!TrainingController || !FoodClass)
	{
		return false;
	}

	const UWorld* World = GetWorld();
	const float CurrentTimeSeconds = World ? World->GetTimeSeconds() : 0.0f;
	if (CurrentTimeSeconds - LastDispenseTimeSeconds < DuplicateClickGuardSeconds)
	{
		return false;
	}

	PruneInactiveFood();
	if (GetActiveFoodCount() >= MaxActiveFoodCount)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.25f, FColor::Yellow, TEXT("Eat the food first."));
		}
		return false;
	}

	if (!TrainingController->TrySpendMoney(FoodCost))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.25f, FColor::Red, TEXT("Not enough money for food."));
		}
		return false;
	}

	const FRotator FoodSpawnRotation = FRotator(
		FMath::FRandRange(-SpawnPitchRollRange, SpawnPitchRollRange),
		FMath::FRandRange(0.0f, 360.0f),
		FMath::FRandRange(-SpawnPitchRollRange, SpawnPitchRollRange));
	ASquirrelFoodActor* SpawnedFood = GetWorld()->SpawnActor<ASquirrelFoodActor>(FoodClass, GetNextFoodSpawnLocation(), FoodSpawnRotation);
	if (SpawnedFood)
	{
		SpawnedFood->SetRespawnAfterConsumed(bSpawnedFoodRespawns);
		SpawnedFood->LaunchFromDispenser(GetFoodDispenseImpulse());
		ActiveSpawnedFoods.Add(SpawnedFood);
		LastDispenseTimeSeconds = CurrentTimeSeconds;
		DispensedFoodCount++;
	}

	return SpawnedFood != nullptr;
}

FText ASquirrelFoodDispenserActor::GetButtonText() const
{
	const ASquirrelTrainingPlayerController* TrainingController = GetTrainingPlayerController();
	const int32 CurrentMoney = TrainingController ? TrainingController->GetMoney() : 0;

	return FText::Format(
		FText::FromString(TEXT("Dispense Food\n${0}   Money: ${1}")),
		FText::AsNumber(FoodCost),
		FText::AsNumber(CurrentMoney));
}

ASquirrelTrainingPlayerController* ASquirrelFoodDispenserActor::GetTrainingPlayerController() const
{
	return Cast<ASquirrelTrainingPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
}

void ASquirrelFoodDispenserActor::PruneInactiveFood()
{
	ActiveSpawnedFoods.RemoveAllSwap(
		[](const TWeakObjectPtr<ASquirrelFoodActor>& SpawnedFood)
		{
			return !SpawnedFood.IsValid();
		});
}

int32 ASquirrelFoodDispenserActor::GetActiveFoodCount() const
{
	int32 ActiveFoodCount = 0;
	for (const TWeakObjectPtr<ASquirrelFoodActor>& SpawnedFood : ActiveSpawnedFoods)
	{
		if (SpawnedFood.IsValid())
		{
			ActiveFoodCount++;
		}
	}

	return ActiveFoodCount;
}

FVector ASquirrelFoodDispenserActor::GetNextFoodSpawnLocation() const
{
	const FVector RightOffset = GetActorRightVector() * FMath::FRandRange(-DispenseSpacing, DispenseSpacing);
	return FoodSpawnPoint->GetComponentLocation() + RightOffset;
}

FVector ASquirrelFoodDispenserActor::GetFoodDispenseImpulse() const
{
	const float SideImpulse = FMath::FRandRange(-DispenseSideImpulseRange, DispenseSideImpulseRange);
	return GetActorForwardVector() * DispenseForwardImpulse
		+ FVector::UpVector * DispenseUpImpulse
		+ GetActorRightVector() * SideImpulse;
}
