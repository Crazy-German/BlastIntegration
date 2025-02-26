#include "FeatherEngine.pch.h"
#include "ComponentSystem.h"
#include "Components/AudioSourceComponent.h"
#include "Components/MusicPlayerComponent.h"
#include "Components/ControllerComponent.h"
#include "Components/EnemyBehavior.h"
#include "Components/MeshComponent.h"
#include "Components/MVCameraComponent.h"
//#include "Components/PlayerComponent.h"
//#include "Components/GroundClickComponent.h"
//#include "Components/ProjectileComponent.h"
#include "Components/CapsuleColliderComponent.h"
#include "Components/CharacterControllerComponent.h"
#include "Components/LevelSwapComponent.h"
#include "Components/OBBColliderComponent.h"
#include "Components/RigidBodyComponent.h"
#include "Components/ScriptComponent.h"
#include "Components/ShadowCameraComponent.h"
#include "Components/SphereColliderComponent.h"
#include "Components/Transform.h"
#include "Components/Lights/LightComponent.h"
#include "Components/ShaderObjectComponent.h"
#include "Components/Animations/Animator.h"

std::unordered_map<const char*, int> ComponentSystem::myIDMap;

ComponentSystem::ComponentSystem()
{
	RegisterComponent<AudioSourceComponent, AUDIO_SOURCE_COMPONENT>();
	RegisterComponent<MusicPlayerComponent, MUSIC_PLAYER_COMPONENT>();
	RegisterComponent<ControllerComponent, CONTROLLER_COMPONENT>();
	RegisterComponent<EnemyBehavior, ENEMY_COMPONENT>();
	RegisterComponent<MeshComponent, MESH_COMPONENT>();
	RegisterComponent<MVCameraComponent, MV_CAMERA_COMPONENT>();
	RegisterComponent<SphereColliderComponent, SPHERE_COLLIDER_COMPONENT>();
	RegisterComponent<CapsuleColliderComponent, CAPSULE_COLLLIDER_COMPONENT>();
	RegisterComponent<OBBColliderComponent, OOB_COLLIDER_COMPONENT>();
	RegisterComponent<ShadowCameraComponent, SHADOW_CAMERA_COMPONENT>();
	RegisterComponent<Transform, TRANSFORM>();
	RegisterComponent<LightComponent, LIGHT_COMPONENT>();
	RegisterComponent<Animator, ANIMATOR>();
	RegisterComponent<LevelSwapComponent, LEVEL_SWAP_COMPONENT>();
	RegisterComponent<ShaderObjectComponent, SHADER_OBJECT_COMPONENT>();
	RegisterComponent<CharacterControllerComponent, CHARACTER_CONTROLLER_COMPONENT>();
	RegisterComponent<RigidBodyComponent, RIGID_BODY_COMPONENT>();
	RegisterComponent<ScriptComponent, SCRIPT_COMPONENT>();
}

std::shared_ptr<ComponentSystem> ComponentSystem::Get()
{
	static std::shared_ptr<ComponentSystem> componentSystem = std::make_shared<ComponentSystem>();
	return componentSystem;
}

void ComponentSystem::Awake()
{
	for (int storeIndex = 0; storeIndex < myComponentStores.size(); storeIndex++)
	{
		myComponentStores[storeIndex]->Awake();
	}
}

void ComponentSystem::Update(const float& aDeltaTime)
{
	for (int storeIndex = 0; storeIndex < myComponentStores.size(); storeIndex++)
	{
		myComponentStores[storeIndex]->Update(aDeltaTime);
	}
}

void ComponentSystem::Render()
{
	for (int storeIndex = 0; storeIndex < myComponentStores.size(); storeIndex++)
	{
		myComponentStores[storeIndex]->Render();
	}
}

void ComponentSystem::CleanUp()
{
	for (int storeIndex = 0; storeIndex < myComponentStores.size(); storeIndex++)
	{
		myComponentStores[storeIndex]->CleanUp();
	}
}
