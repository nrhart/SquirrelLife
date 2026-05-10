// Copyright Epic Games, Inc. All Rights Reserved.

#include "MiniGames/Runner/SquirrelEndlessRunnerPawn.h"

#include "Audio/SquirrelAudioManager.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ASquirrelEndlessRunnerPawn::ASquirrelEndlessRunnerPawn()
{
}

void ASquirrelEndlessRunnerPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsDeathSequencePlaying)
	{
		if (USkeletalMeshComponent* CharacterMesh = GetMesh())
		{
			CharacterMesh->AddLocalRotation(FRotator(0.0f, 0.0f, DeathSpinSpeed * DeltaSeconds));
		}
		return;
	}

	if (bAutoRun)
	{
		if (!bRunInPlace)
		{
			AddMovementInput(AutoRunDirection.GetSafeNormal(), 1.0f, true);
		}
	}
}

void ASquirrelEndlessRunnerPawn::PlayDeathSequence()
{
	if (bIsDeathSequencePlaying)
	{
		return;
	}

	bIsDeathSequencePlaying = true;
	bAutoRun = false;
	StopJumping();

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->SetMovementMode(MOVE_Falling);
		const FVector CameraPushDirection = DeathTowardCameraDirection.GetSafeNormal();
		MoveComp->Velocity = CameraPushDirection * DeathTowardCameraVelocity + FVector::UpVector * DeathHopVelocity;
	}

	if (bDisableCollisionOnDeath)
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	ASquirrelAudioManager::PlaySquirrelAudioEvent(this, ESquirrelAudioEvent::MiniGameDeath, GetActorLocation(), this);
}

void ASquirrelEndlessRunnerPawn::AwardRunScore(int32 ScoreUnits)
{
	AwardScoreUnits(ScoreUnits);
}

float ASquirrelEndlessRunnerPawn::GetRunnerSpeed() const
{
	return GetMiniGameSpeed();
}

float ASquirrelEndlessRunnerPawn::GetMiniGameSpeed() const
{
	if (bAutoRun && bRunInPlace)
	{
		return GetTargetMoveSpeed();
	}

	return Super::GetMiniGameSpeed();
}
