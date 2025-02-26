#pragma once
#include "CommonUtilities/Vector3.hpp"
#include <vector>

struct RayCastHit
{
	CommonUtilities::Vector3f Point;
	CommonUtilities::Vector3f Normal;

};

struct RayCastBuffer
{
	bool hasBlock; 
	RayCastHit Block; //The closest blocking point if hasblock is true
	std::vector<RayCastHit> Hits;
};