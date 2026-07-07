// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SIBGameInstance.generated.h"

class UDataTable;
class USIBSaveGame;

/** Session root: owns the active save object and the shared DataTable references. */
UCLASS()
class SPACEIDLEBOTANIST_API USIBGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;

	/** Stamps LastSessionUtcTicks and writes the active save to disk. */
	UFUNCTION(BlueprintCallable, Category = "SIB|Save")
	bool WriteSaveGame();

	UFUNCTION(BlueprintCallable, Category = "SIB|Save")
	USIBSaveGame* LoadOrCreateSaveGame();

	UFUNCTION(BlueprintPure, Category = "SIB|Save")
	USIBSaveGame* GetActiveSaveGame() const { return ActiveSaveGame; }

	UFUNCTION(BlueprintPure, Category = "SIB|Data")
	UDataTable* GetResourceTable() const;

	UFUNCTION(BlueprintPure, Category = "SIB|Data")
	UDataTable* GetMachineTable() const;

	UFUNCTION(BlueprintPure, Category = "SIB|Data")
	UDataTable* GetSectorTable() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Save")
	FString SaveSlotName = TEXT("SIB_Default");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Save")
	int32 SaveUserIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Data")
	TSoftObjectPtr<UDataTable> ResourceTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Data")
	TSoftObjectPtr<UDataTable> MachineTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Data")
	TSoftObjectPtr<UDataTable> SectorTable;

	UPROPERTY(Transient)
	TObjectPtr<USIBSaveGame> ActiveSaveGame;
};
