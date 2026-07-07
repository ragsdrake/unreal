// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "Core/SIBGameModeSector.h"

#include "Core/SIBPlayerController.h"

ASIBGameModeSector::ASIBGameModeSector()
{
	PlayerControllerClass = ASIBPlayerController::StaticClass();
}

void ASIBGameModeSector::StartPlay()
{
	Super::StartPlay();
	InitializeSectorGeneration();
}

void ASIBGameModeSector::InitializeSectorGeneration()
{
	// Phase 3: locate the level's APlanetarySector, restore its persisted seed from
	// USIBSaveGame.SectorSeeds (never re-roll - RISK-002), then trigger PCG generation.
}
