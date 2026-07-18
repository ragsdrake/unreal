// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/SIBDeployedMachineRecord.h"
#include "ResourceYieldComponent.generated.h"

/**
 * Timestamp-delta accrual math, shared by machines and harvestable flora.
 * Yield is a pure function of (UtcNow - LastCollectUtc), clamped to [0, MaxOfflineHours]
 * against clock tampering and unbounded offline gains (RISK-001/RISK-003). No Tick.
 */
UCLASS(ClassGroup = (SIB), meta = (BlueprintSpawnableComponent))
class SPACEIDLEBOTANIST_API UResourceYieldComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UResourceYieldComponent();

	UFUNCTION(BlueprintPure, Category = "SIB|Yield")
	int64 ComputePendingYield(double YieldPerSecond, float MaxOfflineHours) const;

	/** Single source of truth for the accrual formula; also used by UIdleEconomySubsystem for offline records. */
	UFUNCTION(BlueprintPure, Category = "SIB|Yield")
	static int64 ComputeYieldFromTicks(int64 InLastCollectUtcTicks, double YieldPerSecond, float MaxOfflineHours);

	/** Resets the accrual anchor to now. Call after crediting the ledger. */
	UFUNCTION(BlueprintCallable, Category = "SIB|Yield")
	void MarkCollected();

	UFUNCTION(BlueprintCallable, Category = "SIB|Yield")
	void RestoreFromRecord(const FDeployedMachineRecord& Record);

	UFUNCTION(BlueprintPure, Category = "SIB|Yield")
	int64 GetLastCollectUtcTicks() const { return LastCollectUtcTicks; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(Transient)
	int64 LastCollectUtcTicks = 0;
};
