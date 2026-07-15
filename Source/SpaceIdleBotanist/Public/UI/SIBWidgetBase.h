// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SIBWidgetBase.generated.h"

class UIdleEconomySubsystem;
class USectorTravelSubsystem;

/**
 * C++ base for all game widgets: subsystem access and delegate lifecycle.
 * Strictly event-driven - no NativeTick logic anywhere in the widget tree (PERF-002).
 * WBP_ Blueprint children own all layout, styling, and animation.
 */
UCLASS(Abstract)
class SPACEIDLEBOTANIST_API USIBWidgetBase : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/** Override to AddDynamic/RemoveDynamic subsystem delegates; called from construct/destruct. */
	virtual void BindDelegates();
	virtual void UnbindDelegates();

	UFUNCTION(BlueprintPure, Category = "SIB|UI")
	UIdleEconomySubsystem* GetEconomySubsystem() const;

	UFUNCTION(BlueprintPure, Category = "SIB|UI")
	USectorTravelSubsystem* GetTravelSubsystem() const;
};
