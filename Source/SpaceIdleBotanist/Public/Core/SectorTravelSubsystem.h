// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/TimerHandle.h"
#include "Data/SIBDelegates.h"
#include "SectorTravelSubsystem.generated.h"

/** Hub-to-spoke travel: event-driven [Initialize Jump Drive] state machine (Idle -> Charging -> Jumping -> Arrived). */
UCLASS(Config = Game)
class SPACEIDLEBOTANIST_API USectorTravelSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** Starts the jump sequence toward a DT_Sectors row. Fails while a jump is already in progress. */
	UFUNCTION(BlueprintCallable, Category = "SIB|Travel")
	bool RequestJump(FName SectorRow);

	UFUNCTION(BlueprintPure, Category = "SIB|Travel")
	EJumpDriveState GetJumpState() const { return JumpState; }

	UFUNCTION(BlueprintPure, Category = "SIB|Travel")
	FName GetPendingSectorRow() const { return PendingSectorRow; }

	/** Called by ASIBGameModeSector::StartPlay once the destination level is up. */
	void NotifyArrivedInSector();

	UPROPERTY(BlueprintAssignable, Category = "SIB|Travel")
	FOnJumpStateChanged OnJumpStateChanged;

protected:
	void SetJumpState(EJumpDriveState NewState);
	void HandleChargeComplete();
	void HandleTravelComplete();

	/** Charge duration, seconds. Set in DefaultGame.ini under [/Script/SpaceIdleBotanist.SectorTravelSubsystem]. */
	UPROPERTY(Config)
	float JumpChargeSeconds = 3.0f;

	UPROPERTY(Transient)
	EJumpDriveState JumpState = EJumpDriveState::Idle;

	UPROPERTY(Transient)
	FName PendingSectorRow;

	FTimerHandle JumpTimerHandle;
};
