// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Progress/SquirrelProgressTypes.h"
#include "SquirrelSaveGame.generated.h"

UCLASS()
class SQUIRRELLIFE_API USquirrelSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	int32 SaveVersion = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Economy")
	int32 Coins = 25;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Progress")
	TMap<ESquirrelProgressStat, FSquirrelLevelProgress> ProgressStats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Park")
	bool bHasParkSquirrelTransform = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Park")
	FTransform ParkSquirrelTransform = FTransform::Identity;
};
