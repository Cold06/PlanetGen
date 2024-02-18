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
		TArray<UNoiseSettings*>  NoiseSettings;


	
		FOnItemDataPropertyChangedSignature OnItemDataPropertyChangedDelegate;
	
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override
		{
			Super::PostEditChangeProperty(PropertyChangedEvent);

			static FName NoiseSettingsName("NoiseSettings");
			
			if (PropertyChangedEvent.GetPropertyName() == NoiseSettingsName)
			{
				RebindEverything();	
			}
			
			const FName PropertyName = PropertyChangedEvent.Property->GetFName();
			OnItemDataPropertyChangedDelegate.Broadcast(PropertyName);
		}

		void NotifyChange(const FName PropertyName)
		{
			OnItemDataPropertyChangedDelegate.Broadcast(PropertyName);
		}

		virtual void BeginDestroy() override
		{
			UnbindAll();
		}

		void UnbindAll()
		{
			for (auto Pair : Delegates)
			{
				Pair.Get<0>()->FOnItemDataPropertyChangedDelegate.Remove(Pair.Get<1>());
			}

			Delegates.Reset();
		}
	
		void RebindEverything()
		{
			UnbindAll();

			for (auto Noise : NoiseSettings)
			{
				if (Noise)
				{
					auto DelegateHandle = Noise->FOnItemDataPropertyChangedDelegate.AddUObject(this, &UShapeSettings::NotifyChange);
					Delegates.Add(Noise, DelegateHandle);
				}
			}
		}

private:
	TMap<UNoiseSettings*, FDelegateHandle>  Delegates;
};
 