// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SIBInteractable.generated.h"

class APawn;

UINTERFACE(MinimalAPI, Blueprintable)
class USIBInteractable : public UInterface
{
	GENERATED_BODY()
};

/** Contract for anything the player can use in 3D: lab equipment, consoles, deployed machines. */
class SPACEIDLEBOTANIST_API ISIBInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "SIB|Interaction")
	void Interact(APawn* InstigatorPawn);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "SIB|Interaction")
	FText GetInteractionPrompt() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "SIB|Interaction")
	bool CanInteract(const APawn* InstigatorPawn) const;
};
