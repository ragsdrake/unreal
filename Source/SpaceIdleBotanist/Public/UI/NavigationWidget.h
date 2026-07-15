// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/SIBWidgetBase.h"
#include "Data/SIBDelegates.h"
#include "NavigationWidget.generated.h"

class UDataTable;

/** Destination list ([Destination: <Sector>]) and the [Initialize Jump Drive] action. */
UCLASS(Abstract)
class SPACEIDLEBOTANIST_API UNavigationWidget : public USIBWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "SIB|UI")
	TArray<FName> GetDestinationRows() const;

	UFUNCTION(BlueprintCallable, Category = "SIB|UI")
	FText GetDestinationLabel(FName SectorRow) const;

	UFUNCTION(BlueprintCallable, Category = "SIB|UI")
	bool InitializeJumpDrive(FName DestinationRow);

	UFUNCTION(BlueprintPure, Category = "SIB|UI")
	static FText FormatDestinationLabel(const FText& SectorDisplayName);

	UFUNCTION(BlueprintPure, Category = "SIB|UI")
	static FText GetJumpDriveActionLabel();

protected:
	virtual void BindDelegates() override;
	virtual void UnbindDelegates() override;

	UFUNCTION()
	void HandleJumpStateChanged(EJumpDriveState NewState);

	/** Blueprint child updates jump-drive button state / charge feedback here. */
	UFUNCTION(BlueprintImplementableEvent, Category = "SIB|UI")
	void OnJumpDriveStateChanged(EJumpDriveState NewState);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|UI")
	TSoftObjectPtr<UDataTable> SectorTable;
};
