// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NoiseSettings.h"
#include "Engine/DataAsset.h"
#include "ShapeSettings.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemDataPropertyChangedSignature, FName /* PropertyName */);

/**
 * 
 */
UCLASS()
class PLANETGENERATOR_API UShapeSettings : public UDataAsset
{
	GENERATED_BODY()

	UShapeSettings();
public:
		UPROPERTY(EditAnywhere)
		float Radius = 0.f;

		UPROPERTY(EditAnywhere)
		UNoiseSettings* NoiseSettings;

		FOnItemDataPropertyChangedSignature OnItemDataPropertyChangedDelegate;
	
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override
		{
			Super::PostEditChangeProperty(PropertyChangedEvent);
			
			const FName PropertyName = PropertyChangedEvent.Property->GetFName();
			OnItemDataPropertyChangedDelegate.Broadcast(PropertyName);
		}
};
