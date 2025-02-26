#pragma once
#include "GraphicsCommandList.h"
#include "CommonUtilities/Matrix4x4.hpp"

class Material;
class Mesh;

class GfxCmd_RenderSkybox : public GraphicsCommandBase
{
public:
	GfxCmd_RenderSkybox(std::shared_ptr<Mesh> aMesh, CU::Matrix4x4f aTransform);
	~GfxCmd_RenderSkybox() override;


	void Execute() override;
	void Destroy() override;

private:
	std::shared_ptr<Mesh> myMesh;
	CU::Matrix4x4f myTransform;
};

