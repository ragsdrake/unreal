// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "Hub/InteractableBase.h"

AInteractableBase::AInteractableBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	PromptText = NSLOCTEXT("SIB", "DefaultInteractPrompt", "Interact");
}

void AInteractableBase::Interact_Implementation(APawn* InstigatorPawn)
{
	OnInteracted.Broadcast(InstigatorPawn);
}

FText AInteractableBase::GetInteractionPrompt_Implementation() const
{
	return PromptText;
}

bool AInteractableBase::CanInteract_Implementation(const APawn* InstigatorPawn) const
{
	return bInteractionEnabled;
}
