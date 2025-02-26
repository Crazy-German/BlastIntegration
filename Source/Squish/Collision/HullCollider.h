#pragma once
#include <vector>
#include "Collider.h"
#include "CommonUtilities/Matrix4x4.hpp"
#include "CommonUtilities/Vector3.hpp"

using Vec3 = CommonUtilities::Vector3<float>;

class HullCollider : public Collider
{
public:
	HullCollider();
	~HullCollider() override;

	std::vector<Vec3> GenereateCollider(const std::vector<Vec3>& aVerticies, int aMaxNrOfVerticies, int aMaxNrOfHulls);
	CommonUtilities::Vector3<float> FindFurthestPoint(CommonUtilities::Vector3<float> aDirection) const override;
	void SetPosition(Vec3 aPosition) override;

private:
	std::vector<Vec3> myVerticies;
	CommonUtilities::Matrix4x4f myTransform;
};
