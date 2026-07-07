// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Hub/InteractableBase.h"
#include "PilotConsole.generated.h"

class UDataTable;

/** Navigation station: lists [Destination: <Sector>] entries and forwards [Initialize Jump Drive]. */
UCLASS()
class SPACEIDLEBOTANIST_API APilotConsole : public AInteractableBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "SIB|Navigation")
	TArray<FName> GetAvailableDestinations() const;

	UFUNCTION(BlueprintCallable, Category = "SIB|Navigation")
	bool InitializeJumpDrive(FName DestinationSectorRow);

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

protected:
	/** Blueprint child opens WBP_Navigation here. */
	UFUNCTION(BlueprintImplementableEvent, Category = "SIB|Navigation")
	void OnOpenNavigationUI(APawn* InstigatorPawn);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Navigation")
	TSoftObjectPtr<UDataTable> SectorTable;
};
