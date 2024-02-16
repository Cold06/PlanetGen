// Fill out your copyright notice in the Description page of Project Settings.


#include "ShapeGenerator.h"

ShapeGenerator::ShapeGenerator(UShapeSettings* s, NoiseFilter* n)
{
	Settings = s;
	noiseFilter = n;
}

FVector ShapeGenerator::CalculatePointOnSphere(FVector PointOnSphere)
{
	double Elevation = noiseFilter->Evaluate(PointOnSphere, Settings->NoiseSettings);
	return PointOnSphere * Settings->Radius * (1 + Elevation);
}

ShapeGenerator::~ShapeGenerator()
{
}
