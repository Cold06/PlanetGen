// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBlueprintFunctionLibrary.h"

FVector UMyBlueprintFunctionLibrary::MapSphereToCube(FVector p)
{
	// Check if the point is normalized
	const float EPSILON = 1e-6f;
	if (FMath::Abs(p.Dot(p) - 1.0f) > EPSILON)
	{
		UE_LOG(LogTemp, Error, TEXT("Input point must be on the unit sphere."));
		return FVector::ZeroVector;
	}

	// Determine the dominant axis (axis with largest absolute value)
	int32 dominantAxis = FMath::Abs(p.X) >= FMath::Abs(p.Y) && FMath::Abs(p.X) >= FMath::Abs(p.Z)
		                     ? 0
		                     : FMath::Abs(p.Y) >= FMath::Abs(p.Z)
		                     ? 1
		                     : 2;

	// Calculate the corresponding component on the cube face
	float cubeComponent = FMath::Sqrt(1.0f - FMath::Square(FMath::Abs(p[dominantAxis])) * 0.5f);

	// Calculate the remaining two components based on the dominant axis
	float s, t;
	switch (dominantAxis)
	{
	case 0: // X-axis
		s = p.Y * cubeComponent;
		t = p.Z * cubeComponent;
		break;
	case 1: // Y-axis
		s = p.X * cubeComponent;
		t = p.Z * cubeComponent;
		break;
	case 2: // Z-axis
		s = p.X * cubeComponent;
		t = p.Y * cubeComponent;
		break;
	}

	// Set the sign of the cube components according to the original point
	cubeComponent = FMath::Sign(p[dominantAxis]) * cubeComponent;
	s = FMath::Sign(p[dominantAxis]) * s;
	t = FMath::Sign(p[dominantAxis]) * t;

	// Return the point on the unit cube
	return FVector(cubeComponent, s, t);
}

FVector UMyBlueprintFunctionLibrary::ClosestPointOnCube(const FVector& CubePosition, float CubeSize,
                                                        const FVector& TestPoint)
{
	// Calculate the center of the cube
	FVector CubeCenter = CubePosition;

	// Calculate the half-extents of the cube
	float HalfSize = CubeSize;

	// Clamp the test point's coordinates to the extents of the cube
	FVector ClosestPoint(
		FMath::Clamp(TestPoint.X, CubeCenter.X - HalfSize, CubeCenter.X + HalfSize),
		FMath::Clamp(TestPoint.Y, CubeCenter.Y - HalfSize, CubeCenter.Y + HalfSize),
		FMath::Clamp(TestPoint.Z, CubeCenter.Z - HalfSize, CubeCenter.Z + HalfSize)
	);

	return ClosestPoint;
}

FVector UMyBlueprintFunctionLibrary::CubizePoint(FVector InPosition)
{
	FVector Position;
	
	double x, y, z;
	x = InPosition.X;
	y = InPosition.Y;
	z = InPosition.Z;

	double fx, fy, fz;
	fx = FMath::Abs(x);
	fy = FMath::Abs(y);
	fz = FMath::Abs(z);

	const double inverseSqrt2 = 0.70710676908493042;

	if (fy >= fx && fy >= fz)
	{
		double a2 = x * x * 2.0;
		double b2 = z * z * 2.0;
		double inner = -a2 + b2 - 3;
		double innersqrt = -FMath::Sqrt((inner * inner) - 12.0 * a2);

		if (x == 0.0 || x == -0.0)
		{
			Position.X = 0.0;
		}
		else
		{
			Position.X = FMath::Sqrt(innersqrt + a2 - b2 + 3.0) * inverseSqrt2;
		}

		if (z == 0.0 || z == -0.0)
		{
			Position.Z = 0.0;
		}
		else
		{
			Position.Z = FMath::Sqrt(innersqrt - a2 + b2 + 3.0) * inverseSqrt2;
		}

		if (Position.X > 1.0) Position.X = 1.0;
		if (Position.Z > 1.0) Position.Z = 1.0;

		if (x < 0) Position.X = -Position.X;
		if (z < 0) Position.Z = -Position.Z;

		if (y > 0)
		{
			Position.Y = 1.0;
		}
		else
		{
			Position.Y = -1.0;
		}
	}
	else if (fx >= fy && fx >= fz)
	{
		double a2 = y * y * 2.0;
		double b2 = z * z * 2.0;
		double inner = -a2 + b2 - 3;
		double innersqrt = -FMath::Sqrt((inner * inner) - 12.0 * a2);

		if (y == 0.0 || y == -0.0)
		{
			Position.Y = 0.0;
		}
		else
		{
			Position.Y = FMath::Sqrt(innersqrt + a2 - b2 + 3.0) * inverseSqrt2;
		}

		if (z == 0.0 || z == -0.0)
		{
			Position.Z = 0.0;
		}
		else
		{
			Position.Z = FMath::Sqrt(innersqrt - a2 + b2 + 3.0) * inverseSqrt2;
		}

		if (Position.Y > 1.0) Position.Y = 1.0;
		if (Position.Z > 1.0) Position.Z = 1.0;

		if (y < 0) Position.Y = -Position.Y;
		if (z < 0) Position.Z = -Position.Z;

		if (x > 0)
		{
			// right face
			Position.X = 1.0;
		}
		else
		{
			// left face
			Position.X = -1.0;
		}
	}
	else
	{
		double a2 = x * x * 2.0;
		double b2 = y * y * 2.0;
		double inner = -a2 + b2 - 3;
		double innersqrt = -FMath::Sqrt((inner * inner) - 12.0 * a2);

		if (x == 0.0 || x == -0.0)
		{
			Position.X = 0.0;
		}
		else
		{
			Position.X = FMath::Sqrt(innersqrt + a2 - b2 + 3.0) * inverseSqrt2;
		}

		if (y == 0.0 || y == -0.0)
		{
			Position.Y = 0.0;
		}
		else
		{
			Position.Y = FMath::Sqrt(innersqrt - a2 + b2 + 3.0) * inverseSqrt2;
		}

		if (Position.X > 1.0) Position.X = 1.0;
		if (Position.Y > 1.0) Position.Y = 1.0;

		if (x < 0) Position.X = -Position.X;
		if (y < 0) Position.Y = -Position.Y;

		if (z > 0)
		{
			// front face
			Position.Z = 1.0;
		}
		else
		{
			// back face
			Position.Z = -1.0;
		}

	}
	return Position;

}

float UMyBlueprintFunctionLibrary::CubeInSphereLength(float Radius)
{
		// Calculate the side length of the cube that fits inside a sphere of radius R
		float SideLength = FMath::Sqrt(3.0f) / 3.0f * (2.0f * Radius);
		return SideLength / 2.f;
}

FVector UMyBlueprintFunctionLibrary::ClosestPointOnSphere(const FVector& SphereCenter, const float SphereRadius,
	const FVector& TestPoint)
{
	// Calculate vector from sphere center to test point
	FVector SphereToTest = TestPoint - SphereCenter;

	// Normalize the vector
	auto Len = SphereToTest.Length();

	auto Unit = SphereToTest / Len;

	

	return (Unit.GetSafeNormal() * SphereRadius) + SphereCenter;
}

FVector UMyBlueprintFunctionLibrary::ClosestPointOnSphereUnit(const FVector& SphereCenter, const FVector& TestPoint)
{
	// Calculate vector from sphere center to test point
	FVector SphereToTest = TestPoint - SphereCenter;

	// Normalize the vector
	auto Len = SphereToTest.Length();

	return SphereToTest / Len;
}
