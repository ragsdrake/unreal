// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/TimerHandle.h"
#include "Data/SIBDelegates.h"
#include "Data/SIBDeployedMachineRecord.h"
#include "IdleEconomySubsystem.generated.h"

/**
 * The idle engine. Owns the runtime resource ledger; all accrual is computed from
 * FDateTime UTC timestamp deltas on demand. A low-rate FTimerManager heartbeat only
 * refreshes UI listeners - there is no Tick anywhere in the economy path.
 */
UCLASS(Config = Game)
class SPACEIDLEBOTANIST_API UIdleEconomySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Applies clamped timestamp deltas for every deployed machine after a save is loaded. */
	UFUNCTION(BlueprintCallable, Category = "SIB|Economy")
	void RecalculateOfflineProgress();

	UFUNCTION(BlueprintPure, Category = "SIB|Economy")
	int64 GetResourceAmount(FName ResourceRow) const;

	UFUNCTION(BlueprintCallable, Category = "SIB|Economy")
	void AddResource(FName ResourceRow, int64 Amount);

	/** Atomically checks and deducts all costs; returns false (deducting nothing) if any is unaffordable. */
	UFUNCTION(BlueprintCallable, Category = "SIB|Economy")
	bool TrySpend(const TMap<FName, int64>& Costs);

	UFUNCTION(BlueprintPure, Category = "SIB|Economy")
	int64 GetPendingYield(const FDeployedMachineRecord& Record) const;

	UFUNCTION(BlueprintCallable, Category = "SIB|Economy")
	int64 CollectYield(int32 DeployedMachineIndex);

	UPROPERTY(BlueprintAssignable, Category = "SIB|Economy")
	FOnResourceLedgerUpdated OnResourceLedgerUpdated;

protected:
	void HandleHeartbeat();

	/** UI refresh cadence, seconds. Set in DefaultGame.ini under [/Script/SpaceIdleBotanist.IdleEconomySubsystem]. */
	UPROPERTY(Config)
	float HeartbeatIntervalSeconds = 1.0f;

	UPROPERTY(Transient)
	TMap<FName, int64> RuntimeLedger;

	FTimerHandle HeartbeatHandle;
};
