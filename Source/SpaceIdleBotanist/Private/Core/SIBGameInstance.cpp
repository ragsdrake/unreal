// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "Core/SIBGameInstance.h"

#include "Core/SIBSaveGame.h"
#include "Core/IdleEconomySubsystem.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "SpaceIdleBotanist.h"

void USIBGameInstance::Init()
{
	Super::Init();

	LoadOrCreateSaveGame();

	if (UIdleEconomySubsystem* Economy = GetSubsystem<UIdleEconomySubsystem>())
	{
		Economy->RecalculateOfflineProgress();
	}
}

void USIBGameInstance::Shutdown()
{
	WriteSaveGame();
	Super::Shutdown();
}

bool USIBGameInstance::WriteSaveGame()
{
	if (!ActiveSaveGame)
	{
		return false;
	}

	ActiveSaveGame->LastSessionUtcTicks = FDateTime::UtcNow().GetTicks();
	const bool bSaved = UGameplayStatics::SaveGameToSlot(ActiveSaveGame, SaveSlotName, SaveUserIndex);
	if (!bSaved)
	{
		UE_LOG(LogSIB, Warning, TEXT("Failed to write save game to slot '%s'."), *SaveSlotName);
	}
	return bSaved;
}

USIBSaveGame* USIBGameInstance::LoadOrCreateSaveGame()
{
	if (ActiveSaveGame)
	{
		return ActiveSaveGame;
	}

	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex))
	{
		ActiveSaveGame = Cast<USIBSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveUserIndex));
	}

	if (!ActiveSaveGame)
	{
		ActiveSaveGame = Cast<USIBSaveGame>(UGameplayStatics::CreateSaveGameObject(USIBSaveGame::StaticClass()));
		UE_LOG(LogSIB, Log, TEXT("Created a fresh save game for slot '%s'."), *SaveSlotName);
	}

	return ActiveSaveGame;
}

UDataTable* USIBGameInstance::GetResourceTable() const
{
	return ResourceTable.LoadSynchronous();
}

UDataTable* USIBGameInstance::GetMachineTable() const
{
	return MachineTable.LoadSynchronous();
}

UDataTable* USIBGameInstance::GetSectorTable() const
{
	return SectorTable.LoadSynchronous();
}
