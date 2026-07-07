// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SIBGameModeSector.generated.h"

/** Game mode for procedural sector (spoke) levels. Kicks seeded PCG generation on start. */
UCLASS()
class SPACEIDLEBOTANIST_API ASIBGameModeSector : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASIBGameModeSector();

	virtual void StartPlay() override;

	/** Restores the sector's persisted seed and triggers PCG generation. */
	UFUNCTION(BlueprintCallable, Category = "SIB|Sector")
	void InitializeSectorGeneration();
};
