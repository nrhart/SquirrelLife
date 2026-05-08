// Copyright Epic Games, Inc. All Rights Reserved.

#include "Training/SquirrelFoodActor.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Training/SquirrelTrainingPawn.h"

ASquirrelFoodActor::ASquirrelFoodActor()
{
	PrimaryActorTick.bCanEverTick = false;

	FoodMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FoodMesh"));
	SetRootComponent(FoodMesh);
	FoodMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	FoodMesh->SetCollisionObjectType(ECC_PhysicsBody);
	FoodMesh->SetCollisionResponseToAllChannels(ECR_Block);
	FoodMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	FoodMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	FoodMesh->SetSimulatePhysics(true);
	FoodMesh->SetNotifyRigidBodyCollision(false);
	ApplyFoodTuning();

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		FoodMesh->SetStaticMesh(SphereMesh.Object);
	}
}

void ASquirrelFoodActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ApplyFoodTuning();
}

void ASquirrelFoodActor::BeginPlay()
{
	Super::BeginPlay();

	FoodMesh->SetSimulatePhysics(bSimulatePhysicsOnSpawn);
}

void ASquirrelFoodActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(RespawnTimer);
	}

	Super::EndPlay(EndPlayReason);
}

void ASquirrelFoodActor::ApplyFoodTuning()
{
	if (!FoodMesh)
	{
		return;
	}

	FoodMesh->SetRelativeScale3D(FoodMeshScale);
	FoodMesh->SetLinearDamping(LinearDamping);
	FoodMesh->SetAngularDamping(AngularDamping);
}

bool ASquirrelFoodActor::TryConsume(ASquirrelTrainingPawn* Squirrel)
{
	if (!Squirrel || (IsHidden() && !bIsHeldForEating))
	{
		return false;
	}

	Consume(Squirrel);
	return true;
}

void ASquirrelFoodActor::Consume(ASquirrelTrainingPawn* Squirrel)
{
	if (!Squirrel || (IsHidden() && !bIsHeldForEating))
	{
		return;
	}

	bIsHeldForEating = false;
	FoodMesh->SetSimulatePhysics(false);
	Squirrel->AddPower(PowerValue);
	Squirrel->AddEnergyFromFood(PowerValue);
	OnConsumed(Squirrel);

	if (bRespawnAfterConsumed)
	{
		SetConsumed(true);

		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(RespawnTimer, this, &ASquirrelFoodActor::Respawn, RespawnDelay, false);
		}
	}
	else
	{
		Destroy();
	}
}

void ASquirrelFoodActor::Respawn()
{
	bIsHeldForEating = false;
	SetConsumed(false);
	OnRespawned();
}

void ASquirrelFoodActor::SetConsumed(bool bConsumed)
{
	if (bConsumed)
	{
		bIsHeldForEating = false;
	}

	SetActorHiddenInGame(bConsumed);
	SetActorEnableCollision(!bConsumed);
	SetActorTickEnabled(!bConsumed);

	if (FoodMesh)
	{
		FoodMesh->SetSimulatePhysics(!bConsumed && bSimulatePhysicsOnSpawn);
	}
}

void ASquirrelFoodActor::BeginHeldForEating()
{
	bIsHeldForEating = true;
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	if (FoodMesh)
	{
		FoodMesh->SetSimulatePhysics(false);
		FoodMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
		FoodMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	}
}

void ASquirrelFoodActor::RestoreFromEatingHold()
{
	bIsHeldForEating = false;
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);

	if (FoodMesh)
	{
		FoodMesh->SetSimulatePhysics(bSimulatePhysicsOnSpawn);
	}
}

void ASquirrelFoodActor::LaunchFromDispenser(const FVector& Impulse)
{
	if (!FoodMesh)
	{
		return;
	}

	FoodMesh->SetSimulatePhysics(true);
	FoodMesh->AddImpulse(Impulse, NAME_None, true);
}
