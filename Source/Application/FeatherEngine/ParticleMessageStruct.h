#pragma once
#include <CommonUtilities/Vector.hpp>
struct ParticleMessage
{
	std::string Name;
	float Lifetime;
	CU::Vector3f Position;
};