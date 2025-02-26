#pragma once
#include "PxFiltering.h"

#define COLLIDE_ALL = PX_MAX_U32

struct CollisionLayer : public physx::PxFilterData
{
	CollisionLayer(uint32_t aOwnLayerID, uint32_t aLayersToFilterWith)
	{
		word0 = aOwnLayerID;
		word1 = aLayersToFilterWith;
	}
	CollisionLayer(const PxFilterData& aData)
	{
		word0 = aData.word0;
		word1 = aData.word1;
		word2 = aData.word2;
		word3 = aData.word3;
	}
};
