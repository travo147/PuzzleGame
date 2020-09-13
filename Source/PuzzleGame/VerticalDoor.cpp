// Fill out your copyright notice in the Description page of Project Settings.


#include "VerticalDoor.h"

// Sets default values
AVerticalDoor::AVerticalDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;

}

// Called when the game starts or when spawned
void AVerticalDoor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVerticalDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (open) {
		FVector target = BaseMesh->GetComponentLocation();
		target.Z = baseZ + 330.0f;
		BaseMesh->SetRelativeLocation(FMath::Lerp(BaseMesh->GetComponentLocation(), target, 0.05f));
		timeOpen += DeltaTime;
		if (timeOpen > 3) {
			open = false;
		}
	}
	else {
		FVector target = BaseMesh->GetComponentLocation();
		target.Z = baseZ;
		BaseMesh->SetRelativeLocation(FMath::Lerp(BaseMesh->GetComponentLocation(), target, 0.05f));
	}
}

void AVerticalDoor::Interact_Implementation()
{
	if (!isLocked) {
		open = !open;
		timeOpen = 0;
	}
}

