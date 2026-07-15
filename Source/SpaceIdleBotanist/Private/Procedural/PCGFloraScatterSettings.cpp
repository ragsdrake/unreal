// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "Procedural/PCGFloraScatterSettings.h"

#include "PCGContext.h"
#include "PCGPin.h"

UPCGFloraScatterSettings::UPCGFloraScatterSettings()
{
	bUseSeed = true;
}

TArray<FPCGPinProperties> UPCGFloraScatterSettings::InputPinProperties() const
{
	TArray<FPCGPinProperties> Properties;
	Properties.Emplace(PCGPinConstants::DefaultInputLabel, EPCGDataType::Surface);
	return Properties;
}

TArray<FPCGPinProperties> UPCGFloraScatterSettings::OutputPinProperties() const
{
	TArray<FPCGPinProperties> Properties;
	Properties.Emplace(PCGPinConstants::DefaultOutputLabel, EPCGDataType::Point);
	return Properties;
}

FPCGElementPtr UPCGFloraScatterSettings::CreateElement() const
{
	return MakeShared<FPCGFloraScatterElement>();
}

bool FPCGFloraScatterElement::ExecuteInternal(FPCGContext* Context) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FPCGFloraScatterElement::ExecuteInternal);

	// Phase 3: sample the input surface with USIBRandomLibrary::JitteredGridScatter seeded from
	// Context->GetSeed(), reject points outside GenParams SlopeLimitDegrees/AltitudeRange,
	// pick species by MeshWeights, and emit a UPCGPointData output with per-point scale/yaw.
	return true;
}
