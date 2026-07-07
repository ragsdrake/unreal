// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "Core/SIBGameModeHub.h"

#include "Core/SIBPlayerController.h"

ASIBGameModeHub::ASIBGameModeHub()
{
	PlayerControllerClass = ASIBPlayerController::StaticClass();
}
