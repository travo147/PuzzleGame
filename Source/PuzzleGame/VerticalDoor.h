// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "GameFramework/Actor.h"
#include "VerticalDoor.generated.h"

UCLASS()
class PUZZLEGAME_API AVerticalDoor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVerticalDoor();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Interact")
		class UStaticMeshComponent* BaseMesh;

	void UnlockDoor() { isLocked = false; }
	bool IsLocked() { return isLocked; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool isLocked = true;
	float timeOpen;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interact")
		void Interact();
	virtual void Interact_Implementation() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Interact")
		bool open = false;
	float baseZ = 300.0f;

};
