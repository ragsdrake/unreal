// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PCGSettings.h"
#include "PCGFloraScatterSettings.generated.h"

class USectorGenParams;

/**
 * Custom PCG node: seeded plant placement on sector terrain (Mars Surface, Earth Orbit greenhouses).
 * Applies slope/altitude rejection and species selection from USectorGenParams. Consumes the PCG
 * seed wired from APlanetarySector.GenerationSeed - every generation is unique per save.
 */
UCLASS(BlueprintType, ClassGroup = (Procedural))
class SPACEIDLEBOTANIST_API UPCGFloraScatterSettings : public UPCGSettings
{
	GENERATED_BODY()

public:
	UPCGFloraScatterSettings();

	//~Begin UPCGSettings interface
#if WITH_EDITOR
	virtual FName GetDefaultNodeName() const override { return FName(TEXT("FloraScatter")); }
	virtual FText GetDefaultNodeTitle() const override { return NSLOCTEXT("SIB", "FloraScatterNodeTitle", "Flora Scatter"); }
	virtual EPCGSettingsType GetType() const override { return EPCGSettingsType::Sampler; }
#endif

protected:
	virtual TArray<FPCGPinProperties> InputPinProperties() const override;
	virtual TArray<FPCGPinProperties> OutputPinProperties() const override;
	virtual FPCGElementPtr CreateElement() const override;
	//~End UPCGSettings interface

public:
	/** Density curves, species palette/weights, and slope/altitude limits for this sector. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PCG_Overridable))
	TSoftObjectPtr<USectorGenParams> GenParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PCG_Overridable, ClampMin = "0.0"))
	float DensityMultiplier = 1.0f;

	/** Extra per-point yaw/offset jitter on top of USectorGenParams settings. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PCG_Overridable, ClampMin = "0.0", ClampMax = "1.0"))
	float JitterFraction = 0.75f;
};

class FPCGFloraScatterElement : public IPCGElement
{
protected:
	virtual bool ExecuteInternal(FPCGContext* Context) const override;
};
