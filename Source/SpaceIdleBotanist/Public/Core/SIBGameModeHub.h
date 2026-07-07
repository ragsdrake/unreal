// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SIBGameModeHub.generated.h"

/** Game mode for the ship-interior hub level. Pawn class is assigned by the Blueprint child. */
UCLASS()
class SPACEIDLEBOTANIST_API ASIBGameModeHub : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASIBGameModeHub();
};
