// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "Hub/PilotConsole.h"

#include "Core/SectorTravelSubsystem.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"

TArray<FName> APilotConsole::GetAvailableDestinations() const
{
	if (const UDataTable* Table = SectorTable.LoadSynchronous())
	{
		return Table->GetRowNames();
	}
	return TArray<FName>();
}

bool APilotConsole::InitializeJumpDrive(FName DestinationSectorRow)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (USectorTravelSubsystem* Travel = GameInstance->GetSubsystem<USectorTravelSubsystem>())
		{
			return Travel->RequestJump(DestinationSectorRow);
		}
	}
	return false;
}

void APilotConsole::Interact_Implementation(APawn* InstigatorPawn)
{
	Super::Interact_Implementation(InstigatorPawn);
	OnOpenNavigationUI(InstigatorPawn);
}
