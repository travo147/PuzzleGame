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

//So each state is set here, but there really only needs to be 2. Playing and Paused. Just wanted to be true to the assignment.
//Beginning play is normally a main menu, so the game is initially set to the paused settings.
void APuzzleGameGameMode::BeginPlay() {
	if (GetWorld()) {
		APlayerController* player = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		player->bShowMouseCursor = true;
		player->SetInputMode(FInputModeUIOnly());
		UGameplayStatics::SetGamePaused(GetWorld(), true);

	}
}

//Playing sets the world to be unpaused, hiding the cursor, and sets the input mode to game only.
void APuzzleGameGameMode::SetPlaying() {
	if (GetWorld()) {
		APlayerController* player = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		player->bShowMouseCursor = false;
		player->SetInputMode(FInputModeGameOnly());
		player->GetHUD()->SetActorHiddenInGame(false);
		UGameplayStatics::SetGamePaused(GetWorld(), false);

	}
}

//Paused sets the opposite, showing the cursor, setting the input to UI only, and hiding the current HUD.
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

//Easy way to quit the game.
void APuzzleGameGameMode::SetExit() {
	if (GetWorld()) {
		APlayerController* player = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		player->ConsoleCommand("quit");

	}
}
