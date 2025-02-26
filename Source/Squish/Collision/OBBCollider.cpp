#include "OBBCollider.h"

OBBCollider::OBBCollider()
{
	myType = ColliderType::OBB;
}

OBBCollider::~OBBCollider()
{
}

void OBBCollider::Init(const Vec3& aPosition, const Vec3& aSize,
	const CommonUtilities::Matrix4x4<float>& aRotationMatrix)
{
	myCollider = CommonUtilities::OBB3D<float>(aPosition, aSize, aRotationMatrix);
}

Vec3 OBBCollider::FindFurthestPoint(Vec3 aDirection) const
{
	Vec3 furthestPoint;
	float maxDist = -FLT_MAX;
	float dist;
	for(const Vec3& vertex : myCollider.GetVerticies())
	{
		dist = vertex.Dot(aDirection);
		if(dist > maxDist)
		{
			maxDist = dist;
			furthestPoint = vertex;
		}
	}
	return furthestPoint;
}

void OBBCollider::SetPosition(Vec3 aPosition)
{
	myCollider.SetPosition(aPosition);
}

CommonUtilities::OBB3D<float>& OBBCollider::GetCollider()
{
	return myCollider;
}
