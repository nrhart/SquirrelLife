// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SquirrelMiniGameAnimInstance.generated.h"

UCLASS(Blueprintable)
class SQUIRRELLIFE_API USquirrelMiniGameAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly, Category = "Mini Game|Animation")
	float Speed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Mini Game|Animation")
	float Direction = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Mini Game|Animation")
	bool bIsMoving = false;

	UPROPERTY(BlueprintReadOnly, Category = "Mini Game|Animation")
	bool bIsInAir = false;
};
