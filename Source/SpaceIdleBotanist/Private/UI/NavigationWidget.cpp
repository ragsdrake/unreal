// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "UI/NavigationWidget.h"

#include "Core/SectorTravelSubsystem.h"
#include "Data/SIBSectorDef.h"
#include "Engine/DataTable.h"

TArray<FName> UNavigationWidget::GetDestinationRows() const
{
	if (const UDataTable* Table = SectorTable.LoadSynchronous())
	{
		return Table->GetRowNames();
	}
	return TArray<FName>();
}

FText UNavigationWidget::GetDestinationLabel(FName SectorRow) const
{
	if (const UDataTable* Table = SectorTable.LoadSynchronous())
	{
		if (const FSectorDef* Def = Table->FindRow<FSectorDef>(SectorRow, TEXT("UNavigationWidget::GetDestinationLabel")))
		{
			return FormatDestinationLabel(Def->DisplayName);
		}
	}
	return FText::GetEmpty();
}

bool UNavigationWidget::InitializeJumpDrive(FName DestinationRow)
{
	if (USectorTravelSubsystem* Travel = GetTravelSubsystem())
	{
		return Travel->RequestJump(DestinationRow);
	}
	return false;
}

FText UNavigationWidget::FormatDestinationLabel(const FText& SectorDisplayName)
{
	return FText::Format(NSLOCTEXT("SIB", "DestinationFormat", "[Destination: {0}]"), SectorDisplayName);
}

FText UNavigationWidget::GetJumpDriveActionLabel()
{
	return NSLOCTEXT("SIB", "InitializeJumpDrive", "[Initialize Jump Drive]");
}

void UNavigationWidget::BindDelegates()
{
	Super::BindDelegates();
	if (USectorTravelSubsystem* Travel = GetTravelSubsystem())
	{
		Travel->OnJumpStateChanged.AddDynamic(this, &UNavigationWidget::HandleJumpStateChanged);
	}
}

void UNavigationWidget::UnbindDelegates()
{
	if (USectorTravelSubsystem* Travel = GetTravelSubsystem())
	{
		Travel->OnJumpStateChanged.RemoveDynamic(this, &UNavigationWidget::HandleJumpStateChanged);
	}
	Super::UnbindDelegates();
}

void UNavigationWidget::HandleJumpStateChanged(EJumpDriveState NewState)
{
	OnJumpDriveStateChanged(NewState);
}
