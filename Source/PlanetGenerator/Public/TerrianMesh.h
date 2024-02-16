// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NoiseFilter.h"
#include "ProceduralMeshComponent.h"
#include "ShapeSettings.h"
#include "Components/SceneComponent.h"
#include "TerrianMesh.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PLANETGENERATOR_API UTerrianMesh : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTerrianMesh();

	bool bDirty = true;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, BlueprintSetter=SetGridSize);
	int32 GridSize = 10;

	UPROPERTY(BlueprintReadWrite, EditAnywhere,BlueprintSetter=SetWorldSpaceScalar);
	float WorldSpaceScalar = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere,BlueprintSetter=SetLocalUp);
	FVector LocalUp = FVector::UpVector;

	UFUNCTION(BlueprintSetter)
	void SetGridSize(int32 newValue);

	UFUNCTION(BlueprintSetter)
	void SetLocalUp(FVector newValue);

	UFUNCTION(BlueprintSetter)
	void SetWorldSpaceScalar(float newValue);

	UPROPERTY(EditAnywhere, BlueprintSetter=SetMaterial)
	TObjectPtr<UMaterialInterface> Material;

	UFUNCTION(BlueprintSetter)
	void SetMaterial(UMaterialInterface* NewLandscapeMaterial);
	
	virtual void PostInitProperties() override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;
	
	void RebuildMesh();

	UPROPERTY();
	UProceduralMeshComponent* ProceduralMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, BlueprintSetter=SetShapeSettings);
	UShapeSettings* ShapeSettings;

	UFUNCTION(BlueprintSetter)
	void SetShapeSettings(UShapeSettings* NewShapeSettings);

private:
	NoiseFilter* noiseFilter;
	
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	
private:
	FVector AxisA;
	FVector AxisB;
	void UpdateAxis();
	void RebindDelegates();
	void UnbindPropertyChangeDelegate();
	FDelegateHandle OnShapeSettingsChangedDelegateHandle;
	FDelegateHandle OnNoiseSettingsChangedDelegateHandle;

	
	
public:
	void OnExternalPropChanged(const FName PropertyName);

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
