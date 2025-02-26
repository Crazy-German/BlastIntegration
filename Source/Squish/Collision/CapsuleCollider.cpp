#include "CapsuleCollider.h"

CapsuleCollider::CapsuleCollider()
{
	myType = ColliderType::Capsule;
}

CapsuleCollider::~CapsuleCollider()
= default;

Vec3 CapsuleCollider::FindFurthestPoint(Vec3 aDirection) const
{
	aDirection.Normalize();
	Vec3 dir = (myCollider.GetTip() - myCollider.GetBase()).GetNormalized();
	Vec3 point = myCollider.GetPosition();
	point += aDirection * myCollider.GetRadius();
	point += aDirection.Dot(dir)*myCollider.GetHeight()*dir;
	return point;
}

void CapsuleCollider::SetPosition(Vec3 aPosition)
{
	myCollider.SetPosition(aPosition);
}

CommonUtilities::Capsule<float>& CapsuleCollider::GetCollider()
{
	return myCollider;
}
