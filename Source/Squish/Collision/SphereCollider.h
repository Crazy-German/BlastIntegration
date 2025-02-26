#pragma once
#include "Collider.h"
#include "CommonUtilities/Sphere.hpp"

using Vec3 = CommonUtilities::Vector3<float>;

class SphereCollider : public Collider
{
public:
	SphereCollider();
	~SphereCollider();
	Vec3 FindFurthestPoint(Vec3 aDirection) const override;
	void SetPosition(Vec3 aPosition) override;
	CommonUtilities::Sphere<float>& GetCollider();
private:
	CommonUtilities::Sphere<float> myCollider;
};
