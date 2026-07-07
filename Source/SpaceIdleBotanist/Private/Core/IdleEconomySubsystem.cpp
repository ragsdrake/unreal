// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "Core/IdleEconomySubsystem.h"

#include "Engine/GameInstance.h"
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
	// Phase 3: for each FDeployedMachineRecord in the active USIBSaveGame, resolve its
	// FMachineDef row, compute the clamped timestamp delta since LastCollectUtcTicks,
	// credit the ledger, and advance the record's timestamp.
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
	// Phase 3: resolve Record.MachineRow to FMachineDef and reuse the
	// UResourceYieldComponent clamped-delta math against Record.LastCollectUtcTicks.
	return 0;
}

int64 UIdleEconomySubsystem::CollectYield(int32 DeployedMachineIndex)
{
	// Phase 3: credit GetPendingYield for the record at this index, advance its
	// LastCollectUtcTicks, and broadcast the ledger update.
	return 0;
}

void UIdleEconomySubsystem::HandleHeartbeat()
{
	// Timer-driven UI refresh pulse; pending yields are recomputed from timestamps by listeners.
	OnResourceLedgerUpdated.Broadcast();
}
