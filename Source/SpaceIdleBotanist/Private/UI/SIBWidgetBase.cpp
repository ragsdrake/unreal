// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "UI/SIBWidgetBase.h"

#include "Core/IdleEconomySubsystem.h"
#include "Core/SectorTravelSubsystem.h"
#include "Engine/GameInstance.h"

void USIBWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();
	BindDelegates();
}

void USIBWidgetBase::NativeDestruct()
{
	UnbindDelegates();
	Super::NativeDestruct();
}

void USIBWidgetBase::BindDelegates()
{
}

void USIBWidgetBase::UnbindDelegates()
{
}

UIdleEconomySubsystem* USIBWidgetBase::GetEconomySubsystem() const
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		return GameInstance->GetSubsystem<UIdleEconomySubsystem>();
	}
	return nullptr;
}

USectorTravelSubsystem* USIBWidgetBase::GetTravelSubsystem() const
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		return GameInstance->GetSubsystem<USectorTravelSubsystem>();
	}
	return nullptr;
}
