#pragma once
#include "CommonUtilities/Matrix.hpp"
#include "CommonUtilities/Vector.hpp"

struct FrameBuffer
{
	CU::Matrix4x4f InvView;
	CU::Matrix4x4f Projection;

	CU::Vector3f CameraPos;
	int DebugPass = 0;

	CU::Vector2f Resolution;
	float TotalTime = 0;
	float DeltaTime = 0;

	float MistModifier = 0;
	CU::Vector3f padding;
};