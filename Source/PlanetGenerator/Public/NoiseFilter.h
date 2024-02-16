// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NoiseSettings.h"

/**
 * 
 */
class PLANETGENERATOR_API NoiseFilter
{
public:
	NoiseFilter();
	~NoiseFilter();


	
	float Evaluate(const FVector& Point, UNoiseSettings* NoiseSettings);
};
