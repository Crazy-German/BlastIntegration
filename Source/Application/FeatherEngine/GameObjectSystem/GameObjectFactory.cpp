#include "FeatherEngine.pch.h"
#include "GameObjectFactory.h"
#include "ComponentSystem.h"
#include "Components/AudioSourceComponent.h"
#include "Components/ControllerComponent.h"
#include "Components/EnemyBehavior.h"
#include "Components/MeshComponent.h"
#include "Components/MVCameraComponent.h"
//#include "Components/PlayerComponent.h"
//#include "Components/ProjectileComponent.h"
#include "Components/ShadowCameraComponent.h"
#include "Components/Transform.h"
#include "Components/Lights/LightComponent.h"
#include "Components/Animations/Animator.h"
#include"AssetManagement/AssetManager.h"
#include "Components/LevelSwapComponent.h"
#include "Components/RigidBodyComponent.h"
#include "Components/ShaderObjectComponent.h"

GameObjectFactory::GameObjectFactory(std::shared_ptr<ComponentSystem> aComponentSystem) : myComponentSystem(aComponentSystem)
{
	// Här specifierar man vilka slags engine spel objekt som ska finnas
	myComponentTypeMap["StaticMeshActor"] = eGameObjectType::StaticMesh;
	myComponentTypeMap["SkeletalMeshActor"] = eGameObjectType::SkeletalMesh;
	myComponentTypeMap["Light"] = eGameObjectType::Light;
	myComponentTypeMap["Landscape"] = eGameObjectType::Landscape;
	myComponentTypeMap["Trigger"] = eGameObjectType::Trigger;
	myComponentTypeMap["LevelTransition"] = eGameObjectType::Trigger;
	myComponentTypeMap["ShaderObject"] = eGameObjectType::ShaderObject;

	myTagTypeMap[""] = eTagType::Empty;
	myTagTypeMap["Forward"] = eTagType::Forward;
}

void GameObjectFactory::Init()
{
}

void GameObjectFactory::Fabricate(std::shared_ptr<GameObject> outGameObject, const std::string& aGameObjectType)
{
	switch (myComponentTypeMap[aGameObjectType])
	{
	case eGameObjectType::StaticMesh:
		outGameObject->AddComponent(myComponentSystem->AddComponent <MeshComponent, MESH_COMPONENT>())->SetGameObject(outGameObject);
		outGameObject->AddComponent(myComponentSystem->AddComponent<Transform, TRANSFORM>())->SetGameObject(outGameObject);
		outGameObject->AddComponent(myComponentSystem->AddComponent<RigidBodyComponent, RIGID_BODY_COMPONENT>())->SetGameObject(outGameObject);
		break;
	case eGameObjectType::SkeletalMesh:

		outGameObject->AddComponent(myComponentSystem->AddComponent <MeshComponent, MESH_COMPONENT>())->SetGameObject(outGameObject);
		outGameObject->AddComponent(myComponentSystem->AddComponent<Transform, TRANSFORM>())->SetGameObject(outGameObject);
		outGameObject->AddComponent(myComponentSystem->AddComponent<Animator, ANIMATOR>())->SetGameObject(outGameObject);
		outGameObject->AddComponent(myComponentSystem->AddComponent<AudioSourceComponent, AUDIO_SOURCE_COMPONENT>())->SetGameObject(outGameObject);
		outGameObject->AddComponent(myComponentSystem->AddComponent<RigidBodyComponent, RIGID_BODY_COMPONENT>())->SetGameObject(outGameObject);
		break;
	case eGameObjectType::Light:
		outGameObject->AddComponent(myComponentSystem->AddComponent<Transform, TRANSFORM>())->SetGameObject(outGameObject);
		outGameObject->AddComponent(myComponentSystem->AddComponent<LightComponent, LIGHT_COMPONENT>())->SetGameObject(outGameObject);
		break;
	case eGameObjectType::Trigger:
		outGameObject->AddComponent(myComponentSystem->AddComponent<Transform, TRANSFORM>())->SetGameObject(outGameObject);
		outGameObject->AddComponent(myComponentSystem->AddComponent<RigidBodyComponent, RIGID_BODY_COMPONENT>())->SetGameObject(outGameObject);
		outGameObject->AddComponent(myComponentSystem->AddComponent<LevelSwapComponent, LEVEL_SWAP_COMPONENT>())->SetGameObject(outGameObject);
		break;
	case eGameObjectType::Landscape:
		outGameObject->AddComponent(myComponentSystem->AddComponent<Transform, TRANSFORM>())->SetGameObject(outGameObject);
		outGameObject->AddComponent(myComponentSystem->AddComponent <MeshComponent, MESH_COMPONENT>())->SetGameObject(outGameObject);
		outGameObject->AddComponent(myComponentSystem->AddComponent<RigidBodyComponent, RIGID_BODY_COMPONENT>())->SetGameObject(outGameObject);
		break;
	case eGameObjectType::ShaderObject:
		outGameObject->AddComponent(myComponentSystem->AddComponent<Transform, TRANSFORM>())->SetGameObject(outGameObject);
		outGameObject->AddComponent(myComponentSystem->AddComponent <MeshComponent, MESH_COMPONENT>())->SetGameObject(outGameObject);
		outGameObject->AddComponent(myComponentSystem->AddComponent<ShaderObjectComponent, SHADER_OBJECT_COMPONENT>())->SetGameObject(outGameObject);
		break;
	default:
		break;
	}
}

void GameObjectFactory::Fabricate(std::shared_ptr<GameObject> outGameObject, const std::string& aGameObjectType, const std::vector<std::string>& aTags)
{
	switch (myComponentTypeMap[aGameObjectType])
	{
	case eGameObjectType::StaticMesh:
		outGameObject->AddComponent(myComponentSystem->AddComponent <MeshComponent, MESH_COMPONENT>())->SetGameObject(outGameObject);
		outGameObject->AddComponent(myComponentSystem->AddComponent<Transform, TRANSFORM>())->SetGameObject(outGameObject);
		outGameObject->AddComponent(myComponentSystem->AddComponent<RigidBodyComponent, RIGID_BODY_COMPONENT>())->SetGameObject(outGameObject);
		break;
	case eGameObjectType::SkeletalMesh:
		outGameObject->AddComponent(myComponentSystem->AddComponent <MeshComponent, MESH_COMPONENT>())->SetGameObject(outGameObject);
		outGameObject->AddComponent(myComponentSystem->AddComponent<Transform, TRANSFORM>())->SetGameObject(outGameObject);
		outGameObject->AddComponent(myComponentSystem->AddComponent<Animator, ANIMATOR>())->SetGameObject(outGameObject);
		outGameObject->AddComponent(myComponentSystem->AddComponent<AudioSourceComponent, AUDIO_SOURCE_COMPONENT>())->SetGameObject(outGameObject);
		outGameObject->AddComponent(myComponentSystem->AddComponent<RigidBodyComponent, RIGID_BODY_COMPONENT>())->SetGameObject(outGameObject);
		break;
	case eGameObjectType::Light:
		outGameObject->AddComponent(myComponentSystem->AddComponent<Transform, TRANSFORM>())->SetGameObject(outGameObject);
		outGameObject->AddComponent(myComponentSystem->AddComponent<LightComponent, LIGHT_COMPONENT>())->SetGameObject(outGameObject);
		break;
	case eGameObjectType::Trigger:
		outGameObject->AddComponent(myComponentSystem->AddComponent<Transform, TRANSFORM>())->SetGameObject(outGameObject);
		outGameObject->AddComponent(myComponentSystem->AddComponent<RigidBodyComponent, RIGID_BODY_COMPONENT>())->SetGameObject(outGameObject);
		break;
	case eGameObjectType::Landscape:
		outGameObject->AddComponent(myComponentSystem->AddComponent<Transform, TRANSFORM>())->SetGameObject(outGameObject);
		outGameObject->AddComponent(myComponentSystem->AddComponent<MeshComponent, MESH_COMPONENT>())->SetGameObject(outGameObject);
		outGameObject->AddComponent(myComponentSystem->AddComponent<RigidBodyComponent, RIGID_BODY_COMPONENT>())->SetGameObject(outGameObject);
		break;
	case eGameObjectType::MusicPlayer:
		break;
	default:
		break;
	}

	if (aTags.size() > 0)
	{
		for (std::string tag : aTags)
		{
			if (myComponentTypeMap[aGameObjectType] == eGameObjectType::Trigger && tag.starts_with("L_"))
			{
				outGameObject->AddComponent(myComponentSystem->AddComponent<LevelSwapComponent, LEVEL_SWAP_COMPONENT>())->SetGameObject(outGameObject);
				outGameObject->GetComponent<LevelSwapComponent>()->Init(tag);
			}
			switch (myTagTypeMap[tag])
			{

			case eTagType::Forward:
			{
				outGameObject->GetComponent<MeshComponent>()->SetPSShader("Default_PS");
				outGameObject->GetComponent<MeshComponent>()->SetVSShader("Default_VS");
			}
			case eTagType::TA:
				break;
			default:
				break;
			}
		}

	}
}

std::shared_ptr<GameObject> GameObjectFactory::Fabricate(const std::string& aArchetype, const std::vector<std::string>& aTags)
{
	std::shared_ptr<GameObject> newGameObject = std::make_shared<GameObject>();
	Fabricate(newGameObject, aArchetype);
	for (auto& tag : aTags)
	{
		Fabricate(newGameObject, tag);
	}
	return newGameObject;
}

std::shared_ptr<GameObject> GameObjectFactory::Fabricate(const std::string& aArchetype)
{
	return Fabricate(aArchetype, std::vector <std::string>());
}
