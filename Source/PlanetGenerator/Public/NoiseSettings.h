// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FilterType.h"
#include "Engine/DataAsset.h"
#include "NoiseSettings.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemDataPropertyChangedSignature, FName /* PropertyName */);


/**
 * 
 */
UCLASS(BlueprintType)
class PLANETGENERATOR_API UNoiseSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	bool UseFirstLayerAsMask = false;

	UPROPERTY(EditAnywhere)
	bool Enabled = true;

	UPROPERTY(EditAnywhere)
	float Strength = 1.f;

	UPROPERTY(EditAnywhere)
	float Roughness = 2.f;
	
	UPROPERTY(EditAnywhere)
	int32 NumberOfLayers = 1;

	UPROPERTY(EditAnywhere)
	float Persistence = 0.5;

	UPROPERTY(EditAnywhere)
	float BaseRoughness = 1.f;

	UPROPERTY(EditAnywhere)
	float MinValue = 0.f;

	UPROPERTY(EditAnywhere)
	float WeightMultiplier = 0.8f;

	UPROPERTY(EditAnywhere)
	FVector Center = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	FilterType NoiseType = FilterType::Simple;

	FOnItemDataPropertyChangedSignature FOnItemDataPropertyChangedDelegate;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);
		const FName PropertyName = PropertyChangedEvent.Property->GetFName();
		FOnItemDataPropertyChangedDelegate.Broadcast(PropertyName);
	}
};
