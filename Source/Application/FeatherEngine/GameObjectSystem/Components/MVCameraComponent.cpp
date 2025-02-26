#include "FeatherEngine.pch.h"
#include "MVCameraComponent.h"

#include "MainSingleton.h"
#include "CommonUtilities/Input.h"
#include "GraphicsEngine/GfxCmd_SetFrameBuffer.h"
#include "GraphicsEngine/GraphicsEngine.h"
#include "GraphicsEngine/Texture.h"
#include "Transform.h"

#include "GraphicsEngine\DebugLineObject.h"
#include "GraphicsEngine\GfxCmd_DrawDebugLines.h"

#include "GraphicsEngine/Mesh.h"
#include "AssetManagement\AssetManager.h"
#include <numbers>

#include "Application/Game/Components/PlayerComponent.h"

MVCameraComponent::MVCameraComponent()
{
	myMoveSpeed = 2000.f;
	myProjection = CU::Matrix4x4f();
	myFrameBuffer = std::make_shared<FrameBuffer>();
	myFrameBuffer->MistModifier = 1;
	myDeltaTime = 0.0f;
	myTotalTime = 0.0f;
	myPass = DebugPass::Game;
	myCanRotateCam = true;
	myGameJoint = nullptr;
	myGameJointRot = {};
}

MVCameraComponent::~MVCameraComponent()
{
	MainSingleton::Get().GetInputMapper().UnRegisterFromAll(this);
	myGameJoint = nullptr;
}

void MVCameraComponent::Awake()
{
	Component::Awake();
	MainSingleton::Get().GetInputMapper().Subscribe(ActionEventID::Change_DebugPass, this);
	MainSingleton::Get().GetInputMapper().Subscribe(ActionEventID::CameraMovedX, this);
	MainSingleton::Get().GetInputMapper().Subscribe(ActionEventID::CameraMovedY, this);
	MainSingleton::Get().GetInputMapper().Subscribe(ActionEventID::CameraMovedY, this);
	InitFrustum();
}

void MVCameraComponent::CreateProjection(const float aHorizontalFOV, const float aAspectRatio, const float aNearPlane,
                                         const float aFarPlane)
{
	for (int row = 1; row < 5; row++)
	{
		for (int column = 1; column < 5; column++)
		{
			myProjection(row, column) = 0;
		}
	}
	myProjection = CU::Matrix4x4f::CreatePerspective(aHorizontalFOV, aAspectRatio, aNearPlane, aFarPlane);

	myFOVRadians = aHorizontalFOV * (3.1415926f / 180.0f);
	myAspectRatio = aAspectRatio;
	myNearPlaneDistance = aNearPlane;
	myFarPlaneDistance = aFarPlane;

}

void MVCameraComponent::InitFrustum()
{
	myFrustum = std::make_shared<CU::PlaneVolume<float>>();
	UpdateFrustum();
}

void MVCameraComponent::Update(const float& aDeltaTime)
{
	//UpdateFrustum();
	UpdateTransform({0,0});
	//if (!myFrustumInitalized)
	//{
	//	UpdateFrustum();
	//	myFrustumInitalized = true;
	//}
	myDeltaTime = aDeltaTime;
	myTotalTime += aDeltaTime;

	//Todo remove when fog wall is done
	if(CU::Input::GetKeyHeld(CU::Keys::NUMPAD8))
	{
		AddMist(-aDeltaTime);
	}
	if (CU::Input::GetKeyHeld(CU::Keys::NUMPAD2))
	{
		AddMist(aDeltaTime);
	}
	if (CU::Input::GetKeyDown(CU::Keys::NUMPAD5))
	{
		MainSingleton::Get().GetSceneManager().GetGlobalObjects().Player->GetTransform()->AddRotation(CU::Vector3f(0, 180, 0));
	}
}


void MVCameraComponent::Render()
{
	MainSingleton::Get().GetRenderer().Enqueue<GfxCmd_SetFrameBuffer>(GetFrameBuffer());
}

void MVCameraComponent::DebugRender()
{
	MainSingleton::Get().GetRenderer().Enqueue<GfxCmd_DrawDebugLines>(myDebugLines, myStartTransform);
}

void MVCameraComponent::UpdateTransform(CU::Vector2f aDeltaRotation)
{
	if(MainSingleton::Get().GetMouseLock() && myGameJoint)
	{
		myGameJointRot -= aDeltaRotation;

		if(myGameJointRot.y > 1.07f)
		{
			myGameJointRot.y = 1.07f;
		}
		if(myGameJointRot.y < -1.07)
		{
			myGameJointRot.y = -1.07f;
		}

		*myGameJoint = CU::Matrix4x4f::CreateRotationAroundX(myGameJointRot.y);
		*myGameJoint *= CU::Matrix4x4f::CreateRotationAroundY(myGameJointRot.x);
	}
}

//void MVCameraComponent::GoToCutscene(float aTimeToBlend)
//{
//	//CU::Vector3f gameCamPos = myGameJoint->GetTranslation();
//	//CU::Vector3f cutsceneCamPos = myAnimJoint->GetTranslation();
//
//
//	//myGameJoint->SetTranslation()
//}
//
//void MVCameraComponent::GoToGameplay(float aTimeToBlend)
//{
//
//}

void MVCameraComponent::SetSpeed(float aSpeedValue)
{
	myMoveSpeed = aSpeedValue;
}

void MVCameraComponent::AddSpeed(float aSpeedValue)
{
	myMoveSpeed += aSpeedValue;
}

const CU::Matrix4x4f& MVCameraComponent::GetViewMatrix()
{
	return GetGameObject()->GetComponent<Transform>()->GetMatrixScale();
}

std::shared_ptr<FrameBuffer> MVCameraComponent::GetFrameBuffer()
{
	myFrameBuffer->InvView = GetGameObject()->GetComponent<Transform>()->GetMatrix().GetFastInverse();
	myFrameBuffer->Projection = myProjection;
	if (myGameJoint != nullptr)
	{
		//myFrameBuffer->CameraPos = (GetGameObject()->GetComponent<Transform>()->GetMatrix() * *myGameJoint).GetTranslation();
		if (GetGameObject()->GetTransform()->GetParent() != nullptr)
		{
			myFrameBuffer->CameraPos = GetGameObject()->GetComponent<Transform>()->GetMatrix().GetTranslation(); //maybe dumbass solution but i think it works
		}
	}
	myFrameBuffer->Resolution = CU::Vector2f(static_cast<float>(GraphicsEngine::Get().GetViewPort().Width), static_cast<float>(GraphicsEngine::Get().GetViewPort().Height));
	myFrameBuffer->TotalTime = myTotalTime;
	myFrameBuffer->DeltaTime = myDeltaTime;
	myFrameBuffer->DebugPass = static_cast<int>(myPass);
	return myFrameBuffer;
}
CU::Vector3f MVCameraComponent::ToWorldSpace(const CU::Vector3f& aCameraSpacePosition) const
{
	return (aCameraSpacePosition.ToVector4Zero() * GetGameObject()->GetComponent<Transform>()->GetMatrix()).ToVector3();
}

bool MVCameraComponent::IsInsideFrustum(const CU::Vector3f& aWorldMin, const CU::Vector3f& aWorldMax)
{
	for (auto& plane : myFrustum->GetPlanes())
	{
		CU::Vector3f P;
		CU::Vector3f Q;
		CU::Vector4f worldMin = aWorldMin.ToVector4One() * myStartTransform.GetFastInverse();
		CU::Vector4f worldMax = aWorldMax.ToVector4One() * myStartTransform.GetFastInverse();
		if (plane.GetNormal().x >= 0)
		{
			P.x = worldMin.x;
			Q.x = worldMax.x;
		}
		else
		{
			P.x = worldMax.x;
			Q.x = worldMin.x;
		}
		if (plane.GetNormal().y >= 0)
		{
			P.y = worldMin.y;
			Q.y = worldMax.y;
		}
		else
		{
			P.y = worldMax.y;
			Q.y = worldMin.y;
		}
		if (plane.GetNormal().z >= 0)
		{
			P.z = worldMin.z;
			Q.z = worldMax.z;
		}
		else
		{
			P.z = worldMax.z;
			Q.z = worldMin.z;
		}
		//CU::Vector3f PQ = Q - P;
	
		if (plane.GetNormal().Dot(Q - plane.GetPoint()) <= 0)
		{
			return false;
		}
	}
	return true;
}

void MVCameraComponent::RecieveEvent(const ActionEvent& anEvent)
{
	float degToRag = std::numbers::pi_v<float> / 180.f;
	float deltaCameraRot = anEvent.Weight * myDeltaTime * myRotationSpeed * degToRag;
	switch (anEvent.Id)
	{
	case ActionEventID::Change_DebugPass:
		myPass = static_cast<DebugPass>((static_cast<int>(myPass) + 1) % static_cast<int>(DebugPass::Count));
		break;
	case ActionEventID::CameraMovedX:
		UpdateTransform(CU::Vector2f(deltaCameraRot, 0));
		break;
	case ActionEventID::CameraMovedY:
		UpdateTransform(CU::Vector2f(0, deltaCameraRot));
		break;
	default:
		std::cout << "Camera has received an event it shouldn't\n";
	}
}

bool MVCameraComponent::IsDebugRendering() const
{
	if(myPass == DebugPass::Game)
	{
		return false;
	}
	return true;
}

void MVCameraComponent::SetCamJoint(CU::Matrix4x4f* aCamJoint)
{
	myGameJoint = aCamJoint;
}

void MVCameraComponent::AddYRotation(float aRotation)
{
	if(aRotation > 1.f)
	{
		__debugbreak();
	}
	myGameJointRot.x += aRotation;
}

void MVCameraComponent::AddMist(float aDeltaMistModifier)
{
	myFrameBuffer->MistModifier += aDeltaMistModifier;
}

void MVCameraComponent::SetMist(float aMistModifier)
{
	myFrameBuffer->MistModifier += aMistModifier;
}

void MVCameraComponent::UpdateFrustum()
{
	myStartTransform = MainSingleton::Get().GetSceneManager().GetGlobalObjects().Player->GetTransform()->GetMatrixScale();
	//myStartTransform = myGameObject->GetTransform()->GetMatrixScale();
	CU::Vector3f camForward = myStartTransform.GetRow(3).ToVector3();
	camForward = { 0.0f, 0.0f, 1.0f };
	CU::Vector3f camUp = myStartTransform.GetRow(2).ToVector3();
	camUp = { 0.0f, 1.0f, 0.0f };
	CU::Vector3f camRight = myStartTransform.GetRow(1).ToVector3();
	camRight = { 1.0f, 0.0f, 0.0f };
	CU::Vector3f camPos = myStartTransform.GetTranslation();
	camPos = { 0,0,0 };

	float farPlaneDeltaDistance = myFarPlaneDistance /** 0.1f*/;

	CU::Vector3f nearCenter = camPos + (camForward * myNearPlaneDistance);
	CU::Vector3f farCenter = camPos + (camForward * farPlaneDeltaDistance);
	float nearHeight = 2 * tan(myFOVRadians * 0.5f) * myNearPlaneDistance;
	float nearWidth = nearHeight * myAspectRatio;
	float farHeight = 2 * tan(myFOVRadians * 0.5f) * farPlaneDeltaDistance;
	float farWidth = farHeight * myAspectRatio;

	CU::Vector3f farTopLeft = farCenter + (camUp * (farHeight * 0.5f)) - (camRight * (farWidth * 0.5f));
	CU::Vector3f farTopRight = farCenter + (camUp * (farHeight * 0.5f)) + (camRight * (farWidth * 0.5f));
	CU::Vector3f farBottomLeft = farCenter - (camUp * (farHeight * 0.5f)) - (camRight * (farWidth * 0.5f));
	CU::Vector3f farBottomRight = farCenter - (camUp * (farHeight * 0.5f)) + (camRight * (farWidth * 0.5f));

	CU::Vector3f nearTopLeft = nearCenter + (camUp * (nearHeight * 0.5f)) - (camRight * (nearWidth * 0.5f));
	CU::Vector3f nearTopRight = nearCenter + (camUp * (nearHeight * 0.5f)) + (camRight * (nearWidth * 0.5f));
	CU::Vector3f nearBottomLeft = nearCenter - (camUp * (nearHeight * 0.5f)) - (camRight * (nearWidth * 0.5f));
	CU::Vector3f nearBottomRight = nearCenter - (camUp * (nearHeight * 0.5f)) + (camRight * (nearWidth * 0.5f));

	CU::Vector3f leftPlaneNormal =		-1.0f * (farBottomLeft - nearBottomLeft).Cross(farTopLeft - farBottomLeft).GetNormalized();
	CU::Vector3f rightPlaneNormal =		-1.0f * (farTopRight - nearTopRight).Cross(farBottomRight - farTopRight).GetNormalized();
	CU::Vector3f bottomPlaneNormal =	-1.0f * (farBottomRight - nearBottomRight).Cross(farBottomLeft - farBottomRight).GetNormalized();
	CU::Vector3f topPlaneNormal =		-1.0f * (farTopLeft - nearTopLeft).Cross(farTopRight - farTopLeft).GetNormalized();
	CU::Vector3f nearPlaneNormal =		-1.0f * (nearTopLeft - nearTopRight).Cross(nearTopLeft - nearBottomLeft).GetNormalized();
	CU::Vector3f farPlaneNormal =		-1.0f * (farTopRight - farTopLeft).Cross(farTopRight - farBottomRight).GetNormalized();

	myFrustum->RemoveAllPlanes();
	CU::Plane<float> nearPlane;
	nearPlane.InitWithPointAndNormal(nearBottomLeft, nearPlaneNormal);
	myFrustum->AddPlane(nearPlane);

	CU::Plane<float> farPlane;
	farPlane.InitWithPointAndNormal(farBottomLeft, farPlaneNormal);
	myFrustum->AddPlane(farPlane);

	CU::Plane<float> leftPlane;
	leftPlane.InitWithPointAndNormal(farTopLeft, leftPlaneNormal);
	myFrustum->AddPlane(leftPlane);

	CU::Plane<float> rightPlane;
	rightPlane.InitWithPointAndNormal(farTopRight, rightPlaneNormal);
	myFrustum->AddPlane(rightPlane);

	CU::Plane<float> topPlane;
	topPlane.InitWithPointAndNormal(nearTopRight, topPlaneNormal);
	myFrustum->AddPlane(topPlane);

	CU::Plane<float> bottomPlane;
	bottomPlane.InitWithPointAndNormal(nearBottomLeft, bottomPlaneNormal);
	myFrustum->AddPlane(bottomPlane);

	myDebugLines = std::make_shared<DebugLineObject>();
	std::vector<unsigned> indices;
	std::vector<Vertex> vertices;


	vertices.emplace_back(farTopLeft);
	vertices.emplace_back(farTopRight);
	indices.emplace_back(0);
	indices.emplace_back(1);
	
	vertices.emplace_back(farBottomLeft);
	vertices.emplace_back(farBottomRight);
	indices.emplace_back(2);
	indices.emplace_back(3);
	
	vertices.emplace_back(farTopLeft);
	vertices.emplace_back(farBottomLeft);
	indices.emplace_back(4);
	indices.emplace_back(5);
	
	vertices.emplace_back(farTopRight);
	vertices.emplace_back(farBottomRight);
	indices.emplace_back(6);
	indices.emplace_back(7);
	
	vertices.emplace_back(nearTopLeft);
	vertices.emplace_back(farTopLeft);
	indices.emplace_back(8);
	indices.emplace_back(9);
	
	vertices.emplace_back(nearBottomLeft);
	vertices.emplace_back(farBottomLeft);
	indices.emplace_back(10);
	indices.emplace_back(11);
	
	vertices.emplace_back(nearTopRight);
	vertices.emplace_back(farTopRight);
	indices.emplace_back(12);
	indices.emplace_back(13);
	
	vertices.emplace_back(nearBottomRight);
	vertices.emplace_back(farBottomRight);
	indices.emplace_back(14);
	indices.emplace_back(15);
	
	vertices.emplace_back(nearBottomLeft);
	vertices.emplace_back(nearBottomRight);
	indices.emplace_back(16);
	indices.emplace_back(17);
	
	vertices.emplace_back(nearTopLeft);
	vertices.emplace_back(nearTopRight);
	indices.emplace_back(18);
	indices.emplace_back(19);
	
	vertices.emplace_back(nearBottomLeft);
	vertices.emplace_back(nearTopLeft);
	indices.emplace_back(20);
	indices.emplace_back(21);
	
	vertices.emplace_back(nearBottomRight);
	vertices.emplace_back(nearTopRight);
	indices.emplace_back(22);
	indices.emplace_back(23);

	myDebugLines->Initialize(vertices, indices, DebugColor::Yellow, "Camera frustum");
}

