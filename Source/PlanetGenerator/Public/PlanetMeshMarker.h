// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"
#include "PlanetMeshMarker.generated.h"

UCLASS()
class PLANETGENERATOR_API APlanetMeshMarker : public AActor
{
	GENERATED_BODY()
	
public:	
	APlanetMeshMarker();

	bool bDirty = true;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, BlueprintSetter=SetGridSizeX);
	int32 GridSizeX = 10;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, BlueprintSetter=SetGridSizeY);
	int32 GridSizeY = 10;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, BlueprintSetter=SetStepSize);
	float StepSize = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere,BlueprintSetter=SetWorldSpaceScalar);
	float WorldSpaceScalar = 1;

	UFUNCTION(BlueprintSetter)
	void SetGridSizeX(int32 newValue);

	UFUNCTION(BlueprintSetter)
	void SetGridSizeY(int32 newValue);

	UFUNCTION(BlueprintSetter)
	void SetStepSize(float newValue);

	UFUNCTION(BlueprintSetter)
	void SetWorldSpaceScalar(float newValue);

	virtual void PostInitProperties() override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	void RebuildMesh();

	UPROPERTY();
	TObjectPtr<UProceduralMeshComponent> ProceduralMesh;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
