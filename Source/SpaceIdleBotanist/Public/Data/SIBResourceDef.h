// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SIBResourceDef.generated.h"

class UTexture2D;

/** One row per resource type in DT_Resources. Row names use the Res.<Group>.<Name> convention, e.g. Res.Mineral.Iron. */
USTRUCT(BlueprintType)
struct SPACEIDLEBOTANIST_API FResourceDef : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Resource")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Resource")
	TSoftObjectPtr<UTexture2D> Icon;

	/** Whether this resource participates in [Automated Mineral Export]. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Resource")
	bool bIsExportable = false;
};
