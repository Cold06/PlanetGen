// Fill out your copyright notice in the Description page of Project Settings.


#include "LodDebugDraw.h"

// Sets default values for this component's properties

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



ULodDebugDraw::ULodDebugDraw(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	UpdateAxis();
}

void ULodDebugDraw::SetSampleStart(FVector2D NewSampleStart)
{
	SampleStart = NewSampleStart;
	static FName Reason("SetSampleStart");
	RebuildMesh(Reason);
}

void ULodDebugDraw::SetSampleFocus(float NewSampleFocus)
{
	SampleFocus = NewSampleFocus;
	static FName Reason("SetSampleFocus");
	RebuildMesh(Reason);
}

void ULodDebugDraw::BPSetMaterial(UMaterialInterface* NewLandscapeMaterial)
{
		Material = NewLandscapeMaterial;
    	static FName Reason("BPSetMaterial");
    	RebuildMesh(Reason);
}

void ULodDebugDraw::PostInitProperties()
{
	Super::PostInitProperties();
	static FName Reason("PostInitProperties");
	RebuildMesh(Reason);
}

void ULodDebugDraw::SetGridSize(int32 newValue)
{
	GridSize = newValue;
	static FName Reason("SetGridSize");
	RebuildMesh(Reason);
}

void ULodDebugDraw::SetLocalUp(FVector newValue)
{
	LocalUp = newValue;
	UpdateAxis();
	static FName Reason("SetLocalUp");
	RebuildMesh(Reason);
}

void ULodDebugDraw::UpdateAxis()
{
	AxisA = FVector(LocalUp.Y, LocalUp.Z, LocalUp.X);
	AxisB = LocalUp.Cross(AxisA);
}

void ULodDebugDraw::RebuildMesh(FName Reason)
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UV0;

	{
		Vertices.Init(FVector::ZeroVector, GridSize * GridSize);
		Normals.Init(FVector::ZeroVector, GridSize * GridSize);
		UV0.Init(FVector2D::ZeroVector, GridSize * GridSize);
		Triangles.Init(0 ,(GridSize - 1) * (GridSize - 1) * 6);

		int32 TriIndex = 0;

		for (int32 y = 0; y < GridSize; y++)
		{
			for (int32 x = 0; x < GridSize; x++)
			{
				auto GetPointOnSphere = [this](int32 GridSize, int32 x, int32 y, FVector2D SampleStart, float SampleFocus)
				{
					const FVector2D Percent = SampleStart + (FVector2D(x, y) * SampleFocus) / (GridSize - 1);
					const FVector PointOnUnitCube = LocalUp + (Percent.X - 0.5) * 2.0 * AxisA + (Percent.Y - 0.5) * 2.0 * AxisB;
					return PointOnUnitCube.GetSafeNormal();
				};

				const int32 i = x + y * GridSize;
				Vertices[i] = GetPointOnSphere(GridSize, x, y, SampleStart, SampleFocus);
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
	
	CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, TArray<FLinearColor>(), TArray<FProcMeshTangent>(), false);
	SetMaterial(0, Material);
}



// Called when the game starts
void ULodDebugDraw::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void ULodDebugDraw::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

TStructOnScope<FActorComponentInstanceData> ULodDebugDraw::GetComponentInstanceData() const
{
	TStructOnScope<FActorComponentInstanceData> InstanceData = MakeStructOnScope<FActorComponentInstanceData, FLodDebugDrawInstanceData>(this);
	FLodDebugDrawInstanceData* MyComponentInstanceData = InstanceData.Cast<FLodDebugDrawInstanceData>();
	MyComponentInstanceData->SampleStart = SampleStart;
	MyComponentInstanceData->SampleFocus = SampleFocus;
	MyComponentInstanceData->GridSize = GridSize;
	MyComponentInstanceData->LocalUp = LocalUp;
	MyComponentInstanceData->Material = Material;
	MyComponentInstanceData->Transform = GetComponentTransform();

	return InstanceData;
}


void ULodDebugDraw::ApplyComponentInstanceData(FLodDebugDrawInstanceData* ComponentInstanceData, const bool bPostUCS)
{
	GridSize = ComponentInstanceData->GridSize;
	LocalUp = ComponentInstanceData->LocalUp;
	Material = ComponentInstanceData->Material;
	SampleStart = ComponentInstanceData->SampleStart;
	SampleFocus = ComponentInstanceData->SampleFocus;
	SetWorldTransform(ComponentInstanceData->Transform);

	static FName Reason("ApplyComponentInstanceData");
	RebuildMesh(Reason);
}


