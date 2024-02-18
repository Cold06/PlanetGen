// Fill out your copyright notice in the Description page of Project Settings.


#include "PlanetLodManager.h"

#include <functional>

#include "MyBlueprintFunctionLibrary.h"
#include "Camera/CameraComponent.h"

// Sets default values
APlanetLodManager::APlanetLodManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SceneRoot  = CreateDefaultSubobject<USceneComponent>(TEXT("ROOT")); 
	RootComponent = SceneRoot;
}

void APlanetLodManager::SetShapeSettings(UShapeSettings* NewShapeSettings)
{
	ShapeSettings = NewShapeSettings;
}

void APlanetLodManager::BPSetMaterial(UMaterialInterface* NewLandscapeMaterial)
{
	Material = NewLandscapeMaterial;
}

// Called when the game starts or when spawned
void APlanetLodManager::BeginPlay()
{
	Super::BeginPlay();
}

FVector GetMajorAxis(FVector A)
{
	FVector out(0, 0, 0);

	float X = FMath::Abs(A.X);
	float Y = FMath::Abs(A.X);
	float Z = FMath::Abs(A.X);

	if (X > Y && X > Z)
	{
		out.X = 1.f * FMath::Sign(A.X);
	}
	if (Y > X && Y > Z)
	{
		out.Y = 1.f * FMath::Sign(A.Y);
	}
	if (Z > X && Z > Y)
	{
		out.Z = 1.f * FMath::Sign(A.Z);
	}

	return out;
}


FVector2D GetPointGivenFace(FVector Face, FVector Point)
{
	if (Face == FVector(0.f, 0.f, +1.f))
		return FVector2D(Point.X, Point.Y);
	if (Face == FVector(0.f, 0.f, -1.f))
		return FVector2D(Point.X, Point.Y);

	if (Face == FVector(0.f, +1.f, 0.f))
		return FVector2D(Point.X, Point.Z);
	if (Face == FVector(0.f, -1.f, 0.f))
		return FVector2D(Point.X, Point.Z);

	if (Face == FVector(+1.f, 0.f, 0.f))
		return FVector2D(Point.Y, Point.Z);
	if (Face == FVector(-1.f, 0.f, 0.f))
		return FVector2D(Point.Y, Point.Z);

	return FVector2D::ZeroVector;
}

float CubeSideLengthInsideSphere(float SphereRadius)
{
	// Calculate the side length of the cube that fits inside a sphere of radius R
	float SideLength = FMath::Sqrt(3.0f) / 3.0f * (2.0f * SphereRadius);
	return SideLength;
}

static TArray<FVector> GLOBAL_Directions = {
	{0.f, 0.f, +1.f},
	{0.f, 0.f, -1.f},
	{0.f, +1.f, 0.f},
	{0.f, -1.f, 0.f},
	{+1.f, 0.f, 0.f},
	{-1.f, 0.f, 0.f},
};



struct QuadTreeNode
{
	CellType nodeType;
	FVector2D offset; // Offset from the parent node
	float size; // Size of the node
	int depth; // Depth of the node in the tree
	bool bHasPoint; // Flag indicating if the node has a point
	FVector2D point; // The point contained in the node
	FVector Up; // The point contained in the node
	TArray<QuadTreeNode*> children; // Pointers to children nodes

	NodeKey getKey()
	{
		NodeKey key;
		key.Offset = offset;
		key.nodeType = nodeType;
		key.Size = size;
		key.Depth = depth;
		key.Up = Up;
		return key;
	}
	
	QuadTreeNode(FVector2D _offset, float _size, int _depth, FVector _Up) : offset(_offset), size(_size), depth(_depth),
	                                                           bHasPoint(false), Up(_Up)
	{
		nodeType = CellType::Final;
		point = FVector2D::ZeroVector;
		children.Init(nullptr, 4); // Four children for a quad tree
	}
};


// Function to insert a point into a quad tree node
void InsertPoint(QuadTreeNode* node, const FVector2D& point)
{
	if (node->depth == 0)
	{
		node->nodeType = CellType::Player;
		node->point = point;
		node->bHasPoint = true;
	} else {
		float halfSize = node->size * 0.5f;
		node->nodeType = CellType::Holding;
		// LEFT TOP 
		node->children[0] = new QuadTreeNode(node->offset + FVector2D(0, 0), halfSize, node->depth - 1, node->Up);
		// RIGHT TOP 
		node->children[1] = new QuadTreeNode(node->offset + FVector2D(halfSize, 0), halfSize, node->depth - 1, node->Up);
		// LEFT BOTTOM 
		node->children[2] = new QuadTreeNode(node->offset + FVector2D(0, halfSize), halfSize, node->depth - 1, node->Up);
		// RIGHT BOTTOM
		node->children[3] = new QuadTreeNode(node->offset + FVector2D(halfSize, halfSize), halfSize, node->depth - 1, node->Up);

		if (point.X < node->offset.X + halfSize)
		{
			// LEFT
			if (point.Y < node->offset.Y + halfSize)
			{
				// TOP
				InsertPoint(node->children[0], point);
			} else {
				// BOTTOM
				InsertPoint(node->children[2], point);
			}	
		} else {
			// RIGhT
			if (point.Y < node->offset.Y + halfSize)
			{
				// TOP
				InsertPoint(node->children[1], point);
			} else {
				// BOTTOM
				InsertPoint(node->children[3], point);
			}	
		}
	}
}


// Function to prebuild the entire quad tree
QuadTreeNode* PrebuildQuadTree(const FVector2D& point, float size, int maxDepth, FVector Up)
{
	QuadTreeNode* root = new QuadTreeNode(FVector2D(0, 0), size, maxDepth, Up);
	InsertPoint(root, point);
	return root;
}

TArray<QuadTreeNode*> FlattenQuadTree(QuadTreeNode* Node)
{
	TArray<QuadTreeNode*> FlattenedTree;
	FlattenedTree.Add(Node);

	for (auto Child : Node->children)
	{
		if (Child != nullptr)
		{
			TArray<QuadTreeNode*> ChildFlattenedTree = FlattenQuadTree(Child);
			FlattenedTree.Append(ChildFlattenedTree);
		}
	}

	return FlattenedTree;
}


template<typename Lambda>
void DrawBox(const FVector& Start, const FVector& End, const Lambda& DrawLineFunc)
{
	// Draw lines between each pair of corresponding components of start and end vectors
	DrawLineFunc(Start, FVector(Start.X, Start.Y, End.Z));
	DrawLineFunc(Start, FVector(Start.X, End.Y, Start.Z));
	DrawLineFunc(Start, FVector(End.X, Start.Y, Start.Z));

	DrawLineFunc(End, FVector(End.X, End.Y, Start.Z));
	DrawLineFunc(End, FVector(End.X, Start.Y, End.Z));
	DrawLineFunc(End, FVector(Start.X, End.Y, End.Z));

	DrawLineFunc(FVector(Start.X, End.Y, Start.Z), FVector(Start.X, End.Y, End.Z));
	DrawLineFunc(FVector(Start.X, End.Y, Start.Z), FVector(End.X, End.Y, Start.Z));
	DrawLineFunc(FVector(Start.X, Start.Y, End.Z), FVector(Start.X, End.Y, End.Z));
	DrawLineFunc(FVector(Start.X, Start.Y, End.Z), FVector(End.X, Start.Y, End.Z));
	DrawLineFunc(FVector(End.X, Start.Y, Start.Z), FVector(End.X, End.Y, Start.Z));
	DrawLineFunc(FVector(End.X, Start.Y, Start.Z), FVector(End.X, Start.Y, End.Z));
}



// Called every frame
void APlanetLodManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float Radius = 4000;

	FVector ActorLocation = GetActorLocation();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	if (PlayerController)
	{
		// Get the camera component attached to the player controller
		auto Manager = PlayerController->PlayerCameraManager;

		FVector CameraLocation = Manager->GetCameraLocation();

		float SideLength = UMyBlueprintFunctionLibrary::CubeInSphereLength(Radius);


		DrawDebugBox(GetWorld(), ActorLocation, FVector(SideLength), FColor::Red, false, 1.f);
		DrawDebugSphere(GetWorld(), ActorLocation, Radius, 10, FColor::Yellow, false, 1.f);

		FVector ClosesPoint = UMyBlueprintFunctionLibrary::ClosestPointOnSphere(ActorLocation, Radius, CameraLocation);

		FVector UnitPoint = UMyBlueprintFunctionLibrary::ClosestPointOnSphereUnit(ActorLocation, CameraLocation);
		FVector ToCube = UMyBlueprintFunctionLibrary::CubizePoint(UnitPoint);
		FVector CubePos = (ToCube * SideLength) + ActorLocation;

		

		DrawDebugBox(GetWorld(), ClosesPoint, FVector(3), FColor::Green, false, 1.f);
		DrawDebugBox(GetWorld(), CubePos, FVector(3), FColor::Cyan, false, 1.f);

		FVector2D Out;

		FVector Quad(1);

		FVector Direction(0);

		if (ToCube.X == 1.f || ToCube.X == -1.f)
		{
			Out.X = ToCube.Z;
			Out.Y = ToCube.Y;
			Direction.X = ToCube.X;
			GEngine->AddOnScreenDebugMessage(3, 1, FColor::White, FString::Printf(TEXT("Towards X %f"), Direction.X));
			if (ToCube.X == 1.f)
			{
				Out.Y *= -1.f;
			} else
			{
				Out *= -1.f;
			}
		}
		else if (ToCube.Y == 1.f || ToCube.Y == -1.f)
		{
			Out.X = ToCube.X;
			Out.Y = ToCube.Z;
			Direction.Y = ToCube.Y;
			GEngine->AddOnScreenDebugMessage(3, 1, FColor::White, FString::Printf(TEXT("Towards Y %f"), Direction.Y));
			if (ToCube.Y == 1.f)
			{
				Out.Y *= -1.f;
			} else
			{
				Out *= -1.f;
			}
		}
		else if (ToCube.Z == 1.f || ToCube.Z == -1.f)
		{
			Out.X = ToCube.Y;
			Out.Y = ToCube.X;
			Direction.Z = ToCube.Z;
			GEngine->AddOnScreenDebugMessage(3, 1, FColor::White, FString::Printf(TEXT("Towards Z %f"), Direction.Z));
			if (ToCube.Z == 1.f)
			{
				Out.Y *= -1.f;
			} else
			{
				Out *= -1.f;
			}
		}

		Out.X += 1.f;
		Out.Y += 1.f;
		Out.X *= 0.5;
		Out.Y *= 0.5;

		GEngine->AddOnScreenDebugMessage(1, 1, FColor::White, FString::Printf(TEXT("Plane %s"), *Out.ToString()));

		float depth = 3;
		
		auto Node = PrebuildQuadTree(Out, 1.0, (int)depth, Direction);
		auto Elements = FlattenQuadTree(Node);
		GEngine->AddOnScreenDebugMessage(2, 1, FColor::White, FString::Printf(TEXT("TreeSize %i"), Elements.Num()));

		FVector AxisA = FVector(Direction.Y, Direction.Z, Direction.X);
		FVector AxisB = Direction.Cross(AxisA);

		TSet<NodeKey> CurrentWorkingSet;
		
		for (auto XNode : Elements)
		{
			
			const FVector Start = Direction + (XNode->offset.X - 0.5) * 2.0 * AxisA + (XNode->offset.Y - 0.5) * 2.0 * AxisB;
			const FVector End = Direction + (XNode->offset.X + XNode->size - 0.5) * 2.0 * AxisA + (XNode->offset.Y + XNode->size - 0.5) * 2.0 * AxisB;

			//DrawDebugBox(GetWorld(), ActorLocation + Start * SideLength, Direction + (((Quad - Direction) * SideLength * XNode->size *  (XNode->bHasPoint ? 0.8 : 0.95))), XNode->bHasPoint ? FColor::Green : FColor::White, false);

			// DrawDebugBox(GetWorld(), ActorLocation + Start * SideLength, FVector(3), XNode->bHasPoint ? FColor::Green : FColor::White, false);

			auto S = ActorLocation + Start * SideLength;
			auto E = ActorLocation + End * SideLength;
			
			DrawBox(S, E, [this, XNode](FVector St, FVector Ed){
				CellType typeCell = XNode->bHasPoint ? CellType::Player : XNode->children[0] == nullptr ? CellType::Final : CellType::Holding;
				
				if (typeCell != CellType::Holding)
					DrawDebugLine(GetWorld(), St, Ed, typeCell == CellType::Final? FColor::Green : FColor::Red, false, 1.f);
			});

			if (XNode->nodeType == CellType::Player || XNode->nodeType == CellType::Final)
			{
				CurrentWorkingSet.Add(XNode->getKey());	
			}
		}

		TSet<NodeKey> ToCreate;
		TSet<NodeKey> ToDelete;
		int32 ToKeep = 0;

		for (auto Existing : MeshCache)
		{
			auto Key = Existing.Get<0>();
			auto Value = Existing.Get<1>();

			if (CurrentWorkingSet.Contains(Key))
			{
				ToKeep += 1.0;
			} else
			{
				ToDelete.Add(Key);
			}
		}

		for (auto MaybeNewKey : CurrentWorkingSet)
		{
			if (!MeshCache.Contains((MaybeNewKey)))
			{
				ToCreate.Add(MaybeNewKey);
			}
		}

		GEngine->AddOnScreenDebugMessage(9, 1, FColor::White, FString::Printf(TEXT("LOD Tick Stats")));
		GEngine->AddOnScreenDebugMessage(8, 1, FColor::White, FString::Printf(TEXT(" . . . To Create %i"), ToCreate.Num()));
		GEngine->AddOnScreenDebugMessage(7, 1, FColor::White, FString::Printf(TEXT(" . . . To Delete %i"), ToDelete.Num()));
		GEngine->AddOnScreenDebugMessage(6, 1, FColor::White, FString::Printf(TEXT(" . . . To Keep %i"), ToKeep));

		for (auto Elem : ToCreate)
		{
		
			FName Name(FString::Printf(TEXT("MESH_INSTANCE=%i-%s"), Elem.Depth, *Elem.Offset.ToString()));
			auto NewMesh = NewObject<UTerrianMesh>(this, UTerrianMesh::StaticClass(), Name);
			NewMesh->RegisterComponent();
			AddInstanceComponent(NewMesh);
			NewMesh->AttachToComponent(SceneRoot, FAttachmentTransformRules::KeepRelativeTransform);
			NewMesh->SetComponentTickEnabled(true);
			NewMesh->SampleFocus = Elem.Size;
			NewMesh->SampleStart = Elem.Offset;
			NewMesh->ShapeSettings = ShapeSettings;
			NewMesh->GridSize = 20;
			NewMesh->LocalUp = Direction;
			NewMesh->Material = Material;
			static FName Reason("Spawn");
			NewMesh->RebuildMesh(Reason);
			NewMesh->bDirty = true;
			NewMesh->Activate();
			
			
			MeshCache.Add(Elem, NewMesh);
		}
		
		for (auto Elem : ToDelete)
		{
			auto ToRemove = MeshCache[Elem];
			MeshCache.Remove(Elem);
			if (IsValid(ToRemove))
			{
				RemoveInstanceComponent(ToRemove);
				ToRemove->DestroyComponent();
			}
		}
		
		//
		// if (Manager)
		// {
		// 	FVector CubePos = GetActorLocation();
		// 	FVector CameraLocation = Manager->GetCameraLocation();
		// 	FVector Distance = GetActorLocation() - CameraLocation;
		// 	float Mag = Distance.Length();
		// 	UE_LOG(LogTemp, Warning, TEXT("Distance: %s, total %f"), *Distance.ToString(), Mag);
		//
		// 	TArray<std::tuple<FVector, QuadTreeNode>> Nodes;
		// 	
		// 	static TArray<FVector> Directions = {
		// 		{0.f, 0.f, +1.f},
		// 		{0.f, 0.f, -1.f},
		// 		{0.f, +1.f, 0.f},
		// 		{0.f, -1.f, 0.f},
		// 		{+1.f, 0.f, 0.f},
		// 		{-1.f, 0.f, 0.f},
		// 	};
		//
		// 	const auto Major = GetMajorAxis(Distance);
		// 	
		// 	for (auto Direction : Directions)
		// 	{
		//
		// 		FVector AxisA = FVector(Direction.Y, Direction.Z, Direction.X);
		// 		FVector AxisB = Direction.Cross(AxisA);
		// 		
		// 		auto GetPointOnUnitCube = [Direction, AxisA, AxisB](float x, float y)
		// 		{
		// 			float Focus = 1.f;
		// 			FVector2D SampleStart = FVector2D::ZeroVector;
		// 			const FVector2D Percent = SampleStart + (FVector2D(x, y) * Focus) / 1;
		// 			const FVector PointOnUnitCube = Direction + (Percent.X - 0.5) * 2.0 * AxisA + (Percent.Y - 0.5) * 2.0 * AxisB;
		// 			return PointOnUnitCube;
		// 		};
		// 		
		// 		if (Direction == Major)
		// 		{
		//
		// 			float CubeLength = CubeSideLengthInsideSphere(Radius);
		// 			
		// 			FVector CLosest = UMyBlueprintFunctionLibrary::ClosestPointOnCube(CubePos, CubeLength, CameraLocation);
		// 			FVector2D PointOnFace = GetPointGivenFace(Direction, CLosest);
		//
		// 			auto Tree = buildQuadTree(PointOnFace / CubeLength, 5);
		//
		//
		// 			
		// 		} else
		// 		{
		// 			// Direction;
		// 			// auto X = new QuadTreeNode(FVector2D(0.0, 0.0), 1, 0);
		// 		}
		// 	}
		// }
	}
}
