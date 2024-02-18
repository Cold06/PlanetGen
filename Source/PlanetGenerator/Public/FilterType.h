// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(Blueprintable, BlueprintType)
enum class FilterType : uint8
{
	// Basic ass noise filter
	Simple,

	// The cooler daniel
	Rigid
};
