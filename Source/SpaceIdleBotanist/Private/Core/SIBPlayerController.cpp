// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "Core/SIBPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

void ASIBPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// Phase 3: swap to SectorMappingContext while in a spoke level; hub is the default.
		if (UInputMappingContext* Context = HubMappingContext.LoadSynchronous())
		{
			InputSubsystem->AddMappingContext(Context, MappingContextPriority);
		}
	}
}

void ASIBPlayerController::EnterUIMode(UUserWidget* FocusWidget)
{
	FInputModeUIOnly InputMode;
	if (FocusWidget)
	{
		InputMode.SetWidgetToFocus(FocusWidget->TakeWidget());
	}
	SetInputMode(InputMode);
	SetShowMouseCursor(true);
}

void ASIBPlayerController::EnterGameMode()
{
	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);
}
