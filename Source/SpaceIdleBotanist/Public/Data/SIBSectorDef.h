// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SIBSectorDef.generated.h"

class USectorGenParams;
class UWorld;

/** One row per planetary sector (spoke) in DT_Sectors: Earth Orbit, Mars Surface, Asteroid Belt. */
USTRUCT(BlueprintType)
struct SPACEIDLEBOTANIST_API FSectorDef : public FTableRowBase
{
	GENERATED_BODY()

	/** Shown in navigation as [Destination: <DisplayName>]. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Sector")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Sector")
	TSoftObjectPtr<UWorld> LevelRef;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Sector")
	TSoftObjectPtr<USectorGenParams> GenParams;

	/** Fallback seed for a fresh save; the per-save seed is persisted in USIBSaveGame (RISK-002). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Sector")
	int32 BaseSeed = 0;

	/** DT_Machines rows deployable in this sector. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Sector")
	TArray<FName> AllowedMachineRows;
};
