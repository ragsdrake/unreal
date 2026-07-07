// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "Hub/GeneLab.h"

#include "Core/IdleEconomySubsystem.h"
#include "Engine/GameInstance.h"

bool AGeneLab::SpliceGenome(FName TargetFloraRow)
{
	UGameInstance* GameInstance = GetGameInstance();
	UIdleEconomySubsystem* Economy = GameInstance ? GameInstance->GetSubsystem<UIdleEconomySubsystem>() : nullptr;

	if (!Economy || TargetFloraRow.IsNone() || !Economy->TrySpend(SpliceCosts))
	{
		return false;
	}

	// Phase 3: apply YieldMultiplierPerSplice to the target flora's yield modifier stack.
	OnGenomeSpliced(TargetFloraRow);
	return true;
}
