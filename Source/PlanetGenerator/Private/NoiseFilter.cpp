// Fill out your copyright notice in the Description page of Project Settings.


#include "NoiseFilter.h"

#include "SimplexNoiseBPLibrary.h"

NoiseFilter::NoiseFilter()
{
}

NoiseFilter::~NoiseFilter()
{
}

float NoiseFilter::Evaluate(const FVector& InPoint, UNoiseSettings* NoiseSettings)
{
	float NoiseValue = 0.f;
	float Frequency = NoiseSettings->BaseRoughness;
	float Amplitude = 1.f;

	for (int32 i = 0; i < NoiseSettings->NumberOfLayers; i++)
	{
		auto Point = InPoint;
		Point = Point * (NoiseSettings->Center + Frequency);
		
		float v = USimplexNoiseBPLibrary::SimplexNoise3D(Point.X, Point.Y, Point.Z);
		NoiseValue += (v + 1) * 0.5f * Amplitude;
		Frequency *= NoiseSettings->Roughness;
		Amplitude *= NoiseSettings->Persistence;
	}

	NoiseValue = FMath::Max(0, NoiseValue - NoiseSettings->MinValue);
	
	return NoiseValue * NoiseSettings->Strength;
}
