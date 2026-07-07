// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SectorGenParams.generated.h"

class UCurveFloat;
class UStaticMesh;

/** Per-sector procedural generation tuning, referenced by FSectorDef. One asset per sector biome. */
UCLASS(BlueprintType)
class SPACEIDLEBOTANIST_API USectorGenParams : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Meshes the scatter nodes pick from (asteroids or flora species). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Generation")
	TArray<TSoftObjectPtr<UStaticMesh>> MeshPalette;

	/** Relative selection weight per palette entry; missing entries weigh 1.0. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Generation")
	TArray<float> MeshWeights;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Generation", meta = (ClampMin = "0.0"))
	float PointsPerHectare = 50.0f;

	/** Density multiplier by normalized distance from sector center (0..1). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Generation")
	TSoftObjectPtr<UCurveFloat> DensityFalloff;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Generation")
	FFloatInterval UniformScaleRange = FFloatInterval(0.8f, 1.2f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Generation")
	bool bRandomYaw = true;

	/** Surface placement is rejected outside this slope range (degrees). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Generation")
	FFloatInterval SlopeLimitDegrees = FFloatInterval(0.0f, 35.0f);

	/** Surface placement is rejected outside this world-Z range (cm). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SIB|Generation")
	FFloatInterval AltitudeRange = FFloatInterval(-100000.0f, 100000.0f);
};
