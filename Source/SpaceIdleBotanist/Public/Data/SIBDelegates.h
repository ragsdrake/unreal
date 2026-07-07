// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SIBDelegates.generated.h"

class AIdleMachineBase;

UENUM(BlueprintType)
enum class EJumpDriveState : uint8
{
	Idle,
	Charging,
	Jumping,
	Arrived
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnResourceLedgerUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHullChanged, float, NewHullIntegrity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAutopilotChanged, bool, bEngaged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJumpStateChanged, EJumpDriveState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMachineDeployed, AIdleMachineBase*, Machine);
