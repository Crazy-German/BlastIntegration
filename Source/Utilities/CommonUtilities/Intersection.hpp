#pragma once
#include "Plane.hpp"
#include "Ray.hpp"
#include "Vector3.hpp"
#include "AABB3D.hpp"
#include "Sphere.hpp"
#include "OBB3D.hpp"
#include <vector>
#include <algorithm>

#include "Capsule.hpp"
#include "CommonMath.hpp"
#include "DebugConsole.h"
#include "Triangle.hpp"

namespace CommonUtilities
{
	//Tested
	template <typename T>
	bool IntersectionPlaneRay(const Plane<T>& aPlane, const Ray<T>& aRay, Vector3<T>& outIntersectionPoint);

	//Tested
	template <typename T>
	bool IntersectionSphereAABB(const Sphere<T>& aSphere, const AABB3D<T>& aAABB3D, Vector3<T>& aOutIntersectionPoint, T& aPenetrationDepthOut);

	//Tested
	template <typename T>
	bool IntersectionAABBRay(const AABB3D<T>& aAABB, const Ray<T>& aRay, Vector3<T>& outIntersectionPoint);

	template <class T>
	bool IntersectionAABBRay(const AABB3D<T>& aAABB, const Ray<T>& aRay);

	//Tested, outIntersectionPoint wrong
	template <typename T>
	bool IntersectionSphereRay(const Sphere<T>& aSphere, const Ray<T>& aRay, Vector3<T>& outIntersectionPoint);

	//Not Tested
	template<typename T>
	bool IntersectionSphereSphere(const Sphere<T>& aSphere0, const Sphere<T>& aSphere1, Vector3<T>& outIntersectionPoint, T& aPenetrationDepthOut);

	//Tested
	template<typename T>
	bool IntersectionAABB3DAABB3D(const AABB3D<T>& aAABB3D1, const AABB3D<T>& aAABB3D2);

	//Tested
	template<typename T>
	bool IntersectionSphereOBB3D(const Sphere<T>& aSphere, const OBB3D<T>& aOBB3D);

	//Tested
	template<typename T>
	bool IntersectionAABB3DOBB3D(const AABB3D<T>& aAABB3D, const OBB3D<T>& aOBB3D);

	//Tested
	template<typename T>
	bool IntersectionOBB3DOBB3D(const OBB3D<T>& aOBB3D1, const OBB3D<T>& aOBB3D2);

	//Tested
	template<typename T>
	bool IntersectionOBB3DRay(const OBB3D<T>& aOBB3D, const Ray<T>& aRay, Vector3<T>& aOutIntersectionPoint);

	//Untested
	template<typename T>
	bool IntersectionCapsuleCapsule(const Capsule<T>& aCapsule1, const Capsule<T>& aCapsule2, T& aDepth, Vector3<T>& aCollisionNormal);

	template<typename T>
	bool IntersectionCapsuleSphere(const Capsule<T>& aCapsule, const Sphere<T>& aSphere, T& aDepth, Vector3<T>& aCollisionNormal);

	template<typename T>
	bool IntersectionCapsuleOBB3D(const Capsule<T>& aCapsule, const OBB3D<T>& aOBB3D);

	template<typename T>
	bool IntersectionCapsuleAABB(const Capsule<T>& aCapsule, const AABB3D<T>& aAABB);

	template<typename T>
	bool IntersectionRayTriangle(const Ray<T>& aRay, const Triangle<T>& aTriangle, Vector3<T>& aOutIntersectionPoint);

	template<typename T>
	bool IntersectionLineLine(const Vector2<T>& aFirstLineFirstPoint, const Vector2<T>& aFirstLineSecondPoint, const Vector2<T>& aSecondLineFirstPoint, const Vector2<T>& aSecondLineSecondPoint, Vector2<T>& aOutIntersectionPoint);
}

template<typename T>
bool CommonUtilities::IntersectionPlaneRay(const Plane<T>& aPlane, const Ray<T>& aRay, Vector3<T>& outIntersectionPoint)
{
	Vector3<T> planeNormal = aPlane.GetNormal();
	Vector3<T> planePoint = aPlane.GetPoint();
	Vector3<T> rayDirection = aRay.GetDirection();
	Vector3<T> rayOrigin = aRay.GetOrigin();

	if (rayDirection.GetNormalized().Dot(planeNormal) == 0)
	{
		if (planeNormal.Dot(rayOrigin - planePoint) == 0)
		{
			return true;
		}
		return false;
	}
	else
	{
		T t = ((planePoint.Dot(planeNormal)) - (rayOrigin.Dot(planeNormal))) / (rayDirection.Dot(planeNormal));
		if (t > 0)
		{
			outIntersectionPoint = rayOrigin + (rayDirection * t);
			return true;
		}
		else
		{
			return false;
		}
	}
}

template<typename T>
bool CommonUtilities::IntersectionSphereAABB(const Sphere<T>& aSphere, const AABB3D<T>& aAABB3D, Vector3<T>& outIntersectionPoint, T& aPenetrationDepthOut)
{
	Vector3<T> circleCenterPoint = aSphere.GetPosition();
	Vector3<T> intersectionPoint = circleCenterPoint;
	Vector3<T> AABBMin = aAABB3D.GetMin();
	Vector3<T> AABBMax = aAABB3D.GetMax();

	if (circleCenterPoint.x < AABBMin.x)
	{
		intersectionPoint.x = AABBMin.x;
	}
	else if (circleCenterPoint.x > AABBMax.x)
	{
		intersectionPoint.x = AABBMax.x;
	}

	if (circleCenterPoint.y < AABBMin.y)
	{
		intersectionPoint.y = AABBMin.y;
	}
	else if (circleCenterPoint.y > AABBMax.y)
	{
		intersectionPoint.y = AABBMax.y;
	}

	if (circleCenterPoint.z < AABBMin.z)
	{
		intersectionPoint.z = AABBMin.z;
	}
	else if (circleCenterPoint.z > AABBMax.z)
	{
		intersectionPoint.z = AABBMax.z;
	}

	T sphereRadius = aSphere.GetRadius();
	if ((intersectionPoint - circleCenterPoint).LengthSqr() <= sphereRadius * sphereRadius)
	{
		aPenetrationDepthOut = aSphere.GetRadius() - (intersectionPoint - circleCenterPoint).Length();
		outIntersectionPoint = intersectionPoint;
		return true;
	}
	return false;
}

template<typename T>
bool CommonUtilities::IntersectionAABBRay(const AABB3D<T>& aAABB, const Ray<T>& aRay, Vector3<T>& aOutIntersectionPoint)
{
	bool result = false;
	Vector3<T> max = aAABB.GetMax();
	Vector3<T> min = aAABB.GetMin();
	Vector3<T> rayOrigin = aRay.GetOrigin();

	Vector3<T> closestCollisionPoint{ INFINITY, INFINITY, INFINITY };
	T closestDistance = INFINITY;

	static std::array<Plane<T>, 6> planes;
	planes[0] = Plane<T>(max, Vector3<T>(1, 0, 0));
	planes[1] = Plane<T>(max, Vector3<T>(0, 1, 0));
	planes[2] = Plane<T>(min, Vector3<T>(0, 0, 1));
	planes[3] = Plane<T>(min, Vector3<T>(-1, 0, 0));
	planes[4] = Plane<T>(min, Vector3<T>(0, -1, 0));
	planes[5] = Plane<T>(max, Vector3<T>(0, 0, -1));

	for (int i = 0; i < 6; i++)
	{
		if (IntersectionPlaneRay(planes[i], aRay, aOutIntersectionPoint))
		{
			if (aAABB.IsInsideMeshCell(aOutIntersectionPoint + aRay.GetDirection() * 0.0001f))
			{
				T newDistance = (aOutIntersectionPoint - rayOrigin).LengthSqr();
				if (newDistance < closestDistance)
				{
					closestCollisionPoint = aOutIntersectionPoint;
					closestDistance = newDistance;
					result = true;
				}
			}
		}
	}
	if (result)
	{
		aOutIntersectionPoint = closestCollisionPoint;
		return true;
	}
	else
	{
		aOutIntersectionPoint = Vector3<T>();
		return false;
	}
}

template<class T>
bool CommonUtilities::IntersectionAABBRay(const AABB3D<T>& aAABB, const Ray<T>& aRay)
{
	if (aAABB.IsInsideMeshCell(aRay.GetOrigin()))
	{
		return true;
	}
	Vector3<T> min = aAABB.GetMin();
	Vector3<T> max = aAABB.GetMax();
	Vector3<T> origin = aRay.GetOrigin();
	Vector3<T> direction = aRay.GetDirection();
	T tXMin = (min.x - origin.x) / direction.x;
	T tXMax = (max.x - origin.x) / direction.x;

	// Swap tMin and tMax if necessary to ensure tMin <= tMax
	if (tXMin > tXMax)
		std::swap(tXMin, tXMax);

	// Find the maximum intersection interval along the ray
	T tYMin = (min.y - origin.y) / direction.y;
	T tYMax = (max.y - origin.y) / direction.y;

	// Swap tYMin and tYMax if necessary to ensure tYMin <= tYMax
	if (tYMin > tYMax)
		std::swap(tYMin, tYMax);

	// Update tMin and tMax to represent the intersection intervals along the ray in the Y direction
	if ((tXMin > tYMax) || (tYMin > tXMax))
		return false; // No intersection along Y direction

	if (tYMin > tXMin)
		tXMin = tYMin;
	if (tYMax < tXMax)
		tXMax = tYMax;

	// Find the maximum intersection interval along the ray in the Z direction
	T tZMin = (min.z - origin.z) / direction.z;
	T tZMax = (max.z - origin.z) / direction.z;

	// Swap tZMin and tZMax if necessary to ensure tZMin <= tZMax
	if (tZMin > tZMax)
		std::swap(tZMin, tZMax);

	// Update tMin and tMax to represent the intersection intervals along the ray in the Z direction
	if ((tXMin > tZMax) || (tZMin > tXMax))
		return false; // No intersection along Z direction

	if (tZMin > tXMin)
		tXMin = tZMin;
	if (tZMax < tXMax)
		tXMax = tZMax;

	// Check if the intersection interval along the ray is valid (tMin <= tMax)
	return (tXMin <= tXMax);
}

template<typename T>
bool CommonUtilities::IntersectionSphereRay(const Sphere<T>& aSphere, const Ray<T>& aRay, Vector3<T>& outIntersectionPoint)
{
	Vector3<T> rayOrigin = aRay.GetOrigin();
	Vector3<T> rayDirection = aRay.GetDirection();
	Vector3<T> spherePosition = aSphere.GetPosition();
	T sphereRadius = aSphere.GetRadius();

	Vector3<T> rayOriginToSpherePosition = spherePosition - rayOrigin;
	if (rayOriginToSpherePosition.Dot(rayDirection) < 0)
	{
		return false;
	}
	Vector3<T> Pmal = rayOrigin + rayDirection * rayOriginToSpherePosition.Dot(rayDirection);
	T DistanceToCenter = (spherePosition - Pmal).LengthSqr();
	if (DistanceToCenter <= sphereRadius * sphereRadius)
	{
		outIntersectionPoint = (rayOrigin - spherePosition).GetNormalized() * sphereRadius;
		return true;
	}
	return false;
}

template<typename T>
bool CommonUtilities::IntersectionSphereSphere(const Sphere<T>& aSphere0, const Sphere<T>& aSphere1, Vector3<T>& outIntersectionPoint, T& aPenetrationDepthOut)
{
	T sphere0Radius = aSphere0.GetRadius();
	Vector3<T> origin0ToOrigin1 = aSphere1.GetPosition() - aSphere0.GetPosition();
	if (origin0ToOrigin1.LengthSqr() > static_cast<T>(pow(static_cast<double>(sphere0Radius + aSphere1.GetRadius()), 2)))
	{
		outIntersectionPoint = Vector3<T>();
		return false;
	}
	aPenetrationDepthOut = (sphere0Radius + aSphere1.GetRadius() - origin0ToOrigin1.Length()) * static_cast<T>(2);
	outIntersectionPoint = aSphere0.GetPosition() + (origin0ToOrigin1.GetNormalized() * (sphere0Radius - (aPenetrationDepthOut / static_cast<T>(2))));
	return true;
}

template<typename T>
bool CommonUtilities::IntersectionAABB3DAABB3D(const AABB3D<T>& aAABB3D1, const AABB3D<T>& aAABB3D2)
{
	std::array<Vector3<T>, 8> verticies1 = aAABB3D1.GetVerticies();
	std::array<Vector3<T>, 8> verticies2 = aAABB3D2.GetVerticies();

	for (int i = 0; i < 8; i++)
	{
		if (aAABB3D2.IsInsideMeshCell(verticies1[i]))
		{
			return true;
		}
		if (aAABB3D1.IsInsideMeshCell(verticies2[i]))
		{
			return true;
		}
	}
	return false;
}

template<typename T>
bool CommonUtilities::IntersectionSphereOBB3D(const Sphere<T>& aSphere, const OBB3D<T>& aOBB3D)
{
	Vector3<T> nearestPoint = aOBB3D.NearestPoint(aSphere.GetPosition());
	T distance = (nearestPoint - aSphere.GetPosition()).Length();
	return distance <= aSphere.GetRadius();
}

template<typename T>
bool CommonUtilities::IntersectionAABB3DOBB3D(const AABB3D<T>& aAABB3D, const OBB3D<T>& aOBB3D)
{
	Matrix4x4<T> rotationMatrix = aOBB3D.GetRotationMatrix();
	std::vector<Vector3<T>> axis =
	{
		Vector3<T>(1, 0, 0),
		Vector3<T>(0, 1, 0),
		Vector3<T>(0, 1, 0),

		rotationMatrix.GetRow(1).ToVector3(),
		rotationMatrix.GetRow(2).ToVector3(),
		rotationMatrix.GetRow(3).ToVector3()
	};
	for (int i = 0; i < 3; i++)
	{
		for (int j = 3; j < 6; j++)
		{
			axis.push_back(axis[i].Cross(axis[j]));
		}
	}

	for (int i = 0; i < 15; i++)
	{
		if (!CommonUtilities::AxisOverlap(aOBB3D.GetInterval(axis[i]), aAABB3D.GetInterval(axis[i])))
		{
			return false;
		}
	}
	return true;
}

template<typename T>
bool CommonUtilities::IntersectionOBB3DOBB3D(const OBB3D<T>& aOBB3D1, const OBB3D<T>& aOBB3D2)
{
	Matrix4x4<T> rotationMatrix1 = aOBB3D1.GetRotationMatrix();
	Matrix4x4<T> rotationMatrix2 = aOBB3D2.GetRotationMatrix();
	std::vector<Vector3<T>> axis =
	{
		rotationMatrix1.GetRow(1).ToVector3(),
		rotationMatrix1.GetRow(2).ToVector3(),
		rotationMatrix1.GetRow(3).ToVector3(),

		rotationMatrix2.GetRow(1).ToVector3(),
		rotationMatrix2.GetRow(2).ToVector3(),
		rotationMatrix2.GetRow(3).ToVector3()
	};
	for (int i = 0; i < 3; i++)
	{
		for (int j = 3; j < 6; j++)
		{
			axis.push_back(axis[i].Cross(axis[j]));
		}
	}

	for (int i = 0; i < 15; i++)
	{
		if (!CommonUtilities::AxisOverlap(aOBB3D1.GetInterval(axis[i]), aOBB3D2.GetInterval(axis[i])))
		{
			return false;
		}
	}
	return true;
}

template<typename T>
bool CommonUtilities::IntersectionOBB3DRay(const OBB3D<T>& aOBB3D, const Ray<T>& aRay, Vector3<T>& aOutIntersectionPoint)
{
	Matrix4x4<T> rotationInverse = aOBB3D.GetRotationMatrix().GetFastInverse();
	Matrix4x4<T> transform = aOBB3D.GetRotationMatrix() * Matrix4x4<T>::CreateTranslation(aOBB3D.GetPosition());

	Ray<T> rotatedRay(aRay.GetOrigin() * transform.GetFastInverse(), aRay.GetDirection() * rotationInverse);

	bool result = CommonUtilities::IntersectionAABBRay(aOBB3D.OBB3DToAABB3DLocalSpace(), rotatedRay, aOutIntersectionPoint);
	if (result)
	{
		aOutIntersectionPoint = aOutIntersectionPoint * transform;
		return true;
	}
	else
	{
		aOutIntersectionPoint = Vector3<T>();
		return false;
	}
}
template <typename T>
CommonUtilities::Vector3<T> ClosestPointOnLineSegment(const CommonUtilities::Vector3<T>& A, const CommonUtilities::Vector3<T>& B, const CommonUtilities::Vector3<T>& Point)
{
	CommonUtilities::Vector3<T> AB = B - A;
	float t = (Point - A).Dot(AB) / AB.LengthSqr();
	return A + min(max(t, 0.0f), 1.0f) * AB; // saturate(t) can be written as: min((max(t, 0), 1)
}
template <typename T>
bool CommonUtilities::IntersectionCapsuleCapsule(const Capsule<T>& aCapsule1, const Capsule<T>& aCapsule2, T& aDepth, Vector3<T>& aCollisionNormal)
{
	Vector3<T> firstCapsuleNormal = (aCapsule1.GetTip() - aCapsule1.GetBase()).GetNormalized();
	Vector3<T> firstLineEndOffset = firstCapsuleNormal * aCapsule1.GetRadius();
	Vector3<T> firstCapsulePoint1 = aCapsule1.GetBase() + firstLineEndOffset;
	Vector3<T> firstCapsulePoint2 = aCapsule1.GetTip() - firstLineEndOffset;

	Vector3<T> secondCapsuleNormal = (aCapsule2.GetTip() - aCapsule2.GetBase()).GetNormalized();
	Vector3<T> secondLineEndOffset = secondCapsuleNormal * aCapsule2.GetRadius();
	Vector3<T> secondCapsulePoint1 = aCapsule2.GetBase() + secondLineEndOffset;
	Vector3<T> secondCapsulePoint2 = aCapsule2.GetTip() - secondLineEndOffset;

	Vector3<T> endPointVec1 = secondCapsulePoint1 - firstCapsulePoint1;
	Vector3<T> endPointVec2 = secondCapsulePoint2 - firstCapsulePoint1;
	Vector3<T> endPointVec3 = secondCapsulePoint1 - firstCapsulePoint2;
	Vector3<T> endPointVec4 = secondCapsulePoint2 - firstCapsulePoint2;

	float dist1 = endPointVec1.LengthSqr();
	float dist2 = endPointVec2.LengthSqr();
	float dist3 = endPointVec3.LengthSqr();
	float dist4 = endPointVec4.LengthSqr();

	Vector3<T> bestFirstCapsuleVector;
	Vector3<T> bestSecondCapsuleVector;
	if (dist3 < dist1 || dist3 < dist2 || dist4 < dist1 || dist4 < dist2)
	{
		bestFirstCapsuleVector = firstCapsulePoint2;
	}
	else
	{
		bestFirstCapsuleVector = firstCapsulePoint1;
	}

	bestSecondCapsuleVector = ClosestPointOnLineSegment(secondCapsulePoint1, secondCapsulePoint2, bestFirstCapsuleVector);
	bestFirstCapsuleVector = ClosestPointOnLineSegment(firstCapsulePoint1, firstCapsulePoint1, bestSecondCapsuleVector);

	Vector3<T> penetrationDirection = bestFirstCapsuleVector - bestSecondCapsuleVector;
	float length = penetrationDirection.Length();
	penetrationDirection.Normalize();
	aCollisionNormal = penetrationDirection;

	float penetratingDepth = aCapsule1.GetRadius() + aCapsule2.GetRadius() - length;
	aDepth = penetratingDepth;
	return penetratingDepth >= 0;
}

template <typename T>
bool CommonUtilities::IntersectionCapsuleSphere(const Capsule<T>& aCapsule, const Sphere<T>& aSphere, T& aDepth, Vector3<T>& aCollisionNormal)
{
	Vector3<T> firstCapsuleNormal = (aCapsule.GetTip() - aCapsule.GetBase()).GetNormalized();
	Vector3<T> firstLineEndOffset = firstCapsuleNormal * aCapsule.GetRadius();
	Vector3<T> firstCapsulePoint1 = aCapsule.GetBase() + firstLineEndOffset;
	Vector3<T> firstCapsulePoint2 = aCapsule.GetTip() - firstLineEndOffset;

	Vector3<T> closestPoint = ClosestPointOnLineSegment(firstCapsulePoint1, firstCapsulePoint2, aSphere.GetPosition());

	Vector3<T> penetrationDirection = closestPoint - aSphere.GetPosition();
	float length = penetrationDirection.Length();
	penetrationDirection.Normalize();
	aCollisionNormal = penetrationDirection;

	float penetratingDepth = aCapsule.GetRadius() + aSphere.GetRadius() - length;
	aDepth = penetratingDepth;
	return penetratingDepth > 0;
}

template <typename T>
bool CommonUtilities::IntersectionCapsuleOBB3D(const Capsule<T>& aCapsule, const OBB3D<T>& aOBB3D)
{
	Vector3<T> firstCapsuleNormal = (aCapsule.GetTip() - aCapsule.GetBase()).GetNormalized();
	Vector3<T> firstLineEndOffset = firstCapsuleNormal * aCapsule.GetRadius();
	Vector3<T> firstCapsulePoint1 = aCapsule.GetBase() + firstLineEndOffset;
	Vector3<T> firstCapsulePoint2 = aCapsule.GetTip() - firstLineEndOffset;

	Vector3<T> closestPoint1 = ClosestPointOnLineSegment(firstCapsulePoint1, firstCapsulePoint2, aOBB3D.NearestPoint(firstCapsulePoint1));
	Vector3<T> closestPoint2 = ClosestPointOnLineSegment(firstCapsulePoint1, firstCapsulePoint2, aOBB3D.NearestPoint(firstCapsulePoint2));
	Vector3<T> closestPoint3 = ClosestPointOnLineSegment(firstCapsulePoint1, firstCapsulePoint2, aOBB3D.NearestPoint(aCapsule.GetPosition()));

	float dist1 = closestPoint1.LengthSqr();
	float dist2 = closestPoint2.LengthSqr();
	float dist3 = closestPoint3.LengthSqr();

	Vector3<T> closestPoint = closestPoint1;
	float minDist = dist1;

	if (dist2 < minDist) {
		closestPoint = closestPoint2;
		minDist = dist2;
	}

	if (dist3 < minDist) {
		closestPoint = closestPoint3;
		minDist = dist3;
	}

	return IntersectionSphereOBB3D(Sphere<T>(closestPoint, aCapsule.GetRadius()), aOBB3D);
}

template <typename T>
bool CommonUtilities::IntersectionCapsuleAABB(const Capsule<T>& aCapsule, const AABB3D<T>& aAABB)
{
	Vector3<T> firstCapsuleNormal = (aCapsule.GetTip() - aCapsule.GetBase()).GetNormalized();
	Vector3<T> firstLineEndOffset = firstCapsuleNormal * aCapsule.GetRadius();
	Vector3<T> firstCapsulePoint1 = aCapsule.GetBase() + firstLineEndOffset;
	Vector3<T> firstCapsulePoint2 = aCapsule.GetTip() - firstLineEndOffset;

	Vector3<T> closestPoint1 = ClosestPointOnLineSegment(firstCapsulePoint1, firstCapsulePoint2, aAABB.GetClosestPoint(firstCapsulePoint1));
	Vector3<T> closestPoint2 = ClosestPointOnLineSegment(firstCapsulePoint1, firstCapsulePoint2, aAABB.GetClosestPoint(firstCapsulePoint2));
	Vector3<T> closestPoint3 = ClosestPointOnLineSegment(firstCapsulePoint1, firstCapsulePoint2, aAABB.GetClosestPoint(aCapsule.GetPosition()));

	float dist1 = closestPoint1.LengthSqr();
	float dist2 = closestPoint2.LengthSqr();
	float dist3 = closestPoint3.LengthSqr();

	Vector3<T> closestPoint = closestPoint1;
	float minDist = dist1;

	if (dist2 < minDist) {
		closestPoint = closestPoint2;
		minDist = dist2;
	}

	if (dist3 < minDist) {
		closestPoint = closestPoint3;
		minDist = dist3;
	}

	return IntersectionSphereAABB(Sphere<T>(closestPoint, aCapsule.GetRadius()), aAABB);

}

template <typename T>
bool CommonUtilities::IntersectionRayTriangle(const Ray<T>& aRay, const Triangle<T>& aTriangle, Vector3<T>& aOutIntersectionPoint)
{
	auto& points = aTriangle.GetPoints();
	const Vector3<T> firstEdge = points[1] - points[0];
	const Vector3<T> secondEdge = points[2] - points[0];
	Vector3<T> h = aRay.GetDirection().Cross(secondEdge);
	T a = firstEdge.Dot(h);

	if (a > -std::numeric_limits<T>::epsilon() && a < std::numeric_limits<T>::epsilon()) {
		return false; // The ray is parallel to the triangle.
	}

	T f = static_cast<T>(1.0) / a;
	Vector3<T> s = aRay.GetOrigin() - points[0];
	T u = f * s.Dot(h);

	if (u < 0.0 || u > 1.0) {
		return false; // The intersection is outside the triangle.
	}

	Vector3<T> q = s.Cross(firstEdge);
	T v = f * aRay.GetDirection().Dot(q);

	if (v < 0.0 || u + v > 1.0) {
		return false; // The intersection is outside the triangle.
	}

	// At this stage, we can compute t to find out where the intersection point is on the line.
	T t = f * secondEdge.Dot(q);
	aOutIntersectionPoint = aRay.GetOrigin() + aRay.GetDirection() * (t);

	if (t > std::numeric_limits<T>::epsilon()) {
		return true; // There is an intersection
	}
	else {
		return false; // Intersection is behind the ray
	}
}

// being tested
template<typename T>
bool CommonUtilities::IntersectionLineLine(const Vector2<T>& aFirstLineFirstPoint, const Vector2<T>& aFirstLineSecondPoint, const Vector2<T>& aSecondLineFirstPoint, const Vector2<T>& aSecondLineSecondPoint, Vector2<T>& aOutIntersectionPoint)
{
	T firstLineYDiff = aFirstLineSecondPoint.y - aFirstLineFirstPoint.y;
	T firstLineXDiff = aFirstLineFirstPoint.x - aFirstLineSecondPoint.x;

	T secondLineYDiff = aSecondLineSecondPoint.y - aSecondLineFirstPoint.y;
	T secondLineXDiff = aSecondLineFirstPoint.x - aSecondLineSecondPoint.x;

	T determinant = firstLineYDiff * secondLineXDiff - secondLineYDiff * firstLineXDiff;

	if (determinant == 0)
	{
		return false;
	}
	else
	{
		T c1 = firstLineYDiff * aFirstLineFirstPoint.x + firstLineXDiff * aFirstLineFirstPoint.y;
		T c2 = secondLineYDiff * aSecondLineFirstPoint.x + secondLineXDiff * aSecondLineFirstPoint.y;
		T xCollision = (secondLineXDiff * c1 - firstLineXDiff * c2) / determinant;
		T yCollision = (firstLineYDiff * c2 - secondLineYDiff * c1) / determinant;
		aOutIntersectionPoint.x = xCollision;
		aOutIntersectionPoint.y = yCollision;
		return true;
	}
}

//TODO CREATE MESH COLLIDERS
