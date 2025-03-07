#pragma once
#include <random>
#include "NvBlastExtAuthoringFractureTool.h"

class PhysxRNG : public Nv::Blast::RandomGeneratorBase
{
public:
	PhysxRNG(float aMin = 0.0f, float aMax =1.0f)
	{
		myGen = std::mt19937(myRD());
		myDist = std::uniform_real_distribution<float>(aMin, aMax);
	}
	~PhysxRNG() override = default;
	float getRandomValue() override
	{
		return myDist(myGen);
	}
	void seed(int32_t seed) override
	{
		myGen = std::mt19937(seed);
	}

private:
	std::random_device myRD;
	std::mt19937 myGen;
	std::uniform_real_distribution<float> myDist;
};