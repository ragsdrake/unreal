// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "Procedural/PCGFloraScatterSettings.h"

#include "Data/PCGPointData.h"
#include "Data/PCGSpatialData.h"
#include "PCGContext.h"
#include "PCGPin.h"
#include "Procedural/SIBRandomLibrary.h"
#include "Procedural/SectorGenParams.h"
#include "SpaceIdleBotanist.h"

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

	const UPCGFloraScatterSettings* Settings = Context->GetInputSettings<UPCGFloraScatterSettings>();
	if (!Settings)
	{
		return true;
	}

	const USectorGenParams* GenParams = Settings->GenParams.LoadSynchronous();
	if (!GenParams)
	{
		UE_LOG(LogSIB, Warning, TEXT("FloraScatter node has no USectorGenParams assigned; emitting nothing."));
		return true;
	}

	const int32 Seed = Context->GetSeed();
	FRandomStream Stream(Seed);

	// Cell size from density: 1 hectare = 100 m x 100 m = 1e8 cm^2.
	const double EffectiveDensity =
		FMath::Max(static_cast<double>(GenParams->PointsPerHectare) * Settings->DensityMultiplier, UE_DOUBLE_KINDA_SMALL_NUMBER);
	const float CellSize = static_cast<float>(FMath::Sqrt(1.0e8 / EffectiveDensity));

	const TArray<FPCGTaggedData> Inputs = Context->InputData.GetInputsByPin(PCGPinConstants::DefaultInputLabel);
	for (const FPCGTaggedData& Input : Inputs)
	{
		const UPCGSpatialData* Surface = Cast<UPCGSpatialData>(Input.Data);
		if (!Surface)
		{
			continue;
		}

		const FBox Bounds = Surface->GetBounds();
		if (!Bounds.IsValid)
		{
			continue;
		}

		UPCGPointData* OutputData = NewObject<UPCGPointData>();
		TArray<FPCGPoint>& OutPoints = OutputData->GetMutablePoints();

		const TArray<FVector2D> GridPoints = USIBRandomLibrary::JitteredGridScatter(
			Stream, FVector2D(Bounds.GetSize().X, Bounds.GetSize().Y), CellSize, Settings->JitterFraction);
		OutPoints.Reserve(GridPoints.Num());

		const FVector HalfCell(CellSize * 0.5f);
		for (const FVector2D& GridPoint : GridPoints)
		{
			const FVector ProbeLocation(Bounds.Min.X + GridPoint.X, Bounds.Min.Y + GridPoint.Y, Bounds.GetCenter().Z);

			// Project onto the surface; the sampled transform carries the surface normal.
			FPCGPoint Sampled;
			if (!Surface->SamplePoint(FTransform(ProbeLocation), FBox(-HalfCell, HalfCell), Sampled, nullptr))
			{
				continue;
			}

			const FVector Location = Sampled.Transform.GetLocation();
			if (Location.Z < GenParams->AltitudeRange.Min || Location.Z > GenParams->AltitudeRange.Max)
			{
				continue;
			}

			const FVector SurfaceNormal = Sampled.Transform.GetUnitAxis(EAxis::Z);
			const double SlopeDegrees = FMath::RadiansToDegrees(
				FMath::Acos(FMath::Clamp(FVector::DotProduct(SurfaceNormal, FVector::UpVector), -1.0, 1.0)));
			if (SlopeDegrees < GenParams->SlopeLimitDegrees.Min || SlopeDegrees > GenParams->SlopeLimitDegrees.Max)
			{
				continue;
			}

			FPCGPoint& Point = OutPoints.Add_GetRef(Sampled);
			FTransform PointTransform = Sampled.Transform;
			PointTransform.SetScale3D(FVector(USIBRandomLibrary::RandRangeFromInterval(Stream, GenParams->UniformScaleRange)));
			if (GenParams->bRandomYaw)
			{
				const FQuat Yaw(FVector::UpVector, Stream.FRandRange(0.0f, 2.0f * UE_PI));
				PointTransform.SetRotation(Yaw * PointTransform.GetRotation());
			}
			Point.Transform = PointTransform;
			Point.Density = 1.0f;
			Point.Seed = static_cast<int32>(HashCombine(static_cast<uint32>(Seed), GetTypeHash(GridPoint)));
		}

		FPCGTaggedData& Output = Context->OutputData.TaggedData.Emplace_GetRef();
		Output.Data = OutputData;
		Output.Pin = PCGPinConstants::DefaultOutputLabel;
	}

	// Species/mesh choice stays out of C++: the downstream Static Mesh Spawner uses its weighted
	// mesh entries (mirroring USectorGenParams MeshPalette/MeshWeights), keyed by each point's Seed.
	return true;
}
