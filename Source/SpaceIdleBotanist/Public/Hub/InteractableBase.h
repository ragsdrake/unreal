// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/SIBInteractable.h"
#include "InteractableBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteracted, APawn*, InstigatorPawn);

/** Base for all usable ship equipment. Blueprint children supply meshes, highlight VFX, and timeline animations. */
UCLASS(Abstract)
class SPACEIDLEBOTANIST_API AInteractableBase : public AActor, public ISIBInteractable
{
	GENERATED_BODY()

public:
	AInteractableBase();

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
	virtual FText GetInteractionPrompt_Implementation() const override;
	virtual bool CanInteract_Implementation(const APawn* InstigatorPawn) const override;

	UPROPERTY(BlueprintAssignable, Category = "SIB|Interaction")
	FOnInteracted OnInteracted;

protected:
	/** Visual hooks for Blueprint children (outline material, glow timeline, etc.). */
	UFUNCTION(BlueprintImplementableEvent, Category = "SIB|Interaction")
	void OnHighlightBegin();

	UFUNCTION(BlueprintImplementableEvent, Category = "SIB|Interaction")
	void OnHighlightEnd();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Interaction")
	FText PromptText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Interaction")
	bool bInteractionEnabled = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SIB|Interaction")
	TObjectPtr<USceneComponent> Root;
};
