// Copyright (c) 2026 Space Idle Botanist Project. All Rights Reserved.

#include "Procedural/SIBRandomLibrary.h"

int32 USIBRandomLibrary::DeriveSeed(int32 BaseSeed, FName Salt)
{
	// StrCrc32 hashes the name's characters; GetTypeHash(FName) would use the unstable runtime index.
	const uint32 SaltHash = FCrc::StrCrc32(*Salt.ToString());
	return static_cast<int32>(HashCombine(static_cast<uint32>(BaseSeed), SaltHash));
}

FRandomStream USIBRandomLibrary::MakeStream(int32 BaseSeed, FName Salt)
{
	return FRandomStream(DeriveSeed(BaseSeed, Salt));
}

float USIBRandomLibrary::RandRangeFromInterval(const FRandomStream& Stream, FFloatInterval Range)
{
	return Stream.FRandRange(Range.Min, Range.Max);
}

TArray<FVector2D> USIBRandomLibrary::JitteredGridScatter(const FRandomStream& Stream, FVector2D AreaSize, float CellSize, float JitterFraction)
{
	TArray<FVector2D> Points;
	if (CellSize <= 0.0f || AreaSize.X <= 0.0f || AreaSize.Y <= 0.0f)
	{
		return Points;
	}

	const int32 CellsX = FMath::FloorToInt32(AreaSize.X / CellSize);
	const int32 CellsY = FMath::FloorToInt32(AreaSize.Y / CellSize);
	const float Jitter = FMath::Clamp(JitterFraction, 0.0f, 1.0f);
	Points.Reserve(CellsX * CellsY);

	for (int32 Y = 0; Y < CellsY; ++Y)
	{
		for (int32 X = 0; X < CellsX; ++X)
		{
			const FVector2D CellCenter((X + 0.5f) * CellSize, (Y + 0.5f) * CellSize);
			const FVector2D Offset(
				Stream.FRandRange(-0.5f, 0.5f) * CellSize * Jitter,
				Stream.FRandRange(-0.5f, 0.5f) * CellSize * Jitter);
			Points.Add(CellCenter + Offset);
		}
	}

	return Points;
}
