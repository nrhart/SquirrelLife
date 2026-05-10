// Copyright Epic Games, Inc. All Rights Reserved.

#include "MiniGames/SquirrelMiniGameAnimInstance.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MiniGames/SquirrelMiniGamePawnBase.h"

void USquirrelMiniGameAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APawn* PawnOwner = TryGetPawnOwner();
	if (!PawnOwner)
	{
		Speed = 0.0f;
		Direction = 0.0f;
		bIsMoving = false;
		bIsInAir = false;
		return;
	}

	if (const ASquirrelMiniGamePawnBase* MiniGamePawn = Cast<ASquirrelMiniGamePawnBase>(PawnOwner))
	{
		Speed = MiniGamePawn->GetAnimationSpeed();
		bIsMoving = MiniGamePawn->IsMiniGameMoving();
		bIsInAir = MiniGamePawn->IsInAir();
		Direction = 0.0f;
		return;
	}

	const FVector Velocity = PawnOwner->GetVelocity();
	Speed = FVector(Velocity.X, Velocity.Y, 0.0f).Size();
	bIsMoving = Speed > 5.0f;
	Direction = 0.0f;

	const ACharacter* CharacterOwner = Cast<ACharacter>(PawnOwner);
	const UCharacterMovementComponent* MoveComp = CharacterOwner ? CharacterOwner->GetCharacterMovement() : nullptr;
	bIsInAir = MoveComp ? MoveComp->IsFalling() : false;
}
