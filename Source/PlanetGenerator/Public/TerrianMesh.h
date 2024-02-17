// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NoiseFilter.h"
#include "ProceduralMeshComponent.h"
#include "ShapeSettings.h"
#include "Components/SceneComponent.h"
#include "TerrianMesh.generated.h"





UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PLANETGENERATOR_API UTerrianMesh : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:	
	UTerrianMesh(const FObjectInitializer& ObjectInitializer);

	bool bDirty = true;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, BlueprintSetter=SetGridSize);
	int32 GridSize = 2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere,BlueprintSetter=SetLocalUp);
	FVector LocalUp = FVector::UpVector;

	UPROPERTY(EditAnywhere, BlueprintSetter=BPSetMaterial)
	TObjectPtr<UMaterialInterface> Material;

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, BlueprintSetter=SetShapeSettings);
	UShapeSettings* ShapeSettings;
	
	UFUNCTION(BlueprintSetter)
	void SetGridSize(int32 newValue);

	UFUNCTION(BlueprintSetter)
	void SetLocalUp(FVector newValue);

	UFUNCTION(BlueprintSetter)
	void SetWorldSpaceScalar(float newValue);

	UFUNCTION(BlueprintSetter)
	void BPSetMaterial(UMaterialInterface* NewLandscapeMaterial);
	
	virtual void PostInitProperties() override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;

	virtual void PostLoad() override;
	
	void RebuildMesh(FName Reason);

	UFUNCTION(BlueprintSetter)
	void SetShapeSettings(UShapeSettings* NewShapeSettings);

	virtual TStructOnScope<FActorComponentInstanceData> GetComponentInstanceData() const override;

	void ApplyComponentInstanceData(struct FTerrianMeshInstanceData* ComponentInstanceData, const bool bPostUCS);
	
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

	friend struct FTerrianMeshInstanceData;
};

USTRUCT()
struct FTerrianMeshInstanceData : public FSceneComponentInstanceData {
	GENERATED_BODY()

public:
	UPROPERTY();
	int32 GridSize = 2;

	UPROPERTY();
	FVector LocalUp = FVector::UpVector;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> Material;

	UPROPERTY()
	FTransform Transform;
	
	UPROPERTY();
	UShapeSettings* ShapeSettings;

public:
	FTerrianMeshInstanceData() {}
	explicit FTerrianMeshInstanceData(const UTerrianMesh* SourceComponent)
		: FSceneComponentInstanceData()
	{
		bUseForceVirtualApplyCheck = true;
	}
	virtual ~FTerrianMeshInstanceData() = default;

	virtual bool ShouldForceApply() override
	{
		return true;
	};
	
	virtual void ApplyToComponent(UActorComponent* Component, const ECacheApplyPhase CacheApplyPhase) override {
		Super::ApplyToComponent(Component, CacheApplyPhase);
		
		CastChecked<UTerrianMesh>(Component)->ApplyComponentInstanceData(
				this, (CacheApplyPhase == ECacheApplyPhase::PostUserConstructionScript));
	}
};
