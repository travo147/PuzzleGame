// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PuzzleGameHUD.generated.h"

UCLASS()
class APuzzleGameHUD : public AHUD
{
	GENERATED_BODY()

public:
	APuzzleGameHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Info")
		class APuzzleGameCharacter* player;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Info")
		int playerHealth;

	UFUNCTION(BlueprintCallable)
		void UpdatePlayerHealth();

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

