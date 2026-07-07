// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "Core/SectorTravelSubsystem.h"

#include "Engine/GameInstance.h"
#include "SpaceIdleBotanist.h"
#include "TimerManager.h"

bool USectorTravelSubsystem::RequestJump(FName SectorRow)
{
	if (SectorRow.IsNone())
	{
		return false;
	}

	if (JumpState == EJumpDriveState::Charging || JumpState == EJumpDriveState::Jumping)
	{
		UE_LOG(LogSIB, Verbose, TEXT("Jump request to '%s' rejected: drive busy."), *SectorRow.ToString());
		return false;
	}

	PendingSectorRow = SectorRow;
	SetJumpState(EJumpDriveState::Charging);

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance && JumpChargeSeconds > 0.0f)
	{
		GameInstance->GetTimerManager().SetTimer(
			JumpTimerHandle, this, &USectorTravelSubsystem::HandleChargeComplete, JumpChargeSeconds, false);
	}
	else
	{
		HandleChargeComplete();
	}

	return true;
}

void USectorTravelSubsystem::SetJumpState(EJumpDriveState NewState)
{
	if (JumpState == NewState)
	{
		return;
	}

	JumpState = NewState;
	OnJumpStateChanged.Broadcast(JumpState);
}

void USectorTravelSubsystem::HandleChargeComplete()
{
	SetJumpState(EJumpDriveState::Jumping);

	// Phase 3: resolve FSectorDef.LevelRef for PendingSectorRow and open the sector level;
	// ASIBGameModeSector signals arrival. Until then the state machine completes immediately.
	HandleTravelComplete();
}

void USectorTravelSubsystem::HandleTravelComplete()
{
	SetJumpState(EJumpDriveState::Arrived);
}
