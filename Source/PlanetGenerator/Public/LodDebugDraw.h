// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/ShapeComponent.h"
#include "LodDebugDraw.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PLANETGENERATOR_API ULodDebugDraw : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:	
	ULodDebugDraw(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadWrite, EditAnywhere);
	int32 GridSize = 200;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere,BlueprintSetter=SetLocalUp);
	FVector LocalUp = FVector::UpVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMaterialInterface* Material;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, BlueprintSetter=SetSampleStart);
	FVector2D SampleStart = FVector2D::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, BlueprintSetter=SetSampleFocus);
	float SampleFocus = 1.f;

	UFUNCTION(BlueprintSetter)
	void SetSampleStart(FVector2D NewSampleStart);

	UFUNCTION(BlueprintSetter)
	void SetSampleFocus(float NewSampleFocus);

	UFUNCTION(BlueprintSetter, BlueprintCallable)
	void BPSetMaterial(UMaterialInterface* NewLandscapeMaterial);

	virtual void PostInitProperties() override;

	UFUNCTION(BlueprintSetter)
	void SetGridSize(int32 newValue);

	UFUNCTION(BlueprintSetter)
	void SetLocalUp(FVector newValue);

	void RebuildMesh(FName Reason);

	void UpdateAxis();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	FVector AxisA;
	FVector AxisB;
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ApplyComponentInstanceData(struct FLodDebugDrawInstanceData* ComponentInstanceData, const bool bPostUCS);
	TStructOnScope<FActorComponentInstanceData> GetComponentInstanceData() const override;
};



USTRUCT()
struct FLodDebugDrawInstanceData : public FSceneComponentInstanceData {
	GENERATED_BODY()

	UPROPERTY();
	int32 GridSize = 2;

	UPROPERTY();
	FVector LocalUp = FVector::UpVector;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> Material;

	UPROPERTY()
	FTransform Transform;
	
	UPROPERTY()
	FVector2D SampleStart;

	UPROPERTY()
	float SampleFocus = 1.f;

	FLodDebugDrawInstanceData() {}
	explicit FLodDebugDrawInstanceData(const ULodDebugDraw* SourceComponent)
		: FSceneComponentInstanceData()
	{
		bUseForceVirtualApplyCheck = false;
	}
	virtual ~FLodDebugDrawInstanceData() = default;

	virtual bool ShouldForceApply() override
	{
		return true;
	};
	
	virtual void ApplyToComponent(UActorComponent* Component, const ECacheApplyPhase CacheApplyPhase) override {
		Super::ApplyToComponent(Component, CacheApplyPhase);

		bool bIsPostUCS = (CacheApplyPhase == ECacheApplyPhase::PostUserConstructionScript);
		
		CastChecked<ULodDebugDraw>(Component)->ApplyComponentInstanceData(
				this, bIsPostUCS);	
	}
};
