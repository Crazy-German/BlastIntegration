#pragma once
#include <intsafe.h>
#include <vector>

#include "Collider.h"
#include "Simplex.h"

struct CollisionPoints
{
	CommonUtilities::Vector3f myNormal;
	float myPenetrationDepth;
	bool myHasCollision;
};

CollisionPoints EPA(const Simplex& aSimplex, const Collider& aColliderFirst, const Collider& aColliderSecond);

std::pair<std::vector<CommonUtilities::Vector4f>, size_t> GetFaceNormals(const std::vector<CommonUtilities::Vector3f>& aPolyTope, const std::vector<size_t>& aFaces);

void AddIfUniqueEdge(std::vector<std::pair<size_t, size_t>>& aEdges, const std::vector<size_t>& aFaces, size_t aFirstIndex, size_t aSecondIndex);