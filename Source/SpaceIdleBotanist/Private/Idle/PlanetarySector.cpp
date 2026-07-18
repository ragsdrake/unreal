// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "Idle/PlanetarySector.h"

#include "Core/IdleEconomySubsystem.h"
#include "Core/SIBGameInstance.h"
#include "Data/SIBMachineDef.h"
#include "Engine/DataTable.h"
#include "Idle/IdleMachineBase.h"
#include "PCGComponent.h"
#include "Procedural/SIBRandomLibrary.h"
#include "SpaceIdleBotanist.h"

APlanetarySector::APlanetarySector()
{
	PrimaryActorTick.bCanEverTick = false;
}

FRandomStream APlanetarySector::GetSeededStream(FName Salt) const
{
	return USIBRandomLibrary::MakeStream(GenerationSeed, Salt);
}

void APlanetarySector::TriggerGeneration()
{
	for (AActor* PCGActor : ManagedPCGActors)
	{
		if (!PCGActor)
		{
			continue;
		}
		if (UPCGComponent* PCG = PCGActor->FindComponentByClass<UPCGComponent>())
		{
			// Salt by actor name so multiple graphs in one sector diverge deterministically.
			PCG->Seed = USIBRandomLibrary::DeriveSeed(GenerationSeed, PCGActor->GetFName());
			PCG->Generate(/*bForce=*/true);
		}
	}

	OnGenerationTriggered(GenerationSeed);
}

AIdleMachineBase* APlanetarySector::TryDeployMachine(FName MachineRow, const FTransform& SpawnTransform)
{
	USIBGameInstance* SIBGameInstance = Cast<USIBGameInstance>(GetGameInstance());
	UIdleEconomySubsystem* Economy = SIBGameInstance ? SIBGameInstance->GetSubsystem<UIdleEconomySubsystem>() : nullptr;
	const UDataTable* MachineTable = SIBGameInstance ? SIBGameInstance->GetMachineTable() : nullptr;
	const FMachineDef* Def = MachineTable
		? MachineTable->FindRow<FMachineDef>(MachineRow, TEXT("APlanetarySector::TryDeployMachine"))
		: nullptr;

	if (!Economy || !Def)
	{
		return nullptr;
	}

	if (Def->MachineClass.IsNull())
	{
		UE_LOG(LogSIB, Warning, TEXT("Machine row '%s' has no MachineClass assigned."), *MachineRow.ToString());
		return nullptr;
	}

	if (!Economy->TrySpend(Def->BuildCosts))
	{
		return nullptr;
	}

	FDeployedMachineRecord Record;
	Record.MachineRow = MachineRow;
	Record.SectorRow = SectorRowName;
	Record.Transform = SpawnTransform;
	Record.DeployUtcTicks = FDateTime::UtcNow().GetTicks();
	Record.LastCollectUtcTicks = Record.DeployUtcTicks;
	const int32 RecordIndex = Economy->RegisterDeployedMachine(Record);

	AIdleMachineBase* Machine = SpawnMachineInternal(MachineRow, SpawnTransform, RecordIndex);
	if (Machine)
	{
		Economy->OnMachineDeployed.Broadcast(Machine);
	}
	return Machine;
}

void APlanetarySector::RestoreDeployedMachines()
{
	const USIBGameInstance* SIBGameInstance = Cast<USIBGameInstance>(GetGameInstance());
	UIdleEconomySubsystem* Economy = SIBGameInstance ? SIBGameInstance->GetSubsystem<UIdleEconomySubsystem>() : nullptr;
	if (!Economy || SectorRowName.IsNone())
	{
		return;
	}

	const TArray<FDeployedMachineRecord>& Records = Economy->GetDeployedMachines();
	for (int32 Index = 0; Index < Records.Num(); ++Index)
	{
		if (Records[Index].SectorRow == SectorRowName)
		{
			SpawnMachineInternal(Records[Index].MachineRow, Records[Index].Transform, Index);
		}
	}
}

AIdleMachineBase* APlanetarySector::SpawnMachineInternal(FName MachineRow, const FTransform& SpawnTransform, int32 DeployedRecordIndex)
{
	USIBGameInstance* SIBGameInstance = Cast<USIBGameInstance>(GetGameInstance());
	UDataTable* MachineTable = SIBGameInstance ? SIBGameInstance->GetMachineTable() : nullptr;
	const FMachineDef* Def = MachineTable
		? MachineTable->FindRow<FMachineDef>(MachineRow, TEXT("APlanetarySector::SpawnMachineInternal"))
		: nullptr;
	UClass* MachineClass = Def ? Def->MachineClass.LoadSynchronous() : nullptr;
	if (!MachineClass)
	{
		UE_LOG(LogSIB, Warning, TEXT("Cannot spawn machine row '%s': class unresolved."), *MachineRow.ToString());
		return nullptr;
	}

	AIdleMachineBase* Machine = GetWorld()->SpawnActorDeferred<AIdleMachineBase>(MachineClass, SpawnTransform);
	if (!Machine)
	{
		return nullptr;
	}

	FDataTableRowHandle RowHandle;
	RowHandle.DataTable = MachineTable;
	RowHandle.RowName = MachineRow;
	Machine->SetMachineDefRow(RowHandle);
	Machine->SetDeployedRecordIndex(DeployedRecordIndex);
	Machine->FinishSpawning(SpawnTransform);

	RegisterMachine(Machine);
	return Machine;
}

void APlanetarySector::RegisterMachine(AIdleMachineBase* Machine)
{
	if (Machine)
	{
		RegisteredMachines.AddUnique(Machine);
	}
}

void APlanetarySector::UnregisterMachine(AIdleMachineBase* Machine)
{
	RegisteredMachines.Remove(Machine);
}
