#pragma once
#include "CommonUtilities/Matrix4x4.hpp"

struct AnimationBuffer
{
	CU::Matrix4x4f JointTransforms[128];
};
