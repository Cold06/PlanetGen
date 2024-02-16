// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrianMesh.h"

#include "ProceduralMeshComponent.h"
#include "GeometryCollection/GeometryCollectionEngineUtility.h"

#include "CoreMinimal.h"
#include "ISimplexNoise.h"
#include "ShapeGenerator.h"
#include "SimplexNoiseBPLibrary.h"

void CalculateNormals(const TArray<FVector>& Vertices, const TArray<int32>& Triangles, TArray<FVector>& Normals)
{
	// Initialize normals array with zero vectors
	Normals.Init(FVector::ZeroVector, Vertices.Num());

	// For each triangle, calculate the face normal and accumulate it to the vertex normals
	for (int32 i = 0; i < Triangles.Num(); i += 3)
	{
		int32 Index0 = Triangles[i];
		int32 Index1 = Triangles[i + 1];
		int32 Index2 = Triangles[i + 2];

		// Calculate the face normal
		FVector Edge1 = Vertices[Index1] - Vertices[Index0];
		FVector Edge2 = Vertices[Index2] - Vertices[Index0];
		FVector FaceNormal = FVector::CrossProduct(Edge1, Edge2).GetSafeNormal();

		// Accumulate the face normal to each vertex of the triangle
		Normals[Index0] += FaceNormal;
		Normals[Index1] += FaceNormal;
		Normals[Index2] += FaceNormal;
	}

	// Normalize all the accumulated vertex normals
	for (FVector& Normal : Normals)
	{
		Normal.Normalize();
		Normal *= -1;
	}
}

UTerrianMesh::UTerrianMesh()
{
	PrimaryComponentTick.bCanEverTick = true;
	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Test"));
	ProceduralMesh->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
	bDirty = true;
	UpdateAxis();
}

void UTerrianMesh::SetGridSize(int32 newValue)
{
	GridSize = newValue;
	bDirty = true;
}

void UTerrianMesh::SetLocalUp(FVector newValue)
{
	LocalUp = newValue;
	bDirty = true;
}

void UTerrianMesh::SetWorldSpaceScalar(float newValue)
{
	WorldSpaceScalar = newValue;
	bDirty = true;
}

void UTerrianMesh::SetMaterial(UMaterialInterface* NewLandscapeMaterial)
{
	Material = NewLandscapeMaterial;
	RebuildMesh();
}

void UTerrianMesh::PostInitProperties()
{
	Super::PostInitProperties();
	RebindDelegates();

	if (bDirty)
	{
		bDirty = false;
		RebuildMesh();
	}
}

void UTerrianMesh::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
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

	RebindDelegates();
}

void UTerrianMesh::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
	Super::PostDuplicate(DuplicateMode);
	RebindDelegates();
	RebuildMesh();
}

void UTerrianMesh::RebuildMesh()
{
	double Start = FPlatformTime::Seconds();

	UpdateAxis();

	if (!ShapeSettings) return;

	if (!IsValid(ProceduralMesh))
	{
		UE_LOG(LogActor, Error, TEXT("UTerrianMesh Without ProceduralMesh"));
		return;
	}
	
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UV0;
	
	{
		auto Gen = new ShapeGenerator(ShapeSettings, noiseFilter);
		
		Vertices.Init(FVector::ZeroVector, GridSize * GridSize);
		Normals.Init(FVector::ZeroVector, GridSize * GridSize);
		UV0.Init(FVector2D::ZeroVector, GridSize * GridSize);
		Triangles.Init(0 ,(GridSize - 1) * (GridSize - 1) * 6);

		int32 TriIndex = 0;

		for (int32 y = 0; y < GridSize; y++)
		{
			for (int32 x = 0; x < GridSize; x++)
			{
				int32 i = x + y * GridSize;

				const FVector2D Percent = FVector2D(x, y) / (GridSize - 1);
				const FVector PointOnUnitCube = LocalUp + (Percent.X - 0.5) * 2.0 * AxisA + (Percent.Y - 0.5) * 2.0 * AxisB;
				
				Vertices[i] = Gen->CalculatePointOnSphere(PointOnUnitCube.GetSafeNormal());
				UV0[i] = FVector2D::UnitVector;

				if (x != GridSize - 1 && y != GridSize - 1)
				{
					Triangles[TriIndex+0] = i;
					Triangles[TriIndex+1] = i + GridSize;
					Triangles[TriIndex+2] = i + GridSize + 1;
					Triangles[TriIndex+3] = i;
					Triangles[TriIndex+4] = i + GridSize + 1;
					Triangles[TriIndex+5] = i + 1;
					TriIndex += 6;
				}
			}	
		}
	}

	CalculateNormals(Vertices, Triangles, Normals);
	
	ProceduralMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, TArray<FLinearColor>(), TArray<FProcMeshTangent>(), false);
	ProceduralMesh->SetMaterial(0, Material);

	double End = FPlatformTime::Seconds();
	UE_LOG(LogTemp, Warning, TEXT("Face took %f to build. with size of %i %s %p"), End - Start, GridSize, *GetReadableName(), ProceduralMesh);
}

void UTerrianMesh::SetShapeSettings(UShapeSettings* NewShapeSettings)
{
	ShapeSettings = NewShapeSettings;
	RebuildMesh();
}

void UTerrianMesh::BeginPlay()
{
	Super::BeginPlay();
}

void UTerrianMesh::BeginDestroy()
{
	UnbindPropertyChangeDelegate();
	Super::BeginDestroy();
}

void UTerrianMesh::UpdateAxis()
{
	AxisA = FVector(LocalUp.Y, LocalUp.Z, LocalUp.X);
	AxisB = LocalUp.Cross(AxisA);
	
}

void UTerrianMesh::RebindDelegates()
{
	
	if (OnNoiseSettingsChangedDelegateHandle.IsValid())
	{
		OnNoiseSettingsChangedDelegateHandle.Reset();
	}

	if (OnShapeSettingsChangedDelegateHandle.IsValid())
	{
		OnNoiseSettingsChangedDelegateHandle.Reset();
	}

	if (ShapeSettings)
	{
		ShapeSettings->OnItemDataPropertyChangedDelegate.AddUObject(this, &UTerrianMesh::OnExternalPropChanged);
		
		if (ShapeSettings->NoiseSettings)
		{
			ShapeSettings->NoiseSettings->FOnItemDataPropertyChangedDelegate.AddUObject(this, &UTerrianMesh::OnExternalPropChanged);
		}
	}

}

void UTerrianMesh::UnbindPropertyChangeDelegate()
{
	if (ShapeSettings && OnShapeSettingsChangedDelegateHandle.IsValid())
	{
		ShapeSettings->OnItemDataPropertyChangedDelegate.Remove(OnShapeSettingsChangedDelegateHandle);

		if (ShapeSettings->NoiseSettings && OnNoiseSettingsChangedDelegateHandle.IsValid())
		{
			ShapeSettings->NoiseSettings->FOnItemDataPropertyChangedDelegate.Remove(OnNoiseSettingsChangedDelegateHandle);
		}
	}
}

void UTerrianMesh::OnExternalPropChanged(const FName PropertyName)
{
	bDirty = true;
	RebuildMesh();
}

void UTerrianMesh::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bDirty)
	{
		bDirty = false;
		RebuildMesh();
	}
}

