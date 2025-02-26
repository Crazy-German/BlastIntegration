#include "GraphicsEngine.pch.h"
#include "GfxCmd_DrawDebugLines.h"
#include "Mesh.h"
#include "DebugLineObject.h"

#include "ObjectBuffer.h"

GfxCmd_DrawDebugLines::GfxCmd_DrawDebugLines(std::shared_ptr<DebugLineObject> someDebugLines, const CU::Matrix4x4f& aTransform)
{
	myDebugName = someDebugLines->GetName();
	myDebugObject = someDebugLines;
	myTransform = aTransform;
}

GfxCmd_DrawDebugLines::GfxCmd_DrawDebugLines(std::shared_ptr<Mesh> aMesh, const CU::Matrix4x4f& aTransform, bool aHasBones, DebugColor aColor, std::string_view aDebugName)
{
	myHasBones = aHasBones;
	myDebugName = aDebugName;
	myDebugObject = std::make_shared<DebugLineObject>();
	myDebugObject->Initialize(aMesh->GetVertices(), aMesh->GetIndices(), aColor, aDebugName, true);
	myTransform = aTransform;
}

GfxCmd_DrawDebugLines::~GfxCmd_DrawDebugLines() = default;

void GfxCmd_DrawDebugLines::Execute()
{
	ObjectBuffer objectBufferData;
	objectBufferData.World = myTransform;
	objectBufferData.WorldInvT = myTransform.GetInverse().GetTranspose();
	objectBufferData.HasSkinning = myHasBones;

	auto& engine = GraphicsEngine::Get();
	engine.UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objectBufferData);
	engine.ChangeRenderTarget(GraphicsEngine::Get().GetIntermediateLDRBuffer());
	engine.SetVertexShader("Default_VS");
	engine.SetPixelShader("DebugLine_PS");
	//engine.ClearDepthStencil();
	engine.RenderDebugLines(*myDebugObject);

}

void GfxCmd_DrawDebugLines::Destroy()
{
	myDebugObject = nullptr;
	myDebugName.~basic_string();
}
