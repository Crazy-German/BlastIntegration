#include "HullCollider.h"

#include <algorithm>
#include <set>

HullCollider::HullCollider()
{
	myType = ColliderType::Hull;
}

HullCollider::~HullCollider() = default;
//Support functions for hull generation
bool compare(Vec3 p1, Vec3 p2) {
	return (p1.x < p2.x) || (p1.x == p2.x && p1.y < p2.y) || (p1.x == p2.x && p1.y == p2.y && p1.z < p2.z);
}
//Check if turn is convex name should probably be changed
bool LeftTurn(const Vec3& aFirstPoint, const Vec3 aSecondPoint, const Vec3 aThirdPoint)
{
	Vec3 FirstSecond = aSecondPoint - aFirstPoint;
	Vec3 FirstThird = aThirdPoint - aFirstPoint;
	Vec3 cross = FirstSecond.Cross(FirstThird);
	return cross.z > 0;
}

//Graham scan algorithm
std::vector<Vec3> grahamScan(std::vector<Vec3>& aPoints)
{
	std::sort(aPoints.begin(), aPoints.end(), compare);
	std::vector<Vec3> hull;
	std::set<Vec3> uniquePoints;
	for(const auto& point : aPoints)
	{
		while(hull.size() >= 2 && !LeftTurn(hull[hull.size() - 2], hull.back(), point)) {
			hull.pop_back();
		}
		if (!uniquePoints.contains(point))
		{
			hull.push_back(point);
			uniquePoints.insert(point);
		}
	}
	size_t t = hull.size() + 1;
	for (auto it = aPoints.rbegin(); it != aPoints.rend(); ++it) {
		while (hull.size() >= t && !LeftTurn(hull[hull.size() - 2], hull.back(), *it)) {
			hull.pop_back();
		}
		if (!uniquePoints.contains(*it))
		{
			hull.push_back(*it);
			uniquePoints.insert(*it);
		}
	}
	if (hull.size() > 1 && hull.front() == hull.back()) {
		hull.pop_back();
	}
	
	return  hull;
}
//Generates the verticies for a hull collider with the specified number of subhulls and max verticies per hull
std::vector<Vec3> HullCollider::GenereateCollider(const std::vector<Vec3>& aVerticies, int aMaxNrOfVerticies, int aMaxNrOfHulls)
{
	std::vector<Vec3> returnVal;
	int n = static_cast<int>(aVerticies.size());
	if (n <= 3)
	{
		return  aVerticies;
	}
	std::vector<std::vector<Vec3>> subHulls;
	int m = std::min(aMaxNrOfVerticies, static_cast<int>(std::ceil(static_cast<float>(n) / aMaxNrOfHulls)));
	for(int i = 0; i<n; i+=m)
	{
		std::vector<Vec3> subSet(aVerticies.begin() + i, aVerticies.begin() + std::min((i +  m), n));
		subHulls.push_back(grahamScan(subSet));
	}
	while (subHulls.size() > aMaxNrOfHulls)
	{
		std::vector<Vec3> combinedHull = subHulls.back();
		subHulls.pop_back();
		for (const auto& point : subHulls.back())
		{
			combinedHull.push_back(point);
		}
		subHulls.pop_back();
		subHulls.push_back(grahamScan(combinedHull));
	}
	for(const auto& hull : subHulls)
	{
		returnVal.insert(returnVal.begin(), hull.begin(), hull.end());
	}
	return returnVal;
}

CommonUtilities::Vector3<float> HullCollider::FindFurthestPoint(CommonUtilities::Vector3<float> aDirection) const
{
	Vec3 maxPoint;
	float maxDist = -FLT_MAX;
	float distance;
	for(const Vec3& vertex : myVerticies)
	{
		distance = vertex.Dot(aDirection);
		if(distance > maxDist)
		{
			maxDist = distance;
			maxPoint = vertex;
		}
	}
	return maxPoint;
}

void HullCollider::SetPosition(Vec3 aPosition)
{
	myTransform.SetTranslation(aPosition);
}
