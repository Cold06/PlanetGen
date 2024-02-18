// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LodDebugDraw.h"
#include "TerrianMesh.h"
#include "GameFramework/Actor.h"
#include "PlanetLodManager.generated.h"

enum class CellType: uint8
{
	// A cell that just holds other cells
	Holding,
	// One cell that has no other children so the mesh is visible
	Final,
	// The lod zero cell
	Player,
};


struct NodeKey
{
	FVector Up;
	FVector2D Offset;
	int Depth;
	float Size;
	CellType nodeType;

	static NodeKey LodZero(FVector Up, int Depth)
	{
		NodeKey NewKey;
		NewKey.Up = Up;
		NewKey.Offset = FVector2D(0.f);
		NewKey.Depth = Depth;
		NewKey.Size = 1.f;
		NewKey.nodeType = CellType::Final;
		return NewKey;
	}
	
	// Comparison operators for NodeKey
	friend bool operator==(const NodeKey& A, const NodeKey& B)
	{
		return A.Offset == B.Offset && A.Depth == B.Depth && A.Size == B.Size && A.Up == B.Up && A.nodeType == B.nodeType;
	}

	friend uint32 GetTypeHash(const NodeKey& Key)
	{
		// Use a combination of hash values for individual components
		uint32 Hash = FCrc::MemCrc_DEPRECATED(&Key.Offset, sizeof(FVector2D));
		Hash = FCrc::MemCrc_DEPRECATED(&Key.Depth, sizeof(int), Hash);
		Hash = FCrc::MemCrc_DEPRECATED(&Key.Size, sizeof(float), Hash);
		Hash = FCrc::MemCrc_DEPRECATED(&Key.nodeType, sizeof(CellType), Hash);
		Hash = FCrc::MemCrc_DEPRECATED(&Key.Up, sizeof(FVector), Hash);
		return Hash;
	}
};



UCLASS()
class PLANETGENERATOR_API APlanetLodManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlanetLodManager();

	TMap<NodeKey, UTerrianMesh*> MeshCache;

	TMap<FVector, UTerrianMesh*> LodZero;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* SceneRoot;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UMaterialInterface> Material;

	UPROPERTY(BlueprintReadWrite, EditAnywhere);
	UShapeSettings* ShapeSettings;

	UFUNCTION(BlueprintSetter)
	void SetShapeSettings(UShapeSettings* NewShapeSettings);

	UFUNCTION(BlueprintSetter)
	void BPSetMaterial(UMaterialInterface* NewLandscapeMaterial);

	UFUNCTION(BlueprintCallable)
	void PrintQuadTree();

	bool bShouldPrint = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
