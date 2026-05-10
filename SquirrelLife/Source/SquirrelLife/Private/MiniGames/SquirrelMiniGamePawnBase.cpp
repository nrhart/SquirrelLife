// Copyright Epic Games, Inc. All Rights Reserved.

#include "MiniGames/SquirrelMiniGamePawnBase.h"

#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputCoreTypes.h"
#include "MiniGames/SquirrelMiniGameModeBase.h"
#include "Progress/SquirrelTuningComponent.h"

ASquirrelMiniGamePawnBase::ASquirrelMiniGamePawnBase()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(34.0f, 58.0f);
	GetCharacterMovement()->GravityScale = 2.1f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	CameraBoom->bDoCollisionTest = false;

	SideViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	SideViewCamera->bUsePawnControlRotation = false;

	TuningComponent = CreateDefaultSubobject<USquirrelTuningComponent>(TEXT("TuningComponent"));

	ApplyMiniGameMovementTuning();
	ApplyMiniGameVisualTuning();
}

void ASquirrelMiniGamePawnBase::BeginPlay()
{
	Super::BeginPlay();

	ApplyMiniGameMovementTuning();
	ApplyMiniGameVisualTuning();
}

void ASquirrelMiniGamePawnBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ApplyMiniGameMovementTuning();
	ApplyMiniGameVisualTuning();
}

void ASquirrelMiniGamePawnBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ApplyMiniGameMovementTuning();
	RefreshMiniGameAnimationState();
}

void ASquirrelMiniGamePawnBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (!bBindJumpInput)
	{
		return;
	}

	PlayerInputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &ASquirrelMiniGamePawnBase::StartJump);
	PlayerInputComponent->BindKey(EKeys::SpaceBar, IE_Released, this, &ASquirrelMiniGamePawnBase::StopJump);

	if (bBindMouseJumpInput)
	{
		PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &ASquirrelMiniGamePawnBase::StartJump);
		PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &ASquirrelMiniGamePawnBase::StopJump);
	}
}

float ASquirrelMiniGamePawnBase::GetTargetMoveSpeed() const
{
	if (const ASquirrelMiniGameModeBase* MiniGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ASquirrelMiniGameModeBase>() : nullptr)
	{
		return MiniGameMode->GetCurrentScrollSpeed();
	}

	return FallbackMoveSpeed;
}

void ASquirrelMiniGamePawnBase::ApplyMiniGameVisualTuning()
{
	USkeletalMeshComponent* CharacterMesh = GetMesh();
	if (MiniGameAnimClass && CharacterMesh)
	{
		CharacterMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		CharacterMesh->SetAnimInstanceClass(MiniGameAnimClass);
	}
}

void ASquirrelMiniGamePawnBase::ApplyMiniGameMovementTuning()
{
	if (CameraBoom)
	{
		CameraBoom->TargetArmLength = CameraDistance;
		CameraBoom->SocketOffset = FVector(0.0f, 0.0f, CameraHeight);
	}

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = GetTargetMoveSpeed();
		MoveComp->JumpZVelocity = JumpVelocity;
	}
}

void ASquirrelMiniGamePawnBase::RefreshMiniGameAnimationState()
{
	MiniGameAnimationSpeed = GetMiniGameSpeed();
	bMiniGameAnimationMoving = MiniGameAnimationSpeed > 5.0f;
	bMiniGameAnimationInAir = IsInAir();
}

ASquirrelMiniGameModeBase* ASquirrelMiniGamePawnBase::GetMiniGameMode() const
{
	return GetWorld() ? GetWorld()->GetAuthGameMode<ASquirrelMiniGameModeBase>() : nullptr;
}

void ASquirrelMiniGamePawnBase::StartJump()
{
	Jump();
}

void ASquirrelMiniGamePawnBase::StopJump()
{
	StopJumping();
}

void ASquirrelMiniGamePawnBase::AwardScoreUnits(int32 ScoreUnits)
{
	if (ASquirrelMiniGameModeBase* MiniGameMode = GetMiniGameMode())
	{
		MiniGameMode->AwardScoreUnits(ScoreUnits);
	}
}

void ASquirrelMiniGamePawnBase::AwardCoins(int32 Amount)
{
	if (ASquirrelMiniGameModeBase* MiniGameMode = GetMiniGameMode())
	{
		MiniGameMode->AwardCoins(Amount);
	}
}

bool ASquirrelMiniGamePawnBase::AwardStatProgress(int32 Amount)
{
	if (ASquirrelMiniGameModeBase* MiniGameMode = GetMiniGameMode())
	{
		return MiniGameMode->AwardStatProgress(Amount);
	}

	return false;
}

float ASquirrelMiniGamePawnBase::GetMiniGameSpeed() const
{
	return GetVelocity().Size2D();
}

bool ASquirrelMiniGamePawnBase::IsInAir() const
{
	const UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	return MoveComp ? MoveComp->IsFalling() : false;
}

bool ASquirrelMiniGamePawnBase::IsOnGround() const
{
	const UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	return MoveComp ? MoveComp->IsMovingOnGround() : false;
}

void ASquirrelMiniGamePawnBase::SetMiniGameAnimClass(TSubclassOf<UAnimInstance> NewAnimClass)
{
	MiniGameAnimClass = NewAnimClass;
	ApplyMiniGameVisualTuning();
}
