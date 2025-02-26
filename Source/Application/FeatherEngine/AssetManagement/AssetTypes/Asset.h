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
	ColliderAsset,
	Script
};

struct Asset
{
	std::filesystem::path myName;
	AssetType myType;
	virtual ~Asset() = default;
	/*template <typename T>
	T* as(){return dynamic_cast<T*>(this);}*/
};
