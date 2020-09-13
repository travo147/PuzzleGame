// Copyright Epic Games, Inc. All Rights Reserved.

#include "PuzzleGameCharacter.h"
#include "PuzzleGameProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// APuzzleGameCharacter

APuzzleGameCharacter::APuzzleGameCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
}

bool APuzzleGameCharacter::Trace(
	UWorld* World,
	TArray<AActor*>& ActorsToIgnore,
	const FVector& Start,
	const FVector& End,
	FHitResult& HitOut,
	ECollisionChannel CollisionChannel = ECC_Pawn,
	bool ReturnPhysMat = false
) {
	if (!World) {
		return false;
	}

	FCollisionQueryParams TraceParams(FName(TEXT("My Trace")), true, ActorsToIgnore[0]);
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = ReturnPhysMat;
	TraceParams.AddIgnoredActors(ActorsToIgnore);
	const FName TraceTag("MyTraceTag");
	//World->DebugDrawTraceTag = TraceTag;
	TraceParams.TraceTag = TraceTag;
	HitOut = FHitResult(ForceInit);

	World->LineTraceSingleByChannel(
		HitOut,
		Start,
		End,
		CollisionChannel,
		TraceParams
	);

	return(HitOut.GetActor() != NULL);

}

void APuzzleGameCharacter::CallMyTrace() {
	const FVector Start = FirstPersonCameraComponent->GetComponentLocation();
	const FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
	const FVector End = Start + ForwardVector * 500;

	FHitResult HitData(ForceInit);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	if (Trace(GetWorld(), ActorsToIgnore, Start, End, HitData, ECC_Visibility, false)) {
		if (HitData.GetActor()) {
			ProcessTraceHit(HitData);
		}
		else {
			ClearTrace();
		}
	}
	else {
		ClearTrace();
	}

}

void APuzzleGameCharacter::ProcessTraceHit(FHitResult& HitOut) {

	if (HitOut.GetActor()->GetClass()->ImplementsInterface(UInteractable::StaticClass())) {
		//UE_LOG(LogClass, Warning, TEXT("It's Interactable. %s"), *HitOut.GetActor()->GetName());
		isInteractable = true;
		currentActor = HitOut.GetActor();
		if (currentActor->GetClass()->IsChildOf(AVerticalDoor::StaticClass())) {
			AVerticalDoor* Door = Cast<AVerticalDoor>(currentActor);
			actorName = "Door\n";
			actorInfo = "This is a door that can be unlocked with a key.\n";
			if (Door->IsLocked()) {
				actorState = "The Door is currently locked.\nPlease find a key to unlock it.\n";
			}
			else {
				actorState = "The Door is unlocked.\nPress 'E' to open the door.\n";
			}
		}
		else if (currentActor->GetClass()->IsChildOf(ADoorKey::StaticClass())) {
			ADoorKey* Key = Cast<ADoorKey>(currentActor);
			actorName = "Door Key\n";
			if (Key->GetRemote()) {
				actorInfo = "This is a remote key that unlocks a nearby door.\n";
				if (Key->GetDoor()->IsLocked()) {
					actorState = "Press E to unlock the nearby door.\n";
				}
				else {
					actorState = "The nearby Door is unlocked.\n";
				}
			}
			else {
				actorInfo = "This is a key that can unlock a door.\n";
				actorState = "Press E to pick up the key.\n";
			}
		}
	}
	else {
		ClearTrace();
	}

}

void APuzzleGameCharacter::ClearTrace() {
	isInteractable = false;
	actorName = "";
	actorInfo = "";
	actorState = "";
	return;
}

void APuzzleGameCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	time += DeltaTime;
	if (time > 5) {
		time -= 5;
		health -= 10;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%i"), health));
	}
	CallMyTrace();

	/*if (health <= 0) {
		playerisdead;
	}*/

}

bool APuzzleGameCharacter::GetDoorKey() {
	if (doorKey) return true;
	else return false;

}

void APuzzleGameCharacter::Interact() {
	if (currentActor && isInteractable) {
		if (currentActor->GetClass()->IsChildOf(AVerticalDoor::StaticClass())) {
			AVerticalDoor* Door = Cast<AVerticalDoor>(currentActor);
			if (Door->IsLocked() && doorKey) {
				ADoorKey* Key = Cast<ADoorKey>(doorKey);
				if (Key->GetDoor() == Door) {
					Door->UnlockDoor();
					doorKey = nullptr;
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Door unlocked."));
					AVerticalDoor::Execute_Interact(currentActor);
				}
			}
			else if (!Door->IsLocked()) {
				AVerticalDoor::Execute_Interact(currentActor);
			}
		}
		else if (currentActor->GetClass()->IsChildOf(ADoorKey::StaticClass())) {
			if (currentActor->GetName() == "MyDoorKey_2") {
				doorKey = currentActor;
				currentActor->SetActorHiddenInGame(true);
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Key picked up."));
			}
			if (currentActor->GetName() == "MyDoorKey2_5") {
				ADoorKey* Key = Cast<ADoorKey>(currentActor);
				if (Key->GetDoor()->IsLocked()) {
					Key->GetDoor()->UnlockDoor();
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Door unlocked somewhere."));
				}
			}
		}
		//IInteractable::Execute_Interact(currentActor);
	}

}

void APuzzleGameCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}

	TArray<AActor*> doorList;
	TArray<AActor*> keyList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVerticalDoor::StaticClass(), doorList);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADoorKey::StaticClass(), keyList);

	for (int i = 0; i < keyList.Num(); i++) {
		if (keyList[i]->GetName() == "MyDoorKey_2") {
			for (int j = 0; j < doorList.Num(); j++) {
				if (doorList[j]->GetName() == "MyVerticalDoor_2") {
					ADoorKey* key = Cast<ADoorKey>(keyList[i]);
					AVerticalDoor* door = Cast<AVerticalDoor>(doorList[j]);
					key->SetDoor(door);
				}
			}
		}
		else if (keyList[i]->GetName() == "MyDoorKey2_5") {
			for (int j = 0; j < doorList.Num(); j++) {
				if (doorList[j]->GetName() == "MyVerticalDoor2_2") {
					ADoorKey* key = Cast<ADoorKey>(keyList[i]);
					AVerticalDoor* door = Cast<AVerticalDoor>(doorList[j]);
					key->SetDoor(door);
					key->SetRemote(true);
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void APuzzleGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APuzzleGameCharacter::Interact);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APuzzleGameCharacter::OnFire);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &APuzzleGameCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &APuzzleGameCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APuzzleGameCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &APuzzleGameCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APuzzleGameCharacter::LookUpAtRate);
}

void APuzzleGameCharacter::OnFire()
{
	/*
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			if (bUsingMotionControllers)
			{
				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				World->SpawnActor<APuzzleGameProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
			}
			else
			{
				const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				World->SpawnActor<APuzzleGameProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			}
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
	*/
}

void APuzzleGameCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void APuzzleGameCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void APuzzleGameCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void APuzzleGameCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void APuzzleGameCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void APuzzleGameCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void APuzzleGameCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APuzzleGameCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool APuzzleGameCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &APuzzleGameCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &APuzzleGameCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &APuzzleGameCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}
