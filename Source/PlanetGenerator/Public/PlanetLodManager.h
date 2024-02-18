// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LodDebugDraw.h"
#include "TerrianMesh.h"
#include "GameFramework/Actor.h"
#include "PlanetLodManager.generated.h"


struct NodeKey
{
	FVector2D Offset;
	int Depth;

	// Comparison operators for NodeKey
	friend bool operator==(const NodeKey& A, const NodeKey& B)
	{
		return A.Offset == B.Offset && A.Depth == B.Depth;
	}

	friend uint32 GetTypeHash(const NodeKey& Key)
	{
		// Use a combination of hash values for individual components
		uint32 Hash = FCrc::MemCrc_DEPRECATED(&Key.Offset, sizeof(FVector2D));
		Hash = FCrc::MemCrc_DEPRECATED(&Key.Depth, sizeof(int), Hash);
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

	TMap<NodeKey, bool> MeshCache;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
