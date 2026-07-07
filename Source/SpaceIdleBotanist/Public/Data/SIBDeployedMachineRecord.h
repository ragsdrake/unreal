// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SIBDeployedMachineRecord.generated.h"

/** Save-file record of one deployed machine. Timestamps are FDateTime UTC ticks for stable serialization. */
USTRUCT(BlueprintType)
struct SPACEIDLEBOTANIST_API FDeployedMachineRecord
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "SIB|Save")
	FName MachineRow;

	UPROPERTY(BlueprintReadOnly, Category = "SIB|Save")
	FName SectorRow;

	UPROPERTY(BlueprintReadOnly, Category = "SIB|Save")
	FTransform Transform = FTransform::Identity;

	UPROPERTY(BlueprintReadOnly, Category = "SIB|Save")
	int64 DeployUtcTicks = 0;

	UPROPERTY(BlueprintReadOnly, Category = "SIB|Save")
	int64 LastCollectUtcTicks = 0;
};
