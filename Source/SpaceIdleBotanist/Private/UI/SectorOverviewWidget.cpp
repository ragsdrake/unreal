// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "UI/SectorOverviewWidget.h"

#include "Core/IdleEconomySubsystem.h"
#include "Idle/PlanetarySector.h"
#include "Kismet/GameplayStatics.h"

void USectorOverviewWidget::RequestDeployMachine(FName MachineRow, const FTransform& SpawnTransform)
{
	if (APlanetarySector* Sector =
			Cast<APlanetarySector>(UGameplayStatics::GetActorOfClass(this, APlanetarySector::StaticClass())))
	{
		Sector->TryDeployMachine(MachineRow, SpawnTransform);
	}
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
