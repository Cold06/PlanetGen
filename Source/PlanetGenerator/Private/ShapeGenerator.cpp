// Fill out your copyright notice in the Description page of Project Settings.


#include "ShapeGenerator.h"

ShapeGenerator::ShapeGenerator(UShapeSettings* s, NoiseFilter* n)
{
	Settings = s;
	noiseFilter = n;
}

FVector ShapeGenerator::CalculatePointOnSphere(FVector PointOnSphere)
{
	double Elevation = 0.f;
	double firstLayerValue = 0.f;

	if (!Settings->NoiseSettings.IsEmpty())
	{
		firstLayerValue = noiseFilter->Evaluate(PointOnSphere, Settings->NoiseSettings[0]);
		if (Settings->NoiseSettings[0] && Settings->NoiseSettings[0]->Enabled)
		{
			Elevation = firstLayerValue;
		} 
	}
	
	for (int32 i = 1; i < Settings->NoiseSettings.Num(); i++)
	{
		auto Setting = Settings->NoiseSettings[i];

		
		if (Setting->Enabled)
		{
			double mask = Setting->UseFirstLayerAsMask ? firstLayerValue : 1.f;
			Elevation += noiseFilter->Evaluate(PointOnSphere, Setting) * mask; 	
		}
	}
	
	return PointOnSphere * Settings->Radius * (1 + Elevation);
}

FVector ShapeGenerator::NoNoise(FVector PointOnSphere)
{
	return PointOnSphere * Settings->Radius;
}

ShapeGenerator::~ShapeGenerator()
{
}
