// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "Procedural/PCGAsteroidClusterSettings.h"

#include "PCGContext.h"
#include "PCGPin.h"

UPCGAsteroidClusterSettings::UPCGAsteroidClusterSettings()
{
	bUseSeed = true;
}

TArray<FPCGPinProperties> UPCGAsteroidClusterSettings::InputPinProperties() const
{
	TArray<FPCGPinProperties> Properties;
	Properties.Emplace(PCGPinConstants::DefaultInputLabel, EPCGDataType::Spatial);
	return Properties;
}

TArray<FPCGPinProperties> UPCGAsteroidClusterSettings::OutputPinProperties() const
{
	TArray<FPCGPinProperties> Properties;
	Properties.Emplace(PCGPinConstants::DefaultOutputLabel, EPCGDataType::Point);
	return Properties;
}

FPCGElementPtr UPCGAsteroidClusterSettings::CreateElement() const
{
	return MakeShared<FPCGAsteroidClusterElement>();
}

bool FPCGAsteroidClusterElement::ExecuteInternal(FPCGContext* Context) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FPCGAsteroidClusterElement::ExecuteInternal);

	// Phase 3: derive an FRandomStream from Context->GetSeed(), scatter ClusterCount centers
	// across the input bounds, emit PointsPerCluster points per center with
	// RadialFalloffExponent-weighted offsets and AsteroidScaleRange scales into a
	// UPCGPointData output collection.
	return true;
}
