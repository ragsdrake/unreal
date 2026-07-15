// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/SIBWidgetBase.h"
#include "ShipStatusWidget.generated.h"

class UShipStatusComponent;

/** Formats and pushes [Hull Integrity: 85%] / [Autopilot: Disengaged] readouts, delegate-driven. */
UCLASS(Abstract)
class SPACEIDLEBOTANIST_API UShipStatusWidget : public USIBWidgetBase
{
	GENERATED_BODY()

public:
	/** Binds this widget to a ship's status component; pass nullptr to unbind. */
	UFUNCTION(BlueprintCallable, Category = "SIB|UI")
	void InitializeFromShip(UShipStatusComponent* InShipStatus);

	UFUNCTION(BlueprintPure, Category = "SIB|UI")
	static FText FormatHullText(float HullIntegrity);

	UFUNCTION(BlueprintPure, Category = "SIB|UI")
	static FText FormatAutopilotText(bool bEngaged);

protected:
	virtual void UnbindDelegates() override;

	UFUNCTION()
	void HandleHullChanged(float NewHullIntegrity);

	UFUNCTION()
	void HandleAutopilotChanged(bool bEngaged);

	/** Blueprint child assigns the formatted texts to its TextBlocks here. */
	UFUNCTION(BlueprintImplementableEvent, Category = "SIB|UI")
	void OnShipStatusTextChanged(const FText& HullText, const FText& AutopilotText);

	TWeakObjectPtr<UShipStatusComponent> ShipStatus;
};
