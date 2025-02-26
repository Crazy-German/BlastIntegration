#include "GraphicsEngine.pch.h"
#include "GfxCmd_RenderSkybox.h"

#include "Material.h"
#include "ObjectBuffer.h"

GfxCmd_RenderSkybox::GfxCmd_RenderSkybox(std::shared_ptr<Mesh> aMesh, CU::Matrix4x4f aTransform)
{
	myMesh = aMesh;
	myTransform = aTransform;
}

GfxCmd_RenderSkybox::~GfxCmd_RenderSkybox()
{

}

void GfxCmd_RenderSkybox::Execute()
{
	ObjectBuffer objectBufferData;
	objectBufferData.World = myTransform; //investigate
	objectBufferData.WorldInvT = myTransform.GetInverse().GetTranspose();

	objectBufferData.HasSkinning = !myMesh->GetSkeleton().Joints.empty();
	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objectBufferData);
	GraphicsEngine::Get().SetPixelShader("Skybox_PS");
	GraphicsEngine::Get().RenderSkybox(*myMesh);
}

void GfxCmd_RenderSkybox::Destroy()
{
	myMesh = nullptr;
}
