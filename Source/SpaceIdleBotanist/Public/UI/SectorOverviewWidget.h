// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/SIBWidgetBase.h"
#include "SectorOverviewWidget.generated.h"

/**
 * Idle-factory overview for a sector: deployed machines, pending yields,
 * [Deploy Thermal Generator] and [Automated Mineral Export] actions.
 * Refreshes only on ledger delegate broadcasts - never polls (PERF-002).
 */
UCLASS(Abstract)
class SPACEIDLEBOTANIST_API USectorOverviewWidget : public USIBWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "SIB|UI")
	void RequestDeployMachine(FName MachineRow, const FTransform& SpawnTransform);

	UFUNCTION(BlueprintCallable, Category = "SIB|UI")
	void SetAutomatedMineralExport(bool bEnabled);

	UFUNCTION(BlueprintPure, Category = "SIB|UI")
	bool IsAutomatedMineralExportEnabled() const { return bAutomatedMineralExport; }

	UFUNCTION(BlueprintPure, Category = "SIB|UI")
	static FText GetDeployThermalGeneratorLabel();

	UFUNCTION(BlueprintPure, Category = "SIB|UI")
	static FText GetAutomatedMineralExportLabel();

protected:
	virtual void BindDelegates() override;
	virtual void UnbindDelegates() override;

	UFUNCTION()
	void HandleResourceLedgerUpdated();

	/** Blueprint child rebuilds its machine/resource list views here. */
	UFUNCTION(BlueprintImplementableEvent, Category = "SIB|UI")
	void OnLedgerRefreshed();

	UPROPERTY(Transient, BlueprintReadOnly, Category = "SIB|UI")
	bool bAutomatedMineralExport = false;
};
