#include "FeatherEngine.pch.h"
#include "ShadowCameraComponent.h"

#include "GraphicsEngine/FrameBuffer.h"
#include "GraphicsEngine/GfxCmd_SetFrameBuffer.h"
#include <string>

#include "MainSingleton.h"
#include "Transform.h"
#include "AssetManagement/AssetManager.h"
#include "GameObjectSystem\Components\Lights\LightComponent.h"
#include "GraphicsEngine/DebugLineObject.h"
#include "GraphicsEngine/GfxCmd_DrawDebugLines.h"
#include "GraphicsEngine\LightBuffer.h"

#include "Logger\logger.h"
#ifdef _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(RhiLog, "ShadowCamera", Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(RhiLog, "ShadowCamera", Error);
#endif
void ShadowCameraComponent::Awake()
{
	myFrameBuffer = std::make_shared<FrameBuffer>();
}

void ShadowCameraComponent::Render()
{
	auto& renderer = RENDERER;
	renderer.Enqueue<GfxCmd_SetFrameBuffer>(GetFrameBuffer());

}

void ShadowCameraComponent::Init(const CU::Matrix4x4<float>& aViewInverse, const CU::Matrix4x4<float>& aProjection, const CU::Vector3<float>& aPosition)
{
	myFrameBuffer->CameraPos.x = aPosition.x;
	myFrameBuffer->CameraPos.y = aPosition.y;
	myFrameBuffer->CameraPos.z = aPosition.z;
	myFrameBuffer->Projection = aProjection;
	myFrameBuffer->InvView = aViewInverse;
}

void ShadowCameraComponent::CreateOrthographic(const CU::Vector2f aResolution, const float aNearPlane, const float aFarPlane)
{
	myProjection = CU::Matrix4x4f();
	myProjection(1, 1) = 2 / aResolution.x;
	myProjection(2, 2) = 2 / aResolution.y;
	myProjection(3, 3) = 1 / (aFarPlane - aNearPlane);
	myProjection(4, 3) = aNearPlane / (aNearPlane - aFarPlane);
}

void ShadowCameraComponent::CreatePerspective(const float aNearPlane, const float aFarPlane)
{
	myProjection = CU::Matrix4x4f();
	float aspectRatio = 16.f / 9.f;
	myProjection = CU::Matrix4x4f::CreatePerspective(90, aspectRatio, aNearPlane, aFarPlane);
}

void ShadowCameraComponent::CreateShadowCube()
{

}

CommonUtilities::Matrix4x4f ShadowCameraComponent::GetProjection()
{
	return myProjection;
}

std::shared_ptr<FrameBuffer> ShadowCameraComponent::GetFrameBuffer()
{
	myFrameBuffer->InvView = GetGameObject()->GetComponent<LightComponent>()->GetLightData()->LightViewInv;
	myFrameBuffer->Projection = GetGameObject()->GetComponent<LightComponent>()->GetLightData()->LightProj;
	myFrameBuffer->CameraPos = GetGameObject()->GetComponent<LightComponent>()->GetLightData()->LightPos;
	return myFrameBuffer;
}
#pragma optimize("",off)
void ShadowCameraComponent::DebugRender()
{
	GetFrameBuffer();
	std::shared_ptr<DebugLineObject> debuglines = std::make_shared<DebugLineObject>();
 	debuglines->AddLine(myFrameBuffer->InvView.GetFastInverse().GetTranslation(), myFrameBuffer->InvView.GetFastInverse().GetRow(3).ToVector3()*100000.f);
	debuglines->SetName("Shadow cam");
	MainSingleton::Get().GetRenderer().Enqueue<GfxCmd_DrawDebugLines>(debuglines, GetGameObject()->GetTransform()->GetMatrixScale());
}
#pragma optimize("",on)
