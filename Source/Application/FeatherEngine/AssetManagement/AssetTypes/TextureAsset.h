#pragma once
#include "Asset.h"
#include "GraphicsEngine/Texture.h"

struct TextureAsset : public Asset
{
	~TextureAsset() override = default;
	std::shared_ptr<Texture> myTexture;
};
