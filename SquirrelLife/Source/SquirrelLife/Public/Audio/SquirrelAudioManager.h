// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SquirrelAudioManager.generated.h"

class USoundAttenuation;
class USoundBase;
class USoundConcurrency;

UENUM(BlueprintType)
enum class ESquirrelAudioEvent : uint8
{
	DispenseFood,
	NotEnoughMoney,
	FoodLimitReached,
	EatingStarted,
	EatingFinished,
	DragStarted,
	DragReleased,
	EnergyLevelUp,
	CoinCollected,
	MiniGameDeath
};

USTRUCT(BlueprintType)
struct FSquirrelAudioCue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	TObjectPtr<USoundBase> Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (ClampMin = "0"))
	float Volume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (ClampMin = "0"))
	float Pitch = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (ClampMin = "0", Units = "s"))
	float StartTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	bool bPlay2D = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (EditCondition = "!bPlay2D"))
	TObjectPtr<USoundAttenuation> AttenuationSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	TObjectPtr<USoundConcurrency> ConcurrencySettings;
};

/**
 * Place one audio manager in the training level, or create a BP child and assign
 * sounds per gameplay event. Gameplay code looks this actor up when firing SFX.
 */
UCLASS(Blueprintable)
class SQUIRRELLIFE_API ASquirrelAudioManager : public AActor
{
	GENERATED_BODY()

public:
	ASquirrelAudioManager();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	bool bEnableAudio = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	bool bDebugMissingSounds = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	TMap<ESquirrelAudioEvent, FSquirrelAudioCue> EventCues;

public:
	UFUNCTION(BlueprintCallable, Category = "Audio")
	bool PlayAudioEvent(ESquirrelAudioEvent AudioEvent, FVector WorldLocation, AActor* SourceActor = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Audio", meta = (WorldContext = "WorldContextObject"))
	static bool PlaySquirrelAudioEvent(const UObject* WorldContextObject, ESquirrelAudioEvent AudioEvent, FVector WorldLocation, AActor* SourceActor = nullptr);

	UFUNCTION(BlueprintPure, Category = "Audio", meta = (WorldContext = "WorldContextObject"))
	static ASquirrelAudioManager* FindSquirrelAudioManager(const UObject* WorldContextObject);
};
