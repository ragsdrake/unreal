// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "Core/SIBGameModeSector.h"

#include "Core/SIBGameInstance.h"
#include "Core/SIBPlayerController.h"
#include "Core/SIBSaveGame.h"
#include "Core/SectorTravelSubsystem.h"
#include "Idle/PlanetarySector.h"
#include "Kismet/GameplayStatics.h"
#include "Procedural/SIBRandomLibrary.h"
#include "SpaceIdleBotanist.h"

ASIBGameModeSector::ASIBGameModeSector()
{
	PlayerControllerClass = ASIBPlayerController::StaticClass();
}

void ASIBGameModeSector::StartPlay()
{
	Super::StartPlay();

	InitializeSectorGeneration();

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (USectorTravelSubsystem* Travel = GameInstance->GetSubsystem<USectorTravelSubsystem>())
		{
			Travel->NotifyArrivedInSector();
		}
	}
}

void ASIBGameModeSector::InitializeSectorGeneration()
{
	APlanetarySector* Sector =
		Cast<APlanetarySector>(UGameplayStatics::GetActorOfClass(this, APlanetarySector::StaticClass()));
	if (!Sector)
	{
		UE_LOG(LogSIB, Warning, TEXT("No APlanetarySector placed in '%s'; skipping generation."),
			*GetWorld()->GetName());
		return;
	}

	USIBGameInstance* SIBGameInstance = Cast<USIBGameInstance>(GetGameInstance());
	USIBSaveGame* Save = SIBGameInstance ? SIBGameInstance->LoadOrCreateSaveGame() : nullptr;
	const FName SectorRow = Sector->GetSectorRowName();

	if (Save && !SectorRow.IsNone())
	{
		if (const int32* ExistingSeed = Save->SectorSeeds.Find(SectorRow))
		{
			// Restore, never re-roll: keeps PCG geometry aligned with deployed machines (RISK-002).
			Sector->SetGenerationSeed(*ExistingSeed);
		}
		else
		{
			// First visit in this save: time-entropy seed, unique per save, persisted forever after.
			const int32 FreshSeed = USIBRandomLibrary::DeriveSeed(
				static_cast<int32>(FDateTime::UtcNow().GetTicks() & 0x7FFFFFFF), SectorRow);
			Sector->SetGenerationSeed(FreshSeed);
			Save->SectorSeeds.Add(SectorRow, FreshSeed);
		}
	}

	Sector->TriggerGeneration();
	Sector->RestoreDeployedMachines();
}
