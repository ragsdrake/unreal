// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "Procedural/PCGAsteroidClusterSettings.h"

#include "Data/PCGPointData.h"
#include "Data/PCGSpatialData.h"
#include "PCGContext.h"
#include "PCGPin.h"
#include "Procedural/SIBRandomLibrary.h"

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

	const UPCGAsteroidClusterSettings* Settings = Context->GetInputSettings<UPCGAsteroidClusterSettings>();
	if (!Settings)
	{
		return true;
	}

	// Field bounds: union of spatial inputs, or the settings-driven fallback box when unconnected.
	FBox FieldBounds(EForceInit::ForceInit);
	const TArray<FPCGTaggedData> Inputs = Context->InputData.GetInputsByPin(PCGPinConstants::DefaultInputLabel);
	for (const FPCGTaggedData& Input : Inputs)
	{
		if (const UPCGSpatialData* Spatial = Cast<UPCGSpatialData>(Input.Data))
		{
			FieldBounds += Spatial->GetBounds();
		}
	}
	if (!FieldBounds.IsValid)
	{
		FieldBounds = FBox(-Settings->FallbackFieldExtents, Settings->FallbackFieldExtents);
	}

	const int32 Seed = Context->GetSeed();
	FRandomStream Stream(Seed);

	UPCGPointData* OutputData = NewObject<UPCGPointData>();
	TArray<FPCGPoint>& OutPoints = OutputData->GetMutablePoints();
	OutPoints.Reserve(Settings->ClusterCount * Settings->PointsPerCluster);

	for (int32 ClusterIndex = 0; ClusterIndex < Settings->ClusterCount; ++ClusterIndex)
	{
		const FVector ClusterCenter(
			Stream.FRandRange(FieldBounds.Min.X, FieldBounds.Max.X),
			Stream.FRandRange(FieldBounds.Min.Y, FieldBounds.Max.Y),
			Stream.FRandRange(FieldBounds.Min.Z, FieldBounds.Max.Z));

		for (int32 PointIndex = 0; PointIndex < Settings->PointsPerCluster; ++PointIndex)
		{
			// Exponent > 1 packs asteroids toward the cluster core.
			const double Distance =
				Settings->ClusterRadius * FMath::Pow(Stream.FRand(), Settings->RadialFalloffExponent);
			const FVector Location = ClusterCenter + Stream.VRand() * Distance;

			FPCGPoint& Point = OutPoints.Emplace_GetRef();
			Point.Transform = FTransform(
				FRotator(Stream.FRandRange(0.0f, 360.0f), Stream.FRandRange(0.0f, 360.0f), Stream.FRandRange(0.0f, 360.0f)),
				Location,
				FVector(USIBRandomLibrary::RandRangeFromInterval(Stream, Settings->AsteroidScaleRange)));
			Point.Density = 1.0f;
			Point.Seed = static_cast<int32>(HashCombine(
				static_cast<uint32>(Seed),
				static_cast<uint32>(ClusterIndex * Settings->PointsPerCluster + PointIndex)));
		}
	}

	FPCGTaggedData& Output = Context->OutputData.TaggedData.Emplace_GetRef();
	Output.Data = OutputData;
	Output.Pin = PCGPinConstants::DefaultOutputLabel;

	return true;
}
