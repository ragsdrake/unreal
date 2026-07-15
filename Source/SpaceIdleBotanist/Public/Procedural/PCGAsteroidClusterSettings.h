// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PCGSettings.h"
#include "PCGAsteroidClusterSettings.generated.h"

/**
 * Custom PCG node: seeded asteroid cluster scatter for the Asteroid Belt sector.
 * Emits cluster centers, then per-cluster point clouds with radial falloff. Output points
 * feed a Static Mesh Spawner in ISM/HISM mode (PERF-001). The node consumes the PCG seed,
 * wired from APlanetarySector.GenerationSeed, so no two belts are identical.
 */
UCLASS(BlueprintType, ClassGroup = (Procedural))
class SPACEIDLEBOTANIST_API UPCGAsteroidClusterSettings : public UPCGSettings
{
	GENERATED_BODY()

public:
	UPCGAsteroidClusterSettings();

	//~Begin UPCGSettings interface
#if WITH_EDITOR
	virtual FName GetDefaultNodeName() const override { return FName(TEXT("AsteroidClusterScatter")); }
	virtual FText GetDefaultNodeTitle() const override { return NSLOCTEXT("SIB", "AsteroidClusterNodeTitle", "Asteroid Cluster Scatter"); }
	virtual EPCGSettingsType GetType() const override { return EPCGSettingsType::Sampler; }
#endif

protected:
	virtual TArray<FPCGPinProperties> InputPinProperties() const override;
	virtual TArray<FPCGPinProperties> OutputPinProperties() const override;
	virtual FPCGElementPtr CreateElement() const override;
	//~End UPCGSettings interface

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PCG_Overridable, ClampMin = "0"))
	int32 ClusterCount = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PCG_Overridable, ClampMin = "0"))
	int32 PointsPerCluster = 40;

	/** Cluster radius in cm. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PCG_Overridable, ClampMin = "0.0"))
	float ClusterRadius = 5000.0f;

	/** Higher values pack asteroids toward the cluster center. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PCG_Overridable, ClampMin = "0.1"))
	float RadialFalloffExponent = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PCG_Overridable))
	FFloatInterval AsteroidScaleRange = FFloatInterval(0.5f, 3.0f);
};

class FPCGAsteroidClusterElement : public IPCGElement
{
protected:
	virtual bool ExecuteInternal(FPCGContext* Context) const override;
};
