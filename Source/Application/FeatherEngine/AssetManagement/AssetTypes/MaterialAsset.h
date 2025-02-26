#pragma once
#include "Asset.h"
#include "../Graphics/GraphicsEngine/Material.h"

struct MaterialAsset : Asset
{
	~MaterialAsset() override;
	std::shared_ptr<Material> myMaterial;
};

inline MaterialAsset::~MaterialAsset()
{
	myMaterial = nullptr;
}
