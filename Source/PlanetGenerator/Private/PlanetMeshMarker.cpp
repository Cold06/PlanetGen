// Fill out your copyright notice in the Description page of Project Settings.


#include "PlanetMeshMarker.h"

#include "ProceduralMeshComponent.h"

APlanetMeshMarker::APlanetMeshMarker()
{
	PrimaryActorTick.bCanEverTick = true;
	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Test"));
	ProceduralMesh->AddToRoot();
	ProceduralMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void APlanetMeshMarker::SetGridSizeX(int32 newValue)
{
	GridSizeX = newValue;
	bDirty = true;
}

void APlanetMeshMarker::SetGridSizeY(int32 newValue)
{
	GridSizeY = newValue;
	bDirty = true;
}

void APlanetMeshMarker::SetStepSize(float newValue)
{
	StepSize = newValue;
	bDirty = true;
}

void APlanetMeshMarker::SetWorldSpaceScalar(float newValue)
{
	WorldSpaceScalar = newValue;
	bDirty = true;
}

void APlanetMeshMarker::PostInitProperties()
{
	Super::PostInitProperties();

	if (bDirty)
	{
		bDirty = false;
		RebuildMesh();
	}
}

void APlanetMeshMarker::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property == nullptr)
	{
		return;
	}
	
	static FName GSX("GridSizeX");
	static FName GSY("GridSizeY");
	static FName SS("StepSize");
	static FName WSS("WorldSpaceScalar");

	const auto name = PropertyChangedEvent.Property->NamePrivate;
	
	if (name == GSX || name == GSY || name == SS || name == WSS)
	{
		RebuildMesh();
	}
}

void APlanetMeshMarker::RebuildMesh()
{
	if (!IsValid(ProceduralMesh))
	{
		UE_LOG(LogActor, Error, TEXT("APlanetMeshMarker Without ProceduralMesh"));
		return;
	}
	
	TArray<FVector> Vertices;
    TArray<int32> Triangles;
    TArray<FVector> Normals;
    TArray<FVector2D> UV0;

	for (int32 iy = 0; iy < GridSizeY; ++iy)
	{
		for (int32 ix = 0; ix < GridSizeX; ++ix)
		{
			const float x = WorldSpaceScalar * static_cast<float>(ix);
			const float y = WorldSpaceScalar * static_cast<float>(iy);

			const float NoiseValue = 0.f;
				
			const float z = WorldSpaceScalar * NoiseValue;
				
			FVector Vertex(x, y, z);
			Vertices.Add(Vertex);
			FVector2D UV(x / (GridSizeX - 1), y / (GridSizeY - 1));
			UV0.Add(UV);
			Normals.Add(FVector::UpVector);
				
			if (x > 0 && y > 0)
			{
				int32 CurrentIndex = x + y * GridSizeX;
				int32 BottomLeftIndex = CurrentIndex - GridSizeX - 1;
				int32 BottomRightIndex = CurrentIndex - GridSizeX;
				int32 TopLeftIndex = CurrentIndex - 1;
				int32 TopRightIndex = CurrentIndex;

				Triangles.Add(TopLeftIndex);
				Triangles.Add(BottomRightIndex);
				Triangles.Add(BottomLeftIndex);
					
				Triangles.Add(TopLeftIndex);
				Triangles.Add(TopRightIndex);
				Triangles.Add(BottomRightIndex);
			}
		}
	}

    ProceduralMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, TArray<FLinearColor>(), TArray<FProcMeshTangent>(), true);
    ProceduralMesh->SetMaterial(0, nullptr);
    
}

void APlanetMeshMarker::BeginPlay()
{
	Super::BeginPlay();
}

void APlanetMeshMarker::BeginDestroy()
{
	Super::BeginDestroy();
	ProceduralMesh->RemoveFromRoot();
}

void APlanetMeshMarker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDirty)
	{
		bDirty = false;
		RebuildMesh();
	}
}

