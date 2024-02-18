// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PLANETGENERATOR_API UMyBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (ScriptMethod))
	static FVector MapSphereToCube(FVector Point);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (ScriptMethod))
	static FVector ClosestPointOnCube(const FVector& cubePos, float cubeSize, const FVector& testPoint);


	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (ScriptMethod))
	static FVector CubizePoint(FVector Position);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (ScriptMethod))
	static float CubeInSphereLength(float Radius);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (ScriptMethod))
	static FVector ClosestPointOnSphere(const FVector& SphereCenter, const float SphereRadius, const FVector& TestPoint);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (ScriptMethod))
	static FVector ClosestPointOnSphereUnit(const FVector& SphereCenter, const FVector& TestPoint);
};
