#pragma once
#include "Collider.h"
#include "CommonUtilities/Capsule.hpp"

class CapsuleCollider : public Collider
{
public:
	CapsuleCollider();
	~CapsuleCollider() override;
	Vec3 FindFurthestPoint(Vec3 aDirection) const override;
	void SetPosition(Vec3 aPosition) override;
	CommonUtilities::Capsule<float>& GetCollider();
private:
	CommonUtilities::Capsule<float> myCollider;
};