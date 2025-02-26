#include "FeatherEngine.pch.h"
#include "LightComponent.h"
#include "GraphicsEngine\LightBuffer.h"

#include "GameObjectSystem/Components/ShadowCameraComponent.h"
#include "GraphicsEngine/LightBuffer.h"
#include "GraphicsEngine/GfxCmd_SetLightBuffer.h"
#include "GameObjectSystem/Components/Transform.h"
#define _USE_MATH_DEFINES
#include "math.h"

void LightComponent::Awake()
{
}

void LightComponent::Update(const float& aDeltaTime)
{
	aDeltaTime;
}

void LightComponent::Initialize(const CU::Vector3f& aColor, float aIntensity, const CU::Vector3f& aLightDirection,
	const CU::Vector3f& aLightPosition, float aAngleDeg, LightType aLightMode, float anAttenuationRadius)
{
	myLightData = std::make_shared<LightData>();
	myLightData->Active = true;
	myLightData->Color = aColor;
	myLightData->Intensity = aIntensity;
	myLightData->LightDir = aLightDirection;
	myLightData->LightPos = aLightPosition;
	myLightData->OuterConeAngle = aAngleDeg;
	myLightData->LightMode = static_cast<int>(aLightMode);
	myLightType = aLightMode;
	myLightData->Range = anAttenuationRadius;
	myLightData->LightViewInv = GetGameObject()->GetComponent<Transform>()->GetMatrixScale().GetFastInverse();
	if(GetGameObject()->GetComponent<ShadowCameraComponent>())
	{
		myLightData->LightProj = GetGameObject()->GetComponent<ShadowCameraComponent>()->GetProjection();
		
	}
}


void LightComponent::InitDirLightMatrices(const CU::Matrix4x4f& aPlayerTransform, float aLightBoundsRadius)
{
	LightBounds bounds = {};
	bounds.Origin = aPlayerTransform.GetTranslation();
	bounds.Radius = aLightBoundsRadius;

	CU::Matrix4x4f lightTransformMatrix = {};
	
	lightTransformMatrix.SetTranslation(aPlayerTransform.GetTranslation());
	lightTransformMatrix = lightTransformMatrix * CU::Matrix4x4f::CreateRotationAroundX(DEFAULT_DIRLIGHT_XROTATION);
	lightTransformMatrix = lightTransformMatrix * CU::Matrix4x4f::CreateRotationAroundY(DEFAULT_DIRLIGHT_YROTATION);
	lightTransformMatrix.SetTranslation(aPlayerTransform.GetTranslation() + (-1.f * bounds.Radius * myLightData->LightDir));

	SetLightDirection(lightTransformMatrix.GetRow(3).ToVector3());
	SetLightPosition(lightTransformMatrix.GetRow(4).ToVector3());

	const CU::Vector3<float> targetPos = bounds.Origin;
	const CU::Vector3<float> globalUpDir = CU::Vector3<float>({ 0.0f, 1.0f, 0.0f });
	myLightData->LightViewInv = CU::Matrix4x4<float>::CreateLookAt(myLightData->LightPos, aPlayerTransform.GetTranslation(), globalUpDir);
	myLightData->LightViewInv = myLightData->LightViewInv.GetFastInverse();

	myLightData->LightViewInv = GetGameObject()->GetTransform()->GetMatrixScale().GetFastInverse();
	
	const CU::Vector3<float> frustrumCenter = { bounds.Origin.x, bounds.Origin.y, (bounds.Radius) };
	const float leftPlane = frustrumCenter.x - bounds.Radius;
	const float bottomPlane = frustrumCenter.y - bounds.Radius;
	const float nearPlane = frustrumCenter.z - bounds.Radius * 2.0f;
	const float rightPlane = frustrumCenter.x + bounds.Radius;
	const float topPlane = frustrumCenter.y + bounds.Radius;
	const float farPlane = frustrumCenter.z + bounds.Radius * 2.0f;
	myLightData->LightProj = CU::Matrix4x4<float>::CreateOrthoGraphic(leftPlane, rightPlane, bottomPlane, topPlane, nearPlane, farPlane);
	
	if (GetGameObject()->GetComponent<ShadowCameraComponent>())
	{
		GetGameObject()->GetComponent<ShadowCameraComponent>()->Init(myLightData->LightViewInv, myLightData->LightProj, myLightData->LightPos);
	}
}

void LightComponent::SetColor(const CU::Vector3f& aColor)
{
	myLightData->Color = aColor;
}

void LightComponent::SetIntensity(float aIntensity)
{
	myLightData->Intensity = aIntensity;
}

void LightComponent::SetLightDirection(const CU::Vector3f& aLightDirection)
{
	myLightData->LightDir = aLightDirection;
}

void LightComponent::SetLightPosition(const CU::Vector3f& aLightPosition)
{
	myLightData->LightPos = aLightPosition;
}

void LightComponent::SetConeAngle(float aAngleDeg)
{
	myLightData->OuterConeAngle = aAngleDeg;
}

void LightComponent::SetLightType(LightType aLightMode)
{
	myLightData->LightMode = static_cast<int>(aLightMode);
}

void LightComponent::SetLightProj(CU::Matrix4x4f aLightProj)
{
	myLightData->LightProj = aLightProj;
}

void LightComponent::SetLightViewInverse(CU::Matrix4x4f aLightViewInverse)
{
	myLightData->LightViewInv = aLightViewInverse;
}

std::shared_ptr<LightData> LightComponent::GetLightData() const
{
	return myLightData;
}

int LightComponent::GetLightType() const
{
	return myLightData->LightMode;
}


void LightComponent::Render()
{

}


