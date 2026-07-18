// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "Idle/ResourceYieldComponent.h"

UResourceYieldComponent::UResourceYieldComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UResourceYieldComponent::BeginPlay()
{
	Super::BeginPlay();

	if (LastCollectUtcTicks <= 0)
	{
		MarkCollected();
	}
}

int64 UResourceYieldComponent::ComputePendingYield(double YieldPerSecond, float MaxOfflineHours) const
{
	return ComputeYieldFromTicks(LastCollectUtcTicks, YieldPerSecond, MaxOfflineHours);
}

int64 UResourceYieldComponent::ComputeYieldFromTicks(int64 InLastCollectUtcTicks, double YieldPerSecond, float MaxOfflineHours)
{
	if (InLastCollectUtcTicks <= 0 || YieldPerSecond <= 0.0)
	{
		return 0;
	}

	const FTimespan Delta = FDateTime::UtcNow() - FDateTime(InLastCollectUtcTicks);
	// Negative deltas (OS clock moved backward) and over-cap offline stretches are clamped (RISK-003).
	const double MaxSeconds = static_cast<double>(MaxOfflineHours) * FTimespan::FromHours(1.0).GetTotalSeconds();
	const double Seconds = FMath::Clamp(Delta.GetTotalSeconds(), 0.0, MaxSeconds);

	return static_cast<int64>(Seconds * YieldPerSecond);
}

void UResourceYieldComponent::MarkCollected()
{
	LastCollectUtcTicks = FDateTime::UtcNow().GetTicks();
}

void UResourceYieldComponent::RestoreFromRecord(const FDeployedMachineRecord& Record)
{
	LastCollectUtcTicks = Record.LastCollectUtcTicks;
}
