#pragma once
#include <filesystem>

enum class AssetType
{
	Mesh,
	AnimatedMesh,
	Animation,
	Sprite,
	Material,
	Texture,
	Generic,
	Font,
	NavMesh,
};

struct Asset
{
	std::filesystem::path myName;
	AssetType myType;
	virtual ~Asset() = default;
};
