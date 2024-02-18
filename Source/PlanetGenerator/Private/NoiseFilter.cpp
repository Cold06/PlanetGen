// Fill out your copyright notice in the Description page of Project Settings.


#include "NoiseFilter.h"

#include "MathUtil.h"
#include "SimplexNoiseBPLibrary.h"

NoiseFilter::NoiseFilter()
{
}

NoiseFilter::~NoiseFilter()
{
}

float NoiseFilter::Evaluate(const FVector& InPoint, UNoiseSettings* NoiseSettings)
{
	if (NoiseSettings)
	{
		switch (NoiseSettings->NoiseType)
		{
		case FilterType::Rigid:
			return  EvaluateRigid(InPoint, NoiseSettings);
		case FilterType::Simple:
			return  EvaluateSimple(InPoint, NoiseSettings);
		default:
			return 0.f;
		}	
	}
	return 0.f;
	
}

float NoiseFilter::EvaluateSimple(const FVector& InPoint, UNoiseSettings* NoiseSettings)
{
	float NoiseValue = 0.f;
	float Frequency = NoiseSettings->BaseRoughness;
	float Amplitude = 1.f;

	for (int32 i = 0; i < NoiseSettings->NumberOfLayers; i++)
	{
		auto Point = InPoint;
		Point = (Point * Frequency) + NoiseSettings->Center;
		
		const float v = USimplexNoiseBPLibrary::SimplexNoise3D(Point.X, Point.Y, Point.Z);
		NoiseValue += (v + 1) * 0.5f * Amplitude;
		Frequency *= NoiseSettings->Roughness;
		Amplitude *= NoiseSettings->Persistence;
	}

	NoiseValue = FMath::Max(0, NoiseValue - NoiseSettings->MinValue);
	
	return NoiseValue * NoiseSettings->Strength;
}

float NoiseFilter::EvaluateRigid(const FVector& InPoint, UNoiseSettings* NoiseSettings)
{
	float NoiseValue = 0.f;
	float Frequency = NoiseSettings->BaseRoughness;
	float Amplitude = 1.f;
	float Weight = 1.f;

	for (int32 i = 0; i < NoiseSettings->NumberOfLayers; i++)
	{
		auto Point = InPoint;
		Point = (Point * Frequency) + NoiseSettings->Center;
		
		float v = 1.f - FMathf::Abs(USimplexNoiseBPLibrary::SimplexNoise3D(Point.X, Point.Y, Point.Z));
		v *= v; 
		v *= Weight;
		Weight = FMathf::Clamp(v, 0.f, v * NoiseSettings->WeightMultiplier);
		
		NoiseValue += v * Amplitude;
		Frequency *= NoiseSettings->Roughness;
		Amplitude *= NoiseSettings->Persistence;
	}
	NoiseValue = FMath::Max(0, NoiseValue - NoiseSettings->MinValue);
	return NoiseValue * NoiseSettings->Strength;
}

