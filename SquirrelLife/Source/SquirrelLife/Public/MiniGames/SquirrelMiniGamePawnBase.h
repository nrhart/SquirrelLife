// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SquirrelMiniGamePawnBase.generated.h"

class ASquirrelMiniGameModeBase;
class UAnimInstance;
class UCameraComponent;
class USquirrelTuningComponent;
class USpringArmComponent;

UCLASS(Blueprintable)
class SQUIRRELLIFE_API ASquirrelMiniGamePawnBase : public ACharacter
{
	GENERATED_BODY()

public:
	ASquirrelMiniGamePawnBase();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> SideViewCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USquirrelTuningComponent> TuningComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Visuals")
	TSubclassOf<UAnimInstance> MiniGameAnimClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Input")
	bool bBindJumpInput = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Input", meta = (EditCondition = "bBindJumpInput"))
	bool bBindMouseJumpInput = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Movement", meta = (ClampMin = "0", Units = "cm/s"))
	float FallbackMoveSpeed = 450.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Movement", meta = (ClampMin = "0", Units = "cm/s"))
	float JumpVelocity = 520.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Camera", meta = (ClampMin = "0", Units = "cm"))
	float CameraDistance = 900.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mini Game|Camera", meta = (ClampMin = "0", Units = "cm"))
	float CameraHeight = 180.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mini Game|Animation")
	float MiniGameAnimationSpeed = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mini Game|Animation")
	bool bMiniGameAnimationMoving = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mini Game|Animation")
	bool bMiniGameAnimationInAir = false;

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual float GetTargetMoveSpeed() const;

	void ApplyMiniGameVisualTuning();
	void ApplyMiniGameMovementTuning();
	void RefreshMiniGameAnimationState();
	ASquirrelMiniGameModeBase* GetMiniGameMode() const;
	void StartJump();
	void StopJump();

public:
	UFUNCTION(BlueprintCallable, Category = "Mini Game|Rewards")
	void AwardScoreUnits(int32 ScoreUnits);

	UFUNCTION(BlueprintCallable, Category = "Mini Game|Rewards")
	void AwardCoins(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Mini Game|Rewards")
	bool AwardStatProgress(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Mini Game|Animation")
	virtual float GetMiniGameSpeed() const;

	UFUNCTION(BlueprintPure, Category = "Mini Game|Animation")
	float GetAnimationSpeed() const { return MiniGameAnimationSpeed; }

	UFUNCTION(BlueprintPure, Category = "Mini Game|Animation")
	bool IsMiniGameMoving() const { return bMiniGameAnimationMoving; }

	UFUNCTION(BlueprintPure, Category = "Mini Game|Animation")
	bool IsAnimationInAir() const { return bMiniGameAnimationInAir; }

	UFUNCTION(BlueprintPure, Category = "Mini Game|Animation")
	bool IsInAir() const;

	UFUNCTION(BlueprintPure, Category = "Mini Game|Animation")
	bool IsOnGround() const;

	UFUNCTION(BlueprintPure, Category = "Mini Game|Animation")
	float GetVisualMovementSpeed() const { return GetAnimationSpeed(); }

	UFUNCTION(BlueprintPure, Category = "Mini Game|Animation")
	bool IsGrounded() const { return IsOnGround(); }

	UFUNCTION(BlueprintPure, Category = "Mini Game|Animation")
	bool IsEating() const { return false; }

	UFUNCTION(BlueprintPure, Category = "Mini Game|Visuals")
	USkeletalMeshComponent* GetMiniGameMesh() const { return GetMesh(); }

	UFUNCTION(BlueprintPure, Category = "Mini Game|Tuning")
	USquirrelTuningComponent* GetTuningComponent() const { return TuningComponent; }

	UFUNCTION(BlueprintCallable, Category = "Mini Game|Visuals")
	void SetMiniGameAnimClass(TSubclassOf<UAnimInstance> NewAnimClass);
};
