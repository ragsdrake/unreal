// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Data/SIBDeployedMachineRecord.h"
#include "SIBSaveGame.generated.h"

/** Full persistent state. Offline progress is reconstructed from the UTC tick timestamps stored here. */
UCLASS()
class SPACEIDLEBOTANIST_API USIBSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	/** Resource row name -> owned amount. int64 so week-long offline deltas cannot overflow (RISK-001). */
	UPROPERTY(BlueprintReadOnly, Category = "SIB|Save")
	TMap<FName, int64> ResourceLedger;

	UPROPERTY(BlueprintReadOnly, Category = "SIB|Save")
	TArray<FDeployedMachineRecord> DeployedMachines;

	/** Sector row name -> persisted PCG seed. Written once on first visit, never re-rolled (RISK-002). */
	UPROPERTY(BlueprintReadOnly, Category = "SIB|Save")
	TMap<FName, int32> SectorSeeds;

	UPROPERTY(BlueprintReadOnly, Category = "SIB|Save")
	float HullIntegrity = 100.0f;

	UPROPERTY(BlueprintReadOnly, Category = "SIB|Save")
	bool bAutopilotEngaged = false;

	/** FDateTime::UtcNow().GetTicks() at last save; anchor for offline progress. */
	UPROPERTY(BlueprintReadOnly, Category = "SIB|Save")
	int64 LastSessionUtcTicks = 0;
};
