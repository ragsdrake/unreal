// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "Hub/ShipStatusComponent.h"

UShipStatusComponent::UShipStatusComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UShipStatusComponent::SetHullIntegrity(float NewValue)
{
	const float Clamped = FMath::Clamp(NewValue, 0.0f, MaxHullIntegrity);
	if (!FMath::IsNearlyEqual(Clamped, HullIntegrity))
	{
		HullIntegrity = Clamped;
		OnHullChanged.Broadcast(HullIntegrity);
	}
}

void UShipStatusComponent::ApplyHullDamage(float Amount)
{
	SetHullIntegrity(HullIntegrity - Amount);
}

void UShipStatusComponent::SetAutopilotEngaged(bool bEngaged)
{
	if (bAutopilotEngaged != bEngaged)
	{
		bAutopilotEngaged = bEngaged;
		OnAutopilotChanged.Broadcast(bAutopilotEngaged);
	}
}
