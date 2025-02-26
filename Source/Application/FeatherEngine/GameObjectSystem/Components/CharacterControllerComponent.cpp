#include "FeatherEngine.pch.h"
#include "CharacterControllerComponent.h"

#include "Physcics/PhysicsEngine.h"
#include "Physcics/PhysXScene.h"

CharacterControllerComponent::CharacterControllerComponent()
{
}

CharacterControllerComponent::~CharacterControllerComponent()
{
}

void CharacterControllerComponent::Destroy()
{
	if(myContoller != nullptr)
	{
		Squish::PhysicsEngine::Get()->RemoveController(myContoller);
	}
	myIsMarkedForDestroy = true;
	myIsEnabled = false;
}

void CharacterControllerComponent::Awake()
{
	Component::Awake();
}

void CharacterControllerComponent::Update(const float& aDeltaTime)
{
	CharacterController::Update(aDeltaTime);
}

const Squish::ActorUserData* CharacterControllerComponent::GetUserData() const
{
	return CharacterController::GetUserData();
}
