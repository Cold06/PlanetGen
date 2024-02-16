// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NoiseFilter.h"
#include "ShapeSettings.h"

/**
 * 
 */
class PLANETGENERATOR_API ShapeGenerator
{
public:
	UShapeSettings* Settings;
	NoiseFilter* noiseFilter;
	
	ShapeGenerator(UShapeSettings* Settings, NoiseFilter* noiseFilter);

	FVector CalculatePointOnSphere(FVector PointOnSphere);
	
	~ShapeGenerator();
};
