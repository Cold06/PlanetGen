// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "Components/SceneComponent.h"
#include "ProceduralNoiseMesh.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PLANETGENERATOR_API UProceduralNoiseMesh : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProceduralNoiseMesh();

	UPROPERTY(BlueprintReadWrite, EditAnywhere);
	int32 GridSizeX = 10;

	UPROPERTY(BlueprintReadWrite, EditAnywhere);
	int32 GridSizeY = 10;

	UPROPERTY(BlueprintReadWrite, EditAnywhere);
	float StepSize = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere);
	float WorldSpaceScalar = 1;

	UPROPERTY()
	UProceduralMeshComponent* MeshComponent;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
