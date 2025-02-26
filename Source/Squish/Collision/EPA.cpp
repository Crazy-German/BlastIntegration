#include "EPA.h"

#include "GJK.h"

CollisionPoints EPA(const Simplex& aSimplex, const Collider& aColliderFirst, const Collider& aColliderSecond)
{
	std::vector<Vec3> polytope(aSimplex.begin(), aSimplex.end());
	std::vector<size_t> faces ={ 0,1,2, 0,3,1, 0,2,3, 1,3,2	};

	auto [normals, minFace] = GetFaceNormals(polytope, faces);

	Vec3 minNormal;
	float minDist = FLT_MAX;
	while (minDist == FLT_MAX)
	{
		minNormal = normals[minFace].ToVector3();
		minDist = normals[minFace].w;

		Vec3 support = Support(aColliderFirst, aColliderSecond, minNormal);
		float dist = minNormal.Dot(support);

		if(abs(dist-minDist) > 0.001)
		{
			minDist = FLT_MAX;
		}
	std::vector<std::pair<size_t, size_t>> uniqueEdges;

		for (size_t i = 0; i < normals.size(); i++) {
			if (SameDirection(normals[i].ToVector3(), support)) {
				size_t f = i * 3;

				AddIfUniqueEdge(uniqueEdges, faces, f, f + 1);
				AddIfUniqueEdge(uniqueEdges, faces, f + 1, f + 2);
				AddIfUniqueEdge(uniqueEdges, faces, f + 2, f);

				faces[f + 2] = faces.back(); faces.pop_back();
				faces[f + 1] = faces.back(); faces.pop_back();
				faces[f] = faces.back(); faces.pop_back();

				normals[i] = normals.back(); // pop-erase
				normals.pop_back();

				i--;
			}
		}
		std::vector<size_t> newFaces;
		for (auto [firstEdgeIndex, SecondEdgeIndex] : uniqueEdges) {
			newFaces.push_back(firstEdgeIndex);
			newFaces.push_back(SecondEdgeIndex);
			newFaces.push_back(polytope.size());
		}

		polytope.push_back(support);

		auto [newNormals, newMinFace] = GetFaceNormals(polytope, newFaces);
		float oldMinDistance = FLT_MAX;
		for (size_t i = 0; i < normals.size(); i++) {
			if (normals[i].w < oldMinDistance) {
				oldMinDistance = normals[i].w;
				minFace = i;
			}
		}

		if (newNormals[newMinFace].w < oldMinDistance) {
			minFace = newMinFace + normals.size();
		}

		faces.insert(faces.end(), newFaces.begin(), newFaces.end());
		normals.insert(normals.end(), newNormals.begin(), newNormals.end());
	}
	CollisionPoints Points;
	Points.myNormal = minNormal;
	Points.myPenetrationDepth = minDist + 0.001f;
	Points.myHasCollision = true;
	return Points;
}

std::pair<std::vector<CommonUtilities::Vector4f>, size_t> GetFaceNormals(
	const std::vector<CommonUtilities::Vector3f>& aPolyTope, const std::vector<size_t>& aFaces)
{
	std::vector<CommonUtilities::Vector4f> normals;
	size_t minTriangle;
	float  minDistance = FLT_MAX;

	for (size_t i = 0; i < aFaces.size(); i += 3) {
		CommonUtilities::Vector3f a = aPolyTope[aFaces[i]];
		CommonUtilities::Vector3f b = aPolyTope[aFaces[i + 1]];
		CommonUtilities::Vector3f c = aPolyTope[aFaces[i + 2]];

		CommonUtilities::Vector3f normal = ((b - a).Cross(c - a)).GetNormalized();
		float distance = normal.Dot(a);

		if (distance < 0) {
			normal *= -1.0f;
			distance *= -1;
		}

		normals.emplace_back(normal.x, normal.y, normal.z, distance);

		if (distance < minDistance) {
			minTriangle = i / 3;
			minDistance = distance;
		}
	}

	return { normals, minTriangle };
}

void AddIfUniqueEdge(std::vector<std::pair<size_t, size_t>>& aEdges, const std::vector<size_t>& aFaces,
	size_t aFirstIndex, size_t aSecondIndex)
{
	auto reverse = std::find(                       //      0--<--3
		aEdges.begin(),                              //     / \ B /   A: 2-0
		aEdges.end(),                                //    / A \ /    B: 0-2
		std::make_pair(aFaces[aSecondIndex], aFaces[aFirstIndex]) //   1-->--2
	);

	if (reverse != aEdges.end()) {
		aEdges.erase(reverse);
	}
	else 
	{
		aEdges.emplace_back(aFaces[aFirstIndex], aFaces[aSecondIndex]);
	}
}
