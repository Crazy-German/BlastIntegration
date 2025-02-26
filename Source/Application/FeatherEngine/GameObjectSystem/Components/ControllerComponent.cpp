#include "FeatherEngine.pch.h"
#include "ControllerComponent.h"

#include "corecrt_math_defines.h"
#define _USE_MATH_DEFINES

#include "MainSingleton.h"
#include "MeshComponent.h"
#include "MVCameraComponent.h"
#include "CommonUtilities/Ray.hpp"
#include "Transform.h"
#include "CommonUtilities/Intersection.hpp"
#include "GameObjectSystem/Scenes/SceneManager.h"
#include "GraphicsEngine/GraphicsEngine.h"
#include "GraphicsEngine/Mesh.h"

using namespace CommonUtilities;

ControllerComponent::ControllerComponent()
{
	//TODO: read settings from json?
	mySpeed = 800.0f;
	myRotationSpeed = 75.0f;
}

ControllerComponent::~ControllerComponent()
{
	MainSingleton::Get().GetInputMapper().UnRegisterFromAll(this);
}

void ControllerComponent::Awake()
{
}

void ControllerComponent::AddAction(ActionEventID aGameAction)
{
	MainSingleton::Get().GetInputMapper().Subscribe(aGameAction, this);
}

void ControllerComponent::Update(const float& aDeltaTime)
{
	if(myDirection.LengthSqr() > 0.001)
	{
		Move(aDeltaTime);
	}

	myDirection = Vector4<float>(0, 0, 0, 0);
}

void ControllerComponent::RecieveEvent(const ActionEvent& anEvent)
{

	switch (anEvent.Id)
	{
	case ActionEventID::MoveCamLeft:
		myDirection += Vector4f{ -1.0f, 0.0f, 0.0f , 0.0f };
		break;
	case ActionEventID::MoveCamRight:
		myDirection += Vector4f{ 1.0f, 0.0f, 0.0f , 0.0f };
		break;
	case ActionEventID::MoveCamForward:
		myDirection += Vector4f{ 0.0f, 0.0f, 1.0f, 0.0f };
		break;
	case ActionEventID::MoveCamBackwards:
		myDirection += Vector4f{ 0.0f, 0.0f, -1.0f, 0.0f };
		break;
	case ActionEventID::FlyUp:
		myDirection += Vector4f{ 0.0f, 1.0f, 0.0f, 0.0f };
		break;
	case ActionEventID::FlyDown:
		myDirection += Vector4f{ 0.0f, -1.0f, 0.0f, 0.0f };
		break;
	case ActionEventID::Rotation:
		break;
	default:
		break;
	}
}

void ControllerComponent::AddSpeed(float aSpeedValue)
{
	mySpeed += aSpeedValue;
}
CU::Vector3f ControllerComponent::GetMouseWorldPosition()
{
	auto& ge = GraphicsEngine::Get();
	auto cameraObject = MainSingleton::Get().GetSceneManager().GetGlobalObjects().Camera;
	auto cameraComponent = cameraObject->GetComponent<MVCameraComponent>();
	std::vector<GameObjectSort> objects = MainSingleton::Get().GetSceneManager().GetSortedObjects();
	if (!cameraComponent)
	{
#ifdef DEBUG
		std::cout << "No camera bound to camera game object";
#endif
		return { 0, 0, 0 };
	}
	Vector2f mousePos;
	mousePos.x = static_cast<float>(Input::GetMousePosition().x);
	mousePos.y = static_cast<float>(Input::GetMousePosition().y);

	float width = ge.GetViewPort().Width;
	float height = ge.GetViewPort().Height;

	mousePos.x = 2 * mousePos.x / width - 1;
	mousePos.y = (-2 * mousePos.y / height) + 1;

	Matrix4x4f projMatrix = cameraComponent->GetProjection();
	float xProj = projMatrix(1, 1);
	float yProj = projMatrix(2, 2);

	mousePos.x /= xProj;
	mousePos.y /= yProj;

	Vector4f rayOrigin = { 0, 0, 0, 1 };
	Vector4f rayDirection = { mousePos.x, mousePos.y, 1, 0 };

	Matrix4x4f view = cameraComponent->GetViewMatrix();

	rayOrigin = rayOrigin * view;
	rayDirection = (rayDirection * view).GetNormalized();
	Ray<float> ray = Ray(rayOrigin.ToVector3(), rayDirection.ToVector3());

	for (size_t i = objects.size() - 1; i > 0; i--)
	{
		if (auto meshComp = objects[i].GameObject->GetComponent<MeshComponent>())
		{
			/*if (objects[i].GameObject->GetComponent<PlayerComponent>())
			{
				continue;
			}*/
			AABB3D aabb = meshComp->GetMesh()->GetBoundingBox();
			Vector3f& min = aabb.GetMinRef();
			Vector3f& max = aabb.GetMaxRef();

			Vector3f pos = objects[i].GameObject->GetTransform()->GetPosition();
			min += pos;
			max += pos;

			Vector3f intersectionPoint = {};
			if (IntersectionAABBRay(aabb, ray, intersectionPoint))
			{

				return intersectionPoint;
			}
		}
	}
	return { 0, 0, 0 };
}

CU::Vector3f ControllerComponent::GetNavMeshPosition()
{
	auto& ge = GraphicsEngine::Get();
	auto cameraObject = MainSingleton::Get().GetSceneManager().GetGlobalObjects().Camera;
	auto cameraComponent = cameraObject->GetComponent<MVCameraComponent>();
//	MeshNavigator* meshNav = MainSingleton::Get().GetSceneManager().GetMeshNavigator();
//	NavMesh* navMesh = meshNav->GetNavMesh();
//	if (!cameraComponent || navMesh == nullptr || navMesh->Chunks.empty())
//	{
//#ifdef DEBUG
//		std::cout << "No camera bound to camera game object";
//#endif
//		return { 0, 0, 0 };
//	}
	Vector2f mousePos;
	mousePos.x = static_cast<float>(Input::GetMousePosition().x);
	mousePos.y = static_cast<float>(Input::GetMousePosition().y);

	mousePos *= EngineSettings::Get().GetCurrentResolutionScale();
	 
	float width = ge.GetViewPort().Width;
	float height = ge.GetViewPort().Height;

	mousePos.x = 2 * mousePos.x / width - 1;
	mousePos.y = (-2 * mousePos.y / height) + 1;

	Matrix4x4f projMatrix = cameraComponent->GetProjection();
	float xProj = projMatrix(1, 1);
	float yProj = projMatrix(2, 2);

	mousePos.x /= xProj;
	mousePos.y /= yProj;

	Vector4f rayOrigin = { 0, 0, 0, 1 };
	Vector4f rayDirection = { mousePos.x, mousePos.y, 1, 0 };

	Matrix4x4f view = cameraComponent->GetViewMatrix();

	rayOrigin = rayOrigin * view;
	rayDirection = (rayDirection * view).GetNormalized();
	Ray<float> ray = Ray(rayOrigin.ToVector3(), rayDirection.ToVector3());
	Triangle<float> triangle;
	
	//for(int chunk = 0; chunk<navMesh->Chunks.size(); chunk++) // is always 1 chink but to be sure future prof we have this for
	//{
	//	for(auto& polygon : navMesh->Chunks[chunk].Polygons)
	//	{
	//		triangle.InitWithPoints(navMesh->Chunks[chunk].Vertices[polygon.Indices[0]].Position.ToVector3(), navMesh->Chunks[chunk].Vertices[polygon.Indices[1]].Position.ToVector3(), navMesh->Chunks[chunk].Vertices[polygon.Indices[2]].Position.ToVector3());
	//		Vector3f intersectionPoint = {};
	//		if(IntersectionRayTriangle(ray, triangle, intersectionPoint))
	//		{
	//			return intersectionPoint;
	//		}
	//	}
	//}

	//Vector3f falsePoint = (rayOrigin + rayDirection*2046.f).ToVector3();

	//Node* finalNode = meshNav->FindClosestNode(falsePoint);
	//Vector3f intersectionPoint = {};
	//IntersectionPlaneRay(Plane(navMesh->Chunks[0].Vertices[finalNode->myIndices[0]].Position.ToVector3(), navMesh->Chunks[0].Vertices[finalNode->myIndices[1]].Position.ToVector3(), navMesh->Chunks[0].Vertices[finalNode->myIndices[2]].Position.ToVector3()), ray, intersectionPoint);
	//return intersectionPoint;
	//for (size_t i = navMesh->Chunks[0]() - 1; i > 0; i--)
	//{
	//	if (auto meshComp = objects[i].GameObject->GetComponent<MeshComponent>())
	//	{
	//		/*if (objects[i].GameObject->GetComponent<PlayerComponent>())
	//		{
	//			continue;
	//		}*/
	//		AABB3D aabb = meshComp->GetMesh()->GetBoundingBox();
	//		Vector3f& min = aabb.GetMinRef();
	//		Vector3f& max = aabb.GetMaxRef();

	//		Vector3f pos = objects[i].GameObject->GetTransform()->GetPosition();
	//		min += pos;
	//		max += pos;

	//		Vector3f intersectionPoint = {};
	//		if (IntersectionAABBRay(aabb, ray, intersectionPoint))
	//		{

	//			return intersectionPoint;
	//		}
	//	}
	//}
	return { 0, 0, 0 };
}

GameObject* ControllerComponent::GetMouseGameObject()
{
	auto& ge = GraphicsEngine::Get();
	auto cameraObject = MainSingleton::Get().GetSceneManager().GetGlobalObjects().Camera;
	auto cameraComponent = cameraObject->GetComponent<MVCameraComponent>();
	std::vector<GameObjectSort> objects = MainSingleton::Get().GetSceneManager().GetSortedObjects();

	if (objects.empty())
	{
		return nullptr;
	}

	if (!cameraComponent)
	{
#ifdef debug
		std::cout << "No camera bound to camera game object";
#endif
		return nullptr;
	}
	Vector2f mousePos;
	mousePos.x = static_cast<float>(Input::GetMousePosition().x);
	mousePos.y = static_cast<float>(Input::GetMousePosition().y);

	float width = ge.GetViewPort().Width;
	float height = ge.GetViewPort().Height;

	mousePos.x = 2 * mousePos.x / width - 1;
	mousePos.y = (-2 * mousePos.y / height) + 1;

	Matrix4x4f projMatrix = cameraComponent->GetProjection();
	float xProj = projMatrix(1, 1);
	float yProj = projMatrix(2, 2);

	mousePos.x /= xProj;
	mousePos.y /= yProj;

	Vector4f rayOrigin = { 0, 0, 0, 1 };
	Vector4f rayDirection = { mousePos.x, mousePos.y, 1, 0 };

	Matrix4x4f view = cameraComponent->GetViewMatrix();
	Vector3f cameraPos = cameraComponent->GetGameObject()->GetTransform()->GetPosition();

	rayOrigin = rayOrigin * view;
	rayDirection = (rayDirection * view).GetNormalized();
	Ray<float> ray = Ray(rayOrigin.ToVector3(), rayDirection.ToVector3());

	for (size_t i = objects.size() - 1; i > 0; i--)
	{
		if (auto meshComp = objects[i].GameObject->GetComponent<MeshComponent>())
		{
			/*if (objects[i].GameObject->GetComponent<PlayerComponent>())
			{
				continue;
			}*/
			AABB3D aabb = meshComp->GetMesh()->GetBoundingBox();
			Vector3f& min = aabb.GetMinRef();
			Vector3f& max = aabb.GetMaxRef();

			Vector3f pos = objects[i].GameObject->GetTransform()->GetPosition();
			min += pos;
			max += pos;

			Vector3f intersectionPoint = {};
			if (IntersectionAABBRay(aabb, ray, intersectionPoint) && objects[i].GameObject->GetEnemyType() != eEnemyType::None)
			{
				return objects[i].GameObject.get();
			}
		}
	}
	return nullptr;
}

void ControllerComponent::Move(const float& aDeltaTime)
{
	auto deltaPosition = (myDirection.GetNormalized() * mySpeed * aDeltaTime * GetGameObject()->GetTransform()->GetMatrixScale()).ToVector3();
	GetGameObject()->GetTransform()->AddPosition(deltaPosition);
}

