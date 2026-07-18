// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SIBMachineDef.generated.h"

class AIdleMachineBase;
class UCurveFloat;

/** One row per deployable machine type in DT_Machines. */
USTRUCT(BlueprintType)
struct SPACEIDLEBOTANIST_API FMachineDef : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Machine")
	FText DisplayName;

	/** Actor spawned on deploy; a Blueprint child of AIdleMachineBase. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Machine")
	TSoftClassPtr<AIdleMachineBase> MachineClass;

	/** Row name in DT_Resources this machine produces. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Machine")
	FName YieldResourceRow;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Machine", meta = (ClampMin = "0.0"))
	double YieldPerSecond = 0.0;

	/** Resource row -> amount required to deploy one machine. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Machine")
	TMap<FName, int64> BuildCosts;

	/** Yield multiplier sampled by upgrade level. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Machine")
	TSoftObjectPtr<UCurveFloat> UpgradeCurve;

	/** Offline accrual cap; timestamp deltas beyond this are clamped (RISK-001/RISK-003). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Machine", meta = (ClampMin = "0.0"))
	float MaxOfflineHours = 12.0f;
};
