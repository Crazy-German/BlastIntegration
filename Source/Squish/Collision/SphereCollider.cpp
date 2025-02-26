#include "SphereCollider.h"

SphereCollider::SphereCollider()
{
	myType = ColliderType::Sphere;
}

SphereCollider::~SphereCollider()
{
}

CommonUtilities::Vector3<float> SphereCollider::FindFurthestPoint(CommonUtilities::Vector3<float> aDirection) const
{
	return myCollider.GetPosition()+ myCollider.GetRadius()*aDirection;
}

void SphereCollider::SetPosition(Vec3 aPosition)
{
	myCollider.SetPosition(aPosition);
}

CommonUtilities::Sphere<float>& SphereCollider::GetCollider()
{
	return myCollider;
}
