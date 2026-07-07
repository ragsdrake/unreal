// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/TimerHandle.h"
#include "SIBCharacter.generated.h"

class UCameraComponent;
class UInputAction;
class USpringArmComponent;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFocusedInteractableChanged, AActor*, NewFocus);

/**
 * Player avatar for hub and sector levels. First/third person toggle via spring-arm length.
 * Interaction detection runs on a low-rate timer trace (PERF-003), never per frame.
 */
UCLASS()
class SPACEIDLEBOTANIST_API ASIBCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASIBCharacter();

	UFUNCTION(BlueprintCallable, Category = "SIB|Camera")
	void ToggleCameraMode();

	UFUNCTION(BlueprintPure, Category = "SIB|Interaction")
	AActor* GetFocusedInteractable() const { return FocusedInteractable.Get(); }

	/** HUD listens here to show/hide the interaction prompt. */
	UPROPERTY(BlueprintAssignable, Category = "SIB|Interaction")
	FOnFocusedInteractableChanged OnFocusedInteractableChanged;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void HandleMove(const FInputActionValue& Value);
	void HandleLook(const FInputActionValue& Value);
	void HandleInteract();
	void ScanForInteractable();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SIB|Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SIB|Camera")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Camera", meta = (ClampMin = "0.0"))
	float FirstPersonArmLength = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Camera", meta = (ClampMin = "0.0"))
	float ThirdPersonArmLength = 300.0f;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "SIB|Camera")
	bool bThirdPersonMode = false;

	/** Seconds between interaction traces (10 Hz default; PERF-003). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Interaction", meta = (ClampMin = "0.01"))
	float InteractionScanInterval = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Interaction", meta = (ClampMin = "0.0"))
	float InteractionRange = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Input")
	TObjectPtr<UInputAction> ToggleCameraAction;

	TWeakObjectPtr<AActor> FocusedInteractable;

	FTimerHandle InteractionScanHandle;
};
