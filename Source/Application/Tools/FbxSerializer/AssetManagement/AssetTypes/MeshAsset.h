#pragma once
#include "Asset.h"
#include "GraphicsEngine/Mesh.h"

struct MeshAsset : public Asset
{
	std::shared_ptr<Mesh> myMesh;
	~MeshAsset() override = default;
};
