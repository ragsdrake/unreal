// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "Idle/PlanetarySector.h"

#include "Idle/IdleMachineBase.h"
#include "Procedural/SIBRandomLibrary.h"

APlanetarySector::APlanetarySector()
{
	PrimaryActorTick.bCanEverTick = false;
}

FRandomStream APlanetarySector::GetSeededStream(FName Salt) const
{
	return USIBRandomLibrary::MakeStream(GenerationSeed, Salt);
}

void APlanetarySector::RegisterMachine(AIdleMachineBase* Machine)
{
	if (Machine)
	{
		RegisteredMachines.AddUnique(Machine);
		// Phase 3: forward a FDeployedMachineRecord to the economy subsystem for persistence.
	}
}

void APlanetarySector::UnregisterMachine(AIdleMachineBase* Machine)
{
	RegisteredMachines.Remove(Machine);
}
