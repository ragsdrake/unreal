// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Hub/InteractableBase.h"
#include "GeneLab.generated.h"

/** [Splice Genome] station: spends ledger resources to raise flora yield multipliers. */
UCLASS()
class SPACEIDLEBOTANIST_API AGeneLab : public AInteractableBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "SIB|GeneLab")
	bool SpliceGenome(FName TargetFloraRow);

protected:
	/** Blueprint child plays splice VFX / timeline here. */
	UFUNCTION(BlueprintImplementableEvent, Category = "SIB|GeneLab")
	void OnGenomeSpliced(FName TargetFloraRow);

	/** Resource row -> amount consumed per splice. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|GeneLab")
	TMap<FName, int64> SpliceCosts;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|GeneLab", meta = (ClampMin = "1.0"))
	float YieldMultiplierPerSplice = 1.15f;
};
