#pragma once
#include "CommonUtilities/Vector.hpp"
#include "RHIStructs.h"

struct ParticleVertex
{
	CU::Vector4<float> position = CU::Vector4<float>(0, 0, 0, 1);
	CU::Vector4<float> color = CU::Vector4<float>(1, 1, 1, 1);
	CU::Vector3<float> velocity = CU::Vector3<float>(0, 0, 0);
	float lifetime = 0.0f;
	float angle = 0.0f;
	CU::Vector2<float> size = CU::Vector2<float>(1, 1);
	CU::Vector4<float> channelmask = CU::Vector4<float>(0, 0, 0, 0);
	unsigned mirrorX = false;
	unsigned mirrorY = false;

	ParticleVertex()
	{

	}
	ParticleVertex(float aX, float aY, float aZ)
	{
		position.x = aX; position.y = aY; position.z = aZ; position.w = 1;
	}

	ParticleVertex(CU::Vector4f aPosition, CU::Vector4f aColor, CU::Vector3f aVelocity, float aLifetime, float aAngle, CU::Vector2<float> aSize, CU::Vector4f aChannelMask)
	{
		position = aPosition;
		color = aColor;
		velocity = aVelocity;
		lifetime = aLifetime;
		angle = aAngle;
		size = aSize;
		channelmask = aChannelMask;
	}

	static const std::vector<VertexElementDesc> InputLayoutDefinition;

};

