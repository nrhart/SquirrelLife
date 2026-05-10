// Copyright Epic Games, Inc. All Rights Reserved.

#include "MiniGames/Runner/SquirrelRunnerCoin.h"

#include "Audio/SquirrelAudioManager.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "MiniGames/SquirrelMiniGameModeBase.h"

ASquirrelRunnerCoin::ASquirrelRunnerCoin()
{
	PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	Collision->InitSphereRadius(34.0f);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collision->SetCollisionObjectType(ECC_WorldDynamic);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Collision->SetGenerateOverlapEvents(true);
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ASquirrelRunnerCoin::HandleCoinOverlap);

	CoinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoinMesh"));
	CoinMesh->SetupAttachment(RootComponent);
	CoinMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CoinMesh->SetRelativeScale3D(FVector(0.6f, 0.12f, 0.6f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		CoinMesh->SetStaticMesh(CylinderMesh.Object);
	}
}

void ASquirrelRunnerCoin::BeginPlay()
{
	Super::BeginPlay();

	if (!TargetActor)
	{
		TargetActor = UGameplayStatics::GetPlayerPawn(this, 0);
	}
}

void ASquirrelRunnerCoin::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsPooledInactive)
	{
		return;
	}

	if (bUseBuiltInSpin && CoinMesh && SpinDegreesPerSecond > 0.0f)
	{
		CoinMesh->AddLocalRotation(FRotator(0.0f, SpinDegreesPerSecond * DeltaSeconds, 0.0f));
	}

	if (ShouldDestroyBehindPlayer())
	{
		Destroy();
	}
}

void ASquirrelRunnerCoin::HandleCoinOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsPooledInactive || !OtherActor || (TargetActor && OtherActor != TargetActor))
	{
		return;
	}

	CollectCoin(OtherActor);
}

bool ASquirrelRunnerCoin::ShouldDestroyBehindPlayer() const
{
	if (!bDestroyWhenBehindPlayer)
	{
		return false;
	}

	return HasPassedTarget(DestroyBehindDistance);
}

void ASquirrelRunnerCoin::CollectCoin(AActor* Collector)
{
	if (ASquirrelMiniGameModeBase* MiniGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ASquirrelMiniGameModeBase>() : nullptr)
	{
		MiniGameMode->CollectMiniGameCoins(CoinValue);
	}

	ASquirrelAudioManager::PlaySquirrelAudioEvent(this, ESquirrelAudioEvent::CoinCollected, GetActorLocation(), this);

	if (bDestroyWhenBehindPlayer)
	{
		Destroy();
	}
	else
	{
		DeactivateForPool();
	}
}

void ASquirrelRunnerCoin::SetCoinMesh(UStaticMesh* NewMesh)
{
	if (CoinMesh && NewMesh)
	{
		CoinMesh->SetStaticMesh(NewMesh);
	}
}

void ASquirrelRunnerCoin::ActivateFromPool(const FVector& NewLocation, const FRotator& NewRotation, AActor* NewTargetActor)
{
	bIsPooledInactive = false;
	TargetActor = NewTargetActor;
	SetActorLocationAndRotation(NewLocation, NewRotation, false, nullptr, ETeleportType::TeleportPhysics);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
}

void ASquirrelRunnerCoin::DeactivateForPool()
{
	bIsPooledInactive = true;
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
}

bool ASquirrelRunnerCoin::HasPassedTarget(float BehindDistance) const
{
	if (!TargetActor)
	{
		return false;
	}

	return GetActorLocation().X < TargetActor->GetActorLocation().X - BehindDistance;
}
