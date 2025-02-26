#pragma once
#include "Asset.h"
#include "GraphicsEngine/Animation.h"

struct AnimationAsset : Asset
{
	std::shared_ptr<Animation> myAnimation;
	~AnimationAsset() override = default;
};