// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/SIBMachineDef.h"
#include "Interfaces/SIBInteractable.h"
#include "IdleMachineBase.generated.h"

class UResourceYieldComponent;

/**
 * Deployable idle producer ([Deploy Thermal Generator], mineral extractors, flora pods).
 * Stats come from a DT_Machines row; yield accrues purely from timestamp deltas.
 * Blueprint children add meshes and "working" VFX/timelines via OnProductionStateChanged.
 */
UCLASS(Abstract)
class SPACEIDLEBOTANIST_API AIdleMachineBase : public AActor, public ISIBInteractable
{
	GENERATED_BODY()

public:
	AIdleMachineBase();

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
	virtual FText GetInteractionPrompt_Implementation() const override;
	virtual bool CanInteract_Implementation(const APawn* InstigatorPawn) const override;

	/** Credits pending yield to the economy ledger and resets the accrual anchor. */
	UFUNCTION(BlueprintCallable, Category = "SIB|Machine")
	int64 CollectPendingYield();

	UFUNCTION(BlueprintPure, Category = "SIB|Machine")
	int64 GetPendingYield() const;

	/** Must be set before FinishSpawning so BeginPlay can resolve the row. */
	UFUNCTION(BlueprintCallable, Category = "SIB|Machine")
	void SetMachineDefRow(const FDataTableRowHandle& InRow) { MachineDefRow = InRow; }

	/**
	 * Links this actor to its FDeployedMachineRecord in UIdleEconomySubsystem. When linked, the
	 * record is the single accrual authority (prevents double-crediting against offline progress);
	 * INDEX_NONE (hand-placed machines) falls back to the local yield component.
	 */
	UFUNCTION(BlueprintCallable, Category = "SIB|Machine")
	void SetDeployedRecordIndex(int32 InIndex) { DeployedRecordIndex = InIndex; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "SIB|Machine")
	void OnProductionStateChanged(bool bProducing);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Machine", meta = (RowType = "/Script/SpaceIdleBotanist.MachineDef"))
	FDataTableRowHandle MachineDefRow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SIB|Machine")
	TObjectPtr<UResourceYieldComponent> YieldComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SIB|Machine")
	TObjectPtr<USceneComponent> Root;

	/** Row data resolved once at BeginPlay. */
	UPROPERTY(Transient)
	FMachineDef CachedDef;

	UPROPERTY(Transient)
	bool bDefResolved = false;

	UPROPERTY(Transient)
	int32 DeployedRecordIndex = INDEX_NONE;
};
