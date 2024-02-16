#include "PlanetGenerator/Public/ProceduralNoiseMesh.h"

float NoiseFunction(float x, float y)
{
	return 0;
}

UProceduralNoiseMesh::UProceduralNoiseMesh()
{
	MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));

	PrimaryComponentTick.bCanEverTick = true;

	{
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

				const float NoiseValue = NoiseFunction(x * StepSize, y * StepSize);
				
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

		MeshComponent->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, TArray<FLinearColor>(), TArray<FProcMeshTangent>(), true);
		MeshComponent->SetMaterial(0, nullptr);
	}
	
	MeshComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
}


void UProceduralNoiseMesh::BeginPlay()
{
	Super::BeginPlay();

	
	
}

void UProceduralNoiseMesh::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
