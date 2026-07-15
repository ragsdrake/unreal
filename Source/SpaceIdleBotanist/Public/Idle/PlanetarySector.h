// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "PlanetarySector.generated.h"

class AIdleMachineBase;

/** One per sector level: owns the persisted generation seed and the deployed-machine registry. */
UCLASS()
class SPACEIDLEBOTANIST_API APlanetarySector : public AInfo
{
	GENERATED_BODY()

public:
	APlanetarySector();

	/** Deterministic stream for a generation pass; salt separates consumers (asteroids vs. flora). */
	UFUNCTION(BlueprintPure, Category = "SIB|Sector")
	FRandomStream GetSeededStream(FName Salt) const;

	UFUNCTION(BlueprintCallable, Category = "SIB|Sector")
	void RegisterMachine(AIdleMachineBase* Machine);

	UFUNCTION(BlueprintCallable, Category = "SIB|Sector")
	void UnregisterMachine(AIdleMachineBase* Machine);

	UFUNCTION(BlueprintPure, Category = "SIB|Sector")
	int32 GetGenerationSeed() const { return GenerationSeed; }

	/** Save-load flow only; a live sector's seed must never be re-rolled (RISK-002). */
	UFUNCTION(BlueprintCallable, Category = "SIB|Sector")
	void SetGenerationSeed(int32 NewSeed) { GenerationSeed = NewSeed; }

	UFUNCTION(BlueprintPure, Category = "SIB|Sector")
	FName GetSectorRowName() const { return SectorRowName; }

protected:
	/** DT_Sectors row this level represents. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Sector")
	FName SectorRowName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Sector")
	int32 GenerationSeed = 0;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "SIB|Sector")
	TArray<TObjectPtr<AIdleMachineBase>> RegisteredMachines;
};
