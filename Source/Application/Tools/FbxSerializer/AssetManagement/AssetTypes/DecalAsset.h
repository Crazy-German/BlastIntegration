#pragma once
#include "Asset.h"
#include "GraphicsEngine/Texture.h"

struct DecalAsset : Asset
{
	std::shared_ptr<Texture> myTexture;
	std::filesystem::path myPath;
	~DecalAsset() override = default;
};
