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

	/** Pushes the sector seed into every managed PCG component (salted per actor) and regenerates. */
	UFUNCTION(BlueprintCallable, Category = "SIB|Sector")
	void TriggerGeneration();

	/** Spends FMachineDef.BuildCosts, spawns the machine, and persists a deploy record. Returns nullptr on failure. */
	UFUNCTION(BlueprintCallable, Category = "SIB|Sector")
	AIdleMachineBase* TryDeployMachine(FName MachineRow, const FTransform& SpawnTransform);

	/** Respawns this sector's machines from the save records (no costs charged). */
	UFUNCTION(BlueprintCallable, Category = "SIB|Sector")
	void RestoreDeployedMachines();

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
	/** Extra generation hook for Blueprint children (skybox variation, ambience seeds, ...). */
	UFUNCTION(BlueprintImplementableEvent, Category = "SIB|Sector")
	void OnGenerationTriggered(int32 Seed);

	AIdleMachineBase* SpawnMachineInternal(FName MachineRow, const FTransform& SpawnTransform, int32 DeployedRecordIndex);

	/** Level actors carrying UPCGComponents that this sector seeds and regenerates. */
	UPROPERTY(EditInstanceOnly, Category = "SIB|Sector")
	TArray<TObjectPtr<AActor>> ManagedPCGActors;

	/** DT_Sectors row this level represents. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Sector")
	FName SectorRowName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Sector")
	int32 GenerationSeed = 0;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "SIB|Sector")
	TArray<TObjectPtr<AIdleMachineBase>> RegisteredMachines;
};
