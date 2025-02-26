#pragma once
#include "GraphicsCommandList.h"
#include "CommonUtilities/Matrix4x4.hpp"

class Material;
class Mesh;

class GfxCmd_RenderMesh : public GraphicsCommandBase
{
public:
	GfxCmd_RenderMesh(std::shared_ptr<Mesh> aMesh, const CU::Matrix4x4f& aTransform, 
		const std::vector<std::shared_ptr<Material>>* aMaterialList, bool aIsEnemy = false, bool aIsElite = false, float aHitValue = 0.0f);
	~GfxCmd_RenderMesh() override;

	void Execute() override;
	void Destroy() override;

private:
	std::shared_ptr<Mesh> myMesh;
	CU::Matrix4x4f myTransform;
	const std::vector<std::shared_ptr<Material>>* myMaterialList;
	bool myIsEnemy;
	bool myIsElite;
	float myHitValue;
};

