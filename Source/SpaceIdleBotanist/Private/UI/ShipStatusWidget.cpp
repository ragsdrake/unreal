// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "UI/ShipStatusWidget.h"

#include "Hub/ShipStatusComponent.h"

void UShipStatusWidget::InitializeFromShip(UShipStatusComponent* InShipStatus)
{
	if (UShipStatusComponent* Previous = ShipStatus.Get())
	{
		Previous->OnHullChanged.RemoveDynamic(this, &UShipStatusWidget::HandleHullChanged);
		Previous->OnAutopilotChanged.RemoveDynamic(this, &UShipStatusWidget::HandleAutopilotChanged);
	}

	ShipStatus = InShipStatus;

	if (InShipStatus)
	{
		InShipStatus->OnHullChanged.AddDynamic(this, &UShipStatusWidget::HandleHullChanged);
		InShipStatus->OnAutopilotChanged.AddDynamic(this, &UShipStatusWidget::HandleAutopilotChanged);
		HandleHullChanged(InShipStatus->GetHullIntegrity());
	}
}

FText UShipStatusWidget::FormatHullText(float HullIntegrity)
{
	FFormatNamedArguments Args;
	Args.Add(TEXT("Percent"), FText::AsNumber(FMath::RoundToInt(HullIntegrity)));
	return FText::Format(NSLOCTEXT("SIB", "HullIntegrityFormat", "[Hull Integrity: {Percent}%]"), Args);
}

FText UShipStatusWidget::FormatAutopilotText(bool bEngaged)
{
	return bEngaged
		? NSLOCTEXT("SIB", "AutopilotEngaged", "[Autopilot: Engaged]")
		: NSLOCTEXT("SIB", "AutopilotDisengaged", "[Autopilot: Disengaged]");
}

void UShipStatusWidget::UnbindDelegates()
{
	InitializeFromShip(nullptr);
	Super::UnbindDelegates();
}

void UShipStatusWidget::HandleHullChanged(float NewHullIntegrity)
{
	const bool bAutopilot = ShipStatus.IsValid() && ShipStatus->IsAutopilotEngaged();
	OnShipStatusTextChanged(FormatHullText(NewHullIntegrity), FormatAutopilotText(bAutopilot));
}

void UShipStatusWidget::HandleAutopilotChanged(bool bEngaged)
{
	const float Hull = ShipStatus.IsValid() ? ShipStatus->GetHullIntegrity() : 0.0f;
	OnShipStatusTextChanged(FormatHullText(Hull), FormatAutopilotText(bEngaged));
}
