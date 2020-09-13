// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PuzzleGameGameMode.h"
#include "Components/Button.h"
#include "GameFramework/HUD.h"
#include "PauseMenu.generated.h"

UCLASS()
class PUZZLEGAME_API APauseMenu : public AHUD
{
	GENERATED_BODY()

public:
	APauseMenu();

protected:
	UFUNCTION(BlueprintCallable)
		void SetPlaying();

	UFUNCTION(BlueprintCallable)
		void SetExit();
	
private:

};
