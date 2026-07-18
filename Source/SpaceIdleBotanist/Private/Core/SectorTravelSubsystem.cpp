// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "Core/SectorTravelSubsystem.h"

#include "Core/SIBGameInstance.h"
#include "Data/SIBSectorDef.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
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

	USIBGameInstance* SIBGameInstance = Cast<USIBGameInstance>(GetGameInstance());
	const UDataTable* SectorTable = SIBGameInstance ? SIBGameInstance->GetSectorTable() : nullptr;
	const FSectorDef* Def = SectorTable
		? SectorTable->FindRow<FSectorDef>(PendingSectorRow, TEXT("USectorTravelSubsystem::HandleChargeComplete"))
		: nullptr;

	if (SIBGameInstance && Def && !Def->LevelRef.IsNull())
	{
		// Persist ledger and accrual timestamps before leaving the current level.
		SIBGameInstance->WriteSaveGame();
		UGameplayStatics::OpenLevelBySoftObjectPtr(SIBGameInstance->GetWorld(), Def->LevelRef);
		// ASIBGameModeSector::StartPlay calls NotifyArrivedInSector from the new level.
	}
	else
	{
		UE_LOG(LogSIB, Warning, TEXT("Sector '%s' has no level assigned; completing jump in place."),
			*PendingSectorRow.ToString());
		HandleTravelComplete();
	}
}

void USectorTravelSubsystem::NotifyArrivedInSector()
{
	HandleTravelComplete();
}

void USectorTravelSubsystem::HandleTravelComplete()
{
	SetJumpState(EJumpDriveState::Arrived);
}
