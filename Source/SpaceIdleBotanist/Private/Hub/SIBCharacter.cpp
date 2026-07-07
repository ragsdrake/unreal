// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "Hub/SIBCharacter.h"

#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Engine/World.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "Interfaces/SIBInteractable.h"
#include "TimerManager.h"

ASIBCharacter::ASIBCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->TargetArmLength = FirstPersonArmLength;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
}

void ASIBCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(
		InteractionScanHandle, this, &ASIBCharacter::ScanForInteractable, InteractionScanInterval, true);
}

void ASIBCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(InteractionScanHandle);
	Super::EndPlay(EndPlayReason);
}

void ASIBCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASIBCharacter::HandleMove);
		}
		if (LookAction)
		{
			EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASIBCharacter::HandleLook);
		}
		if (InteractAction)
		{
			EnhancedInput->BindAction(InteractAction, ETriggerEvent::Started, this, &ASIBCharacter::HandleInteract);
		}
		if (ToggleCameraAction)
		{
			EnhancedInput->BindAction(ToggleCameraAction, ETriggerEvent::Started, this, &ASIBCharacter::ToggleCameraMode);
		}
	}
}

void ASIBCharacter::ToggleCameraMode()
{
	bThirdPersonMode = !bThirdPersonMode;
	// Blueprint children may smooth this with a Timeline; C++ snaps directly.
	CameraBoom->TargetArmLength = bThirdPersonMode ? ThirdPersonArmLength : FirstPersonArmLength;
}

void ASIBCharacter::HandleMove(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	if (Controller)
	{
		const FRotator YawRotation(0.0f, GetControlRotation().Yaw, 0.0f);
		AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), Axis.Y);
		AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), Axis.X);
	}
}

void ASIBCharacter::HandleLook(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(Axis.Y);
}

void ASIBCharacter::HandleInteract()
{
	AActor* Target = FocusedInteractable.Get();
	if (Target && Target->Implements<USIBInteractable>())
	{
		ISIBInteractable::Execute_Interact(Target, this);
	}
}

void ASIBCharacter::ScanForInteractable()
{
	AActor* NewFocus = nullptr;

	if (Camera)
	{
		const FVector Start = Camera->GetComponentLocation();
		const FVector End = Start + Camera->GetForwardVector() * InteractionRange;

		FHitResult Hit;
		FCollisionQueryParams Params(SCENE_QUERY_STAT(SIBInteractScan), false, this);
		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
		{
			AActor* HitActor = Hit.GetActor();
			if (HitActor && HitActor->Implements<USIBInteractable>() &&
				ISIBInteractable::Execute_CanInteract(HitActor, this))
			{
				NewFocus = HitActor;
			}
		}
	}

	if (NewFocus != FocusedInteractable.Get())
	{
		FocusedInteractable = NewFocus;
		OnFocusedInteractableChanged.Broadcast(NewFocus);
	}
}
