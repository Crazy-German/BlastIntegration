#pragma once
#include <memory>
#include "AssetManagement/AssetTypes/Asset.h"
#include "GraphicsEngine/Texture.h"

struct TextureAsset : public Asset
{
	std::shared_ptr<Texture> myTexture;
	~TextureAsset() override = default;
};