#pragma once
#include "Collider.h"
#include "CommonUtilities/OBB3D.hpp"

class OBBCollider : public Collider
{
public:
	OBBCollider();
	~OBBCollider();
	void Init(const Vec3& aPosition, const Vec3& aSize, const CommonUtilities::Matrix4x4<float>& aRotationMatrix);
	Vec3 FindFurthestPoint(Vec3 aDirection) const override;
	void SetPosition(Vec3 aPosition) override;
	CommonUtilities::OBB3D<float>& GetCollider();
private:
	CommonUtilities::OBB3D<float> myCollider;
};
