#pragma once
#include "Intersection.hpp"

namespace CommonUtilities
{
	//Tested
	template<typename T>
	void ResolveCollisionSphereSphere(Sphere<T>& aSphere0, Sphere<T>& aSphere1, Vector3<T>& aIntersectionPoint, T& aPenetrationDepth);

	//Tested
	template<typename T>
	void ResolveCollisionSphereStaticPoint(Sphere<T>& aSphere, Vector3<T>& outIntersectionPoint, T& aPenetrationDepth);

	//Tested
	template<typename T>
	void ResolveCollisionDynamicAABBStaticAABB(AABB3D<T>& aDynamicAABB, const AABB3D<T>& aStaticAABB);

	//Tested
	template<typename T>
	void ResolveCollisionDynamicAABBDynamicAABB(AABB3D<T>& aDynamicAABB0, AABB3D<T>& aDynamicAABB1);
}

template<typename T>
inline void CommonUtilities::ResolveCollisionSphereSphere(Sphere<T>& aSphere0, Sphere<T>& aSphere1, Vector3<T>& aIntersectionPoint, T& aPenetrationDepth)
{
	Vector3<T> direction = (aIntersectionPoint - aSphere0.GetPosition()).GetNormalized();
	Vector3<T> distance = direction * (aPenetrationDepth * static_cast<T>(0.5));
	aSphere0.SetPosition(aSphere0.GetPosition() - distance);
	aSphere1.SetPosition(aSphere1.GetPosition() + distance);
}

template<typename T>
inline void CommonUtilities::ResolveCollisionSphereStaticPoint(Sphere<T>& aSphere, Vector3<T>& aIntersectionPoint, T& aPenetrationDepth)
{
	Vector3<T> direction = (aIntersectionPoint - aSphere.GetPosition()).GetNormalized();
	aSphere.SetPosition(aSphere.GetPosition() - (direction * aPenetrationDepth));
}

template<typename T>
inline void CommonUtilities::ResolveCollisionDynamicAABBStaticAABB(AABB3D<T>& aDynamicAABB, const AABB3D<T>& aStaticAABB)
{
	Vector3<T> dynamicAABBMin = aDynamicAABB.GetMin();
	Vector3<T> dynamicAABBMax = aDynamicAABB.GetMax();

	Vector3<T> aStaticAABBMin = aStaticAABB.GetMin();
	Vector3<T> aStaticAABBMax = aStaticAABB.GetMax();

	Vector3<T> direction = (aDynamicAABB.GetCenterPoistion() - aStaticAABB.GetCenterPoistion());
	T largestAxis = std::max(abs(direction.x), std::max(abs(direction.y), abs(direction.z)));
	T distance;

	if (largestAxis == direction.x)
	{
		direction = { T(1),T(0),T(0) };
		distance = aStaticAABB.GetMax().x - dynamicAABBMin.x;
	}
	else if (largestAxis == direction.y)
	{
		direction = { T(0),T(1),T(0) };
		distance = aStaticAABB.GetMax().y - dynamicAABBMin.y;
	}
	else if (largestAxis == direction.z)
	{
		direction = { T(0),T(0),T(1) };
		distance = aStaticAABB.GetMax().z - dynamicAABBMin.z;
	}
	else if (-largestAxis == direction.x)
	{
		direction = { T(-1),T(0),T(0) };
		distance = dynamicAABBMax.x - aStaticAABBMin.x;
	}
	else if (-largestAxis == direction.y)
	{
		direction = { T(0),T(-1),T(0) };
		distance = dynamicAABBMax.y - aStaticAABBMin.y;
	}
	else if (-largestAxis == direction.z)
	{
		direction = { T(0),T(0),T(-1) };
		distance = dynamicAABBMax.z - aStaticAABBMin.z;
	}
	aDynamicAABB.SetCenterPoistion(aDynamicAABB.GetCenterPoistion() + (direction * distance));
}

template<typename T>
inline void CommonUtilities::ResolveCollisionDynamicAABBDynamicAABB(AABB3D<T>& aDynamicAABB0, AABB3D<T>& aDynamicAABB1)
{
	Vector3<T> dynamicAABB0Min = aDynamicAABB0.GetMin();
	Vector3<T> dynamicAABB0Max = aDynamicAABB0.GetMax();

	Vector3<T> dynamicAABB1Min = aDynamicAABB1.GetMin();
	Vector3<T> dynamicAABB1Max = aDynamicAABB1.GetMax();

	Vector3<T> direction = (aDynamicAABB0.GetCenterPoistion() - aDynamicAABB1.GetCenterPoistion());
	T largestAxis = std::max(abs(direction.x), std::max(abs(direction.y), abs(direction.z)));
	T distance = T(0);

	if (largestAxis == direction.x)
	{
		direction = { T(1),T(0),T(0) };
		distance = aDynamicAABB1.GetMax().x - dynamicAABB0Min.x;
	}
	else if (largestAxis == direction.y)
	{
		direction = { T(0),T(1),T(0) };
		distance = aDynamicAABB1.GetMax().y - dynamicAABB0Min.y;
	}
	else if (largestAxis == direction.z)
	{
		direction = { T(0),T(0),T(1) };
		distance = aDynamicAABB1.GetMax().z - dynamicAABB0Min.z;
	}
	else if (-largestAxis == direction.x)
	{
		direction = { T(-1),T(0),T(0) };
		distance = dynamicAABB0Max.x - dynamicAABB1Min.x;
	}
	else if (-largestAxis == direction.y)
	{
		direction = { T(0),T(-1),T(0) };
		distance = dynamicAABB0Max.y - dynamicAABB1Min.y;
	}
	else if (-largestAxis == direction.z)
	{
		direction = { T(0),T(0),T(-1) };
		distance = dynamicAABB0Max.z - dynamicAABB1Min.z;
	}
	aDynamicAABB0.SetCenterPoistion(aDynamicAABB0.GetCenterPoistion() + (direction * distance));
	aDynamicAABB1.SetCenterPoistion(aDynamicAABB1.GetCenterPoistion() - (direction * distance));
}