// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "VerticalDoor.h"
#include "GameFramework/Actor.h"
#include "DoorKey.generated.h"

UCLASS()
class PUZZLEGAME_API ADoorKey : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADoorKey();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Interact")
		class UStaticMeshComponent* BaseMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Door")
		class AVerticalDoor* Door;

	AVerticalDoor* GetDoor();
	bool GetRemote() { return isRemote; }
	void SetRemote(bool remote) { isRemote = remote; }
	void SetDoor(AVerticalDoor* door) { Door = door; }

private:
	bool isRemote = false;

};
