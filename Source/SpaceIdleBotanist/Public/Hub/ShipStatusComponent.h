// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/SIBDelegates.h"
#include "ShipStatusComponent.generated.h"

/** Hull integrity and autopilot state. Purely event-driven: setters broadcast, HUD listens. */
UCLASS(ClassGroup = (SIB), meta = (BlueprintSpawnableComponent))
class SPACEIDLEBOTANIST_API UShipStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UShipStatusComponent();

	UFUNCTION(BlueprintCallable, Category = "SIB|Ship")
	void SetHullIntegrity(float NewValue);

	UFUNCTION(BlueprintCallable, Category = "SIB|Ship")
	void ApplyHullDamage(float Amount);

	UFUNCTION(BlueprintCallable, Category = "SIB|Ship")
	void SetAutopilotEngaged(bool bEngaged);

	UFUNCTION(BlueprintPure, Category = "SIB|Ship")
	float GetHullIntegrity() const { return HullIntegrity; }

	UFUNCTION(BlueprintPure, Category = "SIB|Ship")
	bool IsAutopilotEngaged() const { return bAutopilotEngaged; }

	UPROPERTY(BlueprintAssignable, Category = "SIB|Ship")
	FOnHullChanged OnHullChanged;

	UPROPERTY(BlueprintAssignable, Category = "SIB|Ship")
	FOnAutopilotChanged OnAutopilotChanged;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Ship", meta = (ClampMin = "0.0"))
	float MaxHullIntegrity = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Ship", meta = (ClampMin = "0.0"))
	float HullIntegrity = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Ship")
	bool bAutopilotEngaged = false;
};
