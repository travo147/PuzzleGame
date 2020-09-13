// Copyright Epic Games, Inc. All Rights Reserved.

#include "PuzzleGameGameMode.h"
#include "PuzzleGameHUD.h"
#include "PuzzleGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

APuzzleGameGameMode::APuzzleGameGameMode()
	: Super()
{

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = APuzzleGameHUD::StaticClass();

}

void APuzzleGameGameMode::BeginPlay() {
	if (GetWorld()) {
		APlayerController* player = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		player->bShowMouseCursor = true;
		player->SetInputMode(FInputModeUIOnly());
		UGameplayStatics::SetGamePaused(GetWorld(), true);

	}
}

void APuzzleGameGameMode::SetPlaying() {
	if (GetWorld()) {
		APlayerController* player = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		player->bShowMouseCursor = false;
		player->SetInputMode(FInputModeGameOnly());
		player->GetHUD()->SetActorHiddenInGame(false);
		UGameplayStatics::SetGamePaused(GetWorld(), false);

	}
}

void APuzzleGameGameMode::SetPaused() {
	if (GetWorld()) {
		APlayerController* player = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		player->bShowMouseCursor = true;
		player->SetInputMode(FInputModeUIOnly());
		player->GetHUD()->SetActorHiddenInGame(true);
		UGameplayStatics::SetGamePaused(GetWorld(), true);

	}
}

void APuzzleGameGameMode::SetDead() {
	if (GetWorld()) {
		APlayerController* player = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		player->bShowMouseCursor = true;
		player->SetInputMode(FInputModeUIOnly());
		player->GetHUD()->SetActorHiddenInGame(true);
		UGameplayStatics::SetGamePaused(GetWorld(), true);

	}
}

void APuzzleGameGameMode::SetWon() {
	if (GetWorld()) {
		APlayerController* player = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		player->bShowMouseCursor = true;
		player->SetInputMode(FInputModeUIOnly());
		player->GetHUD()->SetActorHiddenInGame(true);
		UGameplayStatics::SetGamePaused(GetWorld(), true);

	}
}

void APuzzleGameGameMode::SetExit() {
	if (GetWorld()) {
		APlayerController* player = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		player->ConsoleCommand("quit");

	}
}
