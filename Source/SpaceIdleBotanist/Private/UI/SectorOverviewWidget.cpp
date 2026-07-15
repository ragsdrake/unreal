// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "UI/SectorOverviewWidget.h"

#include "Core/IdleEconomySubsystem.h"

void USectorOverviewWidget::RequestDeployMachine(FName MachineRow)
{
	// Phase 3: validate MachineRow against the sector's AllowedMachineRows, spend
	// FMachineDef.BuildCosts via TrySpend, then spawn the machine and register it
	// with the APlanetarySector.
}

void USectorOverviewWidget::SetAutomatedMineralExport(bool bEnabled)
{
	if (bAutomatedMineralExport != bEnabled)
	{
		bAutomatedMineralExport = bEnabled;
		// Phase 3: route exportable resources (FResourceDef.bIsExportable) to the hub ledger on a timer.
		OnLedgerRefreshed();
	}
}

FText USectorOverviewWidget::GetDeployThermalGeneratorLabel()
{
	return NSLOCTEXT("SIB", "DeployThermalGenerator", "[Deploy Thermal Generator]");
}

FText USectorOverviewWidget::GetAutomatedMineralExportLabel()
{
	return NSLOCTEXT("SIB", "AutomatedMineralExport", "[Automated Mineral Export]");
}

void USectorOverviewWidget::BindDelegates()
{
	Super::BindDelegates();
	if (UIdleEconomySubsystem* Economy = GetEconomySubsystem())
	{
		Economy->OnResourceLedgerUpdated.AddDynamic(this, &USectorOverviewWidget::HandleResourceLedgerUpdated);
	}
}

void USectorOverviewWidget::UnbindDelegates()
{
	if (UIdleEconomySubsystem* Economy = GetEconomySubsystem())
	{
		Economy->OnResourceLedgerUpdated.RemoveDynamic(this, &USectorOverviewWidget::HandleResourceLedgerUpdated);
	}
	Super::UnbindDelegates();
}

void USectorOverviewWidget::HandleResourceLedgerUpdated()
{
	OnLedgerRefreshed();
}
