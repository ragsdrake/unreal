// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SIBPlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;

/** Routes Enhanced Input mapping contexts and switches between 3D-game and UI input modes. */
UCLASS()
class SPACEIDLEBOTANIST_API ASIBPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "SIB|Input")
	void EnterUIMode(UUserWidget* FocusWidget);

	UFUNCTION(BlueprintCallable, Category = "SIB|Input")
	void EnterGameMode();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Input")
	TSoftObjectPtr<UInputMappingContext> HubMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Input")
	TSoftObjectPtr<UInputMappingContext> SectorMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Input")
	int32 MappingContextPriority = 0;

	/** HUD root widget, created on BeginPlay by the Blueprint child (WBP layout stays in Content). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|UI")
	TSubclassOf<UUserWidget> HudWidgetClass;
};
