#pragma once
#include "Asset.h"
#include "GraphicsEngine/Texture.h"

struct SpriteAsset : public Asset
{
	//Add specific variables

	std::shared_ptr<Texture> myTexture;
	~SpriteAsset() override = default;
};