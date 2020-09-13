// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "PuzzleGameGameMode.generated.h"

UCLASS(minimalapi)
class APuzzleGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APuzzleGameGameMode();

	UFUNCTION(BlueprintCallable)
		void SetPlaying();

	UFUNCTION(BlueprintCallable)
		void SetPaused();

	UFUNCTION(BlueprintCallable)
		void SetDead();

	UFUNCTION(BlueprintCallable)
		void SetWon();

	UFUNCTION(BlueprintCallable)
		void SetExit();

	UFUNCTION(BlueprintCallable)
		bool GetPaused() { return IsPaused(); }

	virtual void BeginPlay() override;

private:

};



