// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "Core/IdleEconomySubsystem.h"

#include "Core/SIBGameInstance.h"
#include "Core/SIBSaveGame.h"
#include "Data/SIBMachineDef.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "Idle/ResourceYieldComponent.h"
#include "TimerManager.h"

void UIdleEconomySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (HeartbeatIntervalSeconds > 0.0f)
		{
			GameInstance->GetTimerManager().SetTimer(
				HeartbeatHandle, this, &UIdleEconomySubsystem::HandleHeartbeat, HeartbeatIntervalSeconds, true);
		}
	}
}

void UIdleEconomySubsystem::Deinitialize()
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		GameInstance->GetTimerManager().ClearTimer(HeartbeatHandle);
	}
	Super::Deinitialize();
}

void UIdleEconomySubsystem::RecalculateOfflineProgress()
{
	USIBGameInstance* SIBGameInstance = Cast<USIBGameInstance>(GetGameInstance());
	USIBSaveGame* Save = SIBGameInstance ? SIBGameInstance->LoadOrCreateSaveGame() : nullptr;
	if (!Save)
	{
		return;
	}

	RuntimeLedger = Save->ResourceLedger;
	DeployedMachines = Save->DeployedMachines;

	const int64 NowTicks = FDateTime::UtcNow().GetTicks();
	for (FDeployedMachineRecord& Record : DeployedMachines)
	{
		if (const FMachineDef* Def = ResolveMachineDef(Record.MachineRow))
		{
			const int64 Pending = UResourceYieldComponent::ComputeYieldFromTicks(
				Record.LastCollectUtcTicks, Def->YieldPerSecond, Def->MaxOfflineHours);
			if (Pending > 0)
			{
				RuntimeLedger.FindOrAdd(Def->YieldResourceRow) += Pending;
			}
			Record.LastCollectUtcTicks = NowTicks;
		}
	}

	OnResourceLedgerUpdated.Broadcast();
}

int64 UIdleEconomySubsystem::GetResourceAmount(FName ResourceRow) const
{
	const int64* Found = RuntimeLedger.Find(ResourceRow);
	return Found ? *Found : 0;
}

void UIdleEconomySubsystem::AddResource(FName ResourceRow, int64 Amount)
{
	if (ResourceRow.IsNone() || Amount == 0)
	{
		return;
	}

	RuntimeLedger.FindOrAdd(ResourceRow) += Amount;
	OnResourceLedgerUpdated.Broadcast();
}

bool UIdleEconomySubsystem::TrySpend(const TMap<FName, int64>& Costs)
{
	for (const TPair<FName, int64>& Cost : Costs)
	{
		if (GetResourceAmount(Cost.Key) < Cost.Value)
		{
			return false;
		}
	}

	for (const TPair<FName, int64>& Cost : Costs)
	{
		RuntimeLedger.FindOrAdd(Cost.Key) -= Cost.Value;
	}

	OnResourceLedgerUpdated.Broadcast();
	return true;
}

int64 UIdleEconomySubsystem::GetPendingYield(const FDeployedMachineRecord& Record) const
{
	const FMachineDef* Def = ResolveMachineDef(Record.MachineRow);
	return Def
		? UResourceYieldComponent::ComputeYieldFromTicks(Record.LastCollectUtcTicks, Def->YieldPerSecond, Def->MaxOfflineHours)
		: 0;
}

int64 UIdleEconomySubsystem::CollectYield(int32 DeployedMachineIndex)
{
	if (!DeployedMachines.IsValidIndex(DeployedMachineIndex))
	{
		return 0;
	}

	FDeployedMachineRecord& Record = DeployedMachines[DeployedMachineIndex];
	const FMachineDef* Def = ResolveMachineDef(Record.MachineRow);
	if (!Def)
	{
		return 0;
	}

	const int64 Pending = UResourceYieldComponent::ComputeYieldFromTicks(
		Record.LastCollectUtcTicks, Def->YieldPerSecond, Def->MaxOfflineHours);
	Record.LastCollectUtcTicks = FDateTime::UtcNow().GetTicks();

	if (Pending > 0)
	{
		RuntimeLedger.FindOrAdd(Def->YieldResourceRow) += Pending;
		OnResourceLedgerUpdated.Broadcast();
	}
	return Pending;
}

int32 UIdleEconomySubsystem::RegisterDeployedMachine(const FDeployedMachineRecord& Record)
{
	return DeployedMachines.Add(Record);
}

void UIdleEconomySubsystem::FlushToSave(USIBSaveGame* Save) const
{
	if (Save)
	{
		Save->ResourceLedger = RuntimeLedger;
		Save->DeployedMachines = DeployedMachines;
	}
}

const FMachineDef* UIdleEconomySubsystem::ResolveMachineDef(FName MachineRow) const
{
	const USIBGameInstance* SIBGameInstance = Cast<USIBGameInstance>(GetGameInstance());
	const UDataTable* Table = SIBGameInstance ? SIBGameInstance->GetMachineTable() : nullptr;
	return Table ? Table->FindRow<FMachineDef>(MachineRow, TEXT("UIdleEconomySubsystem::ResolveMachineDef")) : nullptr;
}

void UIdleEconomySubsystem::HandleHeartbeat()
{
	// Timer-driven UI refresh pulse; pending yields are recomputed from timestamps by listeners.
	OnResourceLedgerUpdated.Broadcast();
}
