#pragma once
#include "CommonUtilities/Matrix.hpp"

struct ObjectBuffer
{
	CU::Matrix4x4f World;
	CU::Matrix4x4f WorldInvT;

	unsigned HasSkinning = false;
	unsigned MaterialCount;
	unsigned IsOutlinedA = false;
	unsigned IsOutLinedB = false;

	float IsHit = 0;
	CU::Vector3f Padding;
};