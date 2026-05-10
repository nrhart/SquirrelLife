// Copyright Epic Games, Inc. All Rights Reserved.

#include "Audio/SquirrelAudioManager.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundAttenuation.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundConcurrency.h"

ASquirrelAudioManager::ASquirrelAudioManager()
{
	PrimaryActorTick.bCanEverTick = false;

	EventCues.Add(ESquirrelAudioEvent::DispenseFood);
	EventCues.Add(ESquirrelAudioEvent::NotEnoughMoney);
	EventCues.Add(ESquirrelAudioEvent::FoodLimitReached);
	EventCues.Add(ESquirrelAudioEvent::EatingStarted);
	EventCues.Add(ESquirrelAudioEvent::EatingFinished);
	EventCues.Add(ESquirrelAudioEvent::DragStarted);
	EventCues.Add(ESquirrelAudioEvent::DragReleased);
	EventCues.Add(ESquirrelAudioEvent::EnergyLevelUp);
	EventCues.Add(ESquirrelAudioEvent::CoinCollected);
	EventCues.Add(ESquirrelAudioEvent::MiniGameDeath);
}

bool ASquirrelAudioManager::PlayAudioEvent(ESquirrelAudioEvent AudioEvent, FVector WorldLocation, AActor* SourceActor)
{
	if (!bEnableAudio)
	{
		return false;
	}

	const FSquirrelAudioCue* Cue = EventCues.Find(AudioEvent);
	if (!Cue || !Cue->Sound)
	{
		if (bDebugMissingSounds && GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Yellow, TEXT("Missing squirrel audio cue."));
		}
		return false;
	}

	if (Cue->bPlay2D)
	{
		UGameplayStatics::PlaySound2D(this, Cue->Sound, Cue->Volume, Cue->Pitch, Cue->StartTime, Cue->ConcurrencySettings, SourceActor);
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			Cue->Sound,
			WorldLocation,
			FRotator::ZeroRotator,
			Cue->Volume,
			Cue->Pitch,
			Cue->StartTime,
			Cue->AttenuationSettings,
			Cue->ConcurrencySettings,
			SourceActor);
	}

	return true;
}

bool ASquirrelAudioManager::PlaySquirrelAudioEvent(const UObject* WorldContextObject, ESquirrelAudioEvent AudioEvent, FVector WorldLocation, AActor* SourceActor)
{
	if (ASquirrelAudioManager* AudioManager = FindSquirrelAudioManager(WorldContextObject))
	{
		return AudioManager->PlayAudioEvent(AudioEvent, WorldLocation, SourceActor);
	}

	return false;
}

ASquirrelAudioManager* ASquirrelAudioManager::FindSquirrelAudioManager(const UObject* WorldContextObject)
{
	const UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull) : nullptr;
	if (!World)
	{
		return nullptr;
	}

	for (TActorIterator<ASquirrelAudioManager> It(World); It; ++It)
	{
		return *It;
	}

	return nullptr;
}
