// Copyright Epic Games, Inc. All Rights Reserved.

#include "MiniGames/Runner/SquirrelRunnerPlatformTile.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"

ASquirrelRunnerPlatformTile::ASquirrelRunnerPlatformTile()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	PlatformMesh->SetupAttachment(RootComponent);
	PlatformMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PlatformMesh->SetCollisionObjectType(ECC_WorldStatic);
	PlatformMesh->SetCollisionResponseToAllChannels(ECR_Block);
	PlatformMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		PlatformMesh->SetStaticMesh(CubeMesh.Object);
	}
}

void ASquirrelRunnerPlatformTile::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoFindPlayer && !TargetActor)
	{
		FindPlayerTarget();
	}
}

void ASquirrelRunnerPlatformTile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bAutoRecycle)
	{
		return;
	}

	if (bAutoFindPlayer && !TargetActor)
	{
		FindPlayerTarget();
	}

	if (ShouldRecycle())
	{
		RecycleTile();
	}
}

void ASquirrelRunnerPlatformTile::FindPlayerTarget()
{
	TargetActor = UGameplayStatics::GetPlayerPawn(this, 0);
}

bool ASquirrelRunnerPlatformTile::ShouldRecycle() const
{
	if (!TargetActor)
	{
		return false;
	}

	const FVector Direction = RecycleDirection.GetSafeNormal();
	if (Direction.IsNearlyZero())
	{
		return false;
	}

	const float TilePosition = FVector::DotProduct(GetActorLocation(), Direction);
	const float TargetPosition = FVector::DotProduct(TargetActor->GetActorLocation(), Direction);
	return TilePosition < TargetPosition - RecycleBehindDistance;
}

void ASquirrelRunnerPlatformTile::SetTargetActor(AActor* NewTargetActor)
{
	TargetActor = NewTargetActor;
}

void ASquirrelRunnerPlatformTile::RecycleTile()
{
	const FVector Direction = RecycleDirection.GetSafeNormal();
	if (Direction.IsNearlyZero())
	{
		return;
	}

	SetActorLocation(GetActorLocation() + Direction * GetLoopDistance());
}
