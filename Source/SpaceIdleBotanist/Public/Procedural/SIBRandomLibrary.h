// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SIBRandomLibrary.generated.h"

/**
 * Deterministic FRandomStream helpers. Seeds derive from content (string CRC), never from
 * FName runtime indices, so results reproduce across sessions - the backbone of the
 * no-identical-clones rule and seed persistence (RISK-002).
 */
UCLASS()
class SPACEIDLEBOTANIST_API USIBRandomLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Combines a base seed with a salt so each consumer (asteroids, flora, debris) gets an independent stream. */
	UFUNCTION(BlueprintPure, Category = "SIB|Random")
	static int32 DeriveSeed(int32 BaseSeed, FName Salt);

	UFUNCTION(BlueprintPure, Category = "SIB|Random")
	static FRandomStream MakeStream(int32 BaseSeed, FName Salt);

	UFUNCTION(BlueprintPure, Category = "SIB|Random")
	static float RandRangeFromInterval(const FRandomStream& Stream, FFloatInterval Range);

	/**
	 * Grid-cell scatter with per-cell jitter: even coverage without visible repetition.
	 * Returns 2D points in [0..AreaSize] local space; JitterFraction 0 = perfect grid, 1 = full-cell jitter.
	 */
	UFUNCTION(BlueprintPure, Category = "SIB|Random")
	static TArray<FVector2D> JitteredGridScatter(const FRandomStream& Stream, FVector2D AreaSize, float CellSize, float JitterFraction);
};
