#include "FootStepsComponent.h"

#include "../FeatherEngine/GameObjectSystem/GameObject.h"
#include "../FeatherEngine/GameObjectSystem/Components/ShaderObjectComponent.h"
#include "../FeatherEngine/GameObjectSystem/Components/MeshComponent.h"
#include "../FeatherEngine/GameObjectSystem/Components/Animations/Animator.h"
#include "../FeatherEngine/GameObjectSystem/Components/Transform.h"
#include "../FeatherEngine/GameObjectSystem/ComponentSystem.h"
#include "../FeatherEngine/AssetManagement/AssetManager.h"


void FootStepsComponent::Init(Animator* anAnimator)
{
	for (int index = 0; index < AMOUNT_OF_FOOTSTEPS; index++)
	{
		myLeftSteps[index] = std::make_shared<GameObject>();
		myLeftSteps[index]->AddComponent(ComponentSystem::Get()->AddComponent<Transform, TRANSFORM>())->SetGameObject(myLeftSteps[index]);
		myLeftSteps[index]->AddComponent(ComponentSystem::Get()->AddComponent<MeshComponent, MESH_COMPONENT>())->SetGameObject(myLeftSteps[index]);
		myLeftSteps[index]->AddComponent(ComponentSystem::Get()->AddComponent<ShaderObjectComponent, SHADER_OBJECT_COMPONENT>())->SetGameObject(myLeftSteps[index]);
		myLeftSteps[index]->GetComponent<ShaderObjectComponent>()->Init(0.0f, FOOTSTEP_VISIBILITY_TIME, "SM_FootprintL", "FootPrint_PS");

		myRightSteps[index] = std::make_shared<GameObject>();
		myRightSteps[index]->AddComponent(ComponentSystem::Get()->AddComponent<Transform, TRANSFORM>())->SetGameObject(myRightSteps[index]);
		myRightSteps[index]->AddComponent(ComponentSystem::Get()->AddComponent<MeshComponent, MESH_COMPONENT>())->SetGameObject(myRightSteps[index]);
		myRightSteps[index]->AddComponent(ComponentSystem::Get()->AddComponent<ShaderObjectComponent, SHADER_OBJECT_COMPONENT>())->SetGameObject(myRightSteps[index]);
		myRightSteps[index]->GetComponent<ShaderObjectComponent>()->Init(0.0f, FOOTSTEP_VISIBILITY_TIME, "SM_FootprintR", "FootPrint_PS");
	}
	if (anAnimator)
	{
		anAnimator->SubscribeToEvent(LEFT_FOOT_EVENT_NAME, this);
		//anAnimator->SubscribeToEvent("rightfoot", this);
		myAnimator = anAnimator;
	}
}

void FootStepsComponent::SetLeftBone(CU::Matrix4x4f* aJointTransform, const CU::Matrix4x4f& aBindPose)
{
	myLeftFoot.BindPose = aBindPose.GetFastInverse();
	myLeftFoot.JoinTransform = aJointTransform;
	//for (int index = 0; index < myLeftSteps.size(); index++) 
	//{
	//	myLeftSteps[index]->GetComponent<Transform>()->AddJointParent(aJointTransform, aBindPose);
	//	myLeftSteps[index]->GetComponent<Transform>()->AddObjectParent(GetGameObject()->GetComponent<Transform>());
	//}
}

void FootStepsComponent::SetRightBone(CU::Matrix4x4f* aJointTransform, const CU::Matrix4x4f& aBindPose)
{
	myRightFoot.BindPose = aBindPose.GetFastInverse();
	myRightFoot.JoinTransform = aJointTransform;
	//for (int index = 0; index < myLeftSteps.size(); index++)
	//{
	//	myRightSteps[index]->GetComponent<Transform>()->AddJointParent(aJointTransform, aBindPose);
	//	myRightSteps[index]->GetComponent<Transform>()->AddObjectParent(GetGameObject()->GetComponent<Transform>());
	//}
}

void FootStepsComponent::Destroy()
{
	for (int index = 0; index < AMOUNT_OF_FOOTSTEPS; index++)
	{
		if (myLeftSteps[index])
		{
			myLeftSteps[index]->Destroy();
			myLeftSteps[index] = nullptr;
		}
		if (myRightSteps[index])
		{
			myRightSteps[index]->Destroy();
			myRightSteps[index] = nullptr;
		}
	}

	myAnimator->UnSubscribeFromEvent(LEFT_FOOT_EVENT_NAME, this);
	//myAnimator->UnSubscribeFromEvent("rightFoot", this);
}

void FootStepsComponent::Update(const float& aDeltaTime)
{
	aDeltaTime;

	//for (int index = 0; index < AMOUNT_OF_FOOTSTEPS; index++)
	//{
	//	if (myLeftSteps[index])
	//	{
	//		myLeftSteps[index]->Update(aDeltaTime);
	//	}
	//
	//	if (myRightSteps[index])
	//	{
	//		myRightSteps[index]->Update(aDeltaTime);
	//	}
	//}
}

void FootStepsComponent::Render()
{
	for (int index = 0; index < AMOUNT_OF_FOOTSTEPS; index++)
	{
		if (myLeftSteps[index])
		{
			myLeftSteps[index]->Render();
		}
	
		if (myRightSteps[index])
		{
			myRightSteps[index]->Render();
		}
	}
}

void FootStepsComponent::PlaceStep(const Foot aFoot)
{
	const CU::Matrix4x4f& parentTransform = GetGameObject()->GetTransform()->GetMatrixScale();
	CU::Matrix4x4f jointWorld;
	switch (aFoot)
	{
	case Foot::Left:
		jointWorld = myLeftFoot.BindPose * *myLeftFoot.JoinTransform;
		jointWorld = jointWorld * parentTransform;
		myLeftSteps[myCurrentLeftIndex]->GetTransform()->SetTransform(jointWorld);
		myLeftSteps[myCurrentLeftIndex]->GetComponent<ShaderObjectComponent>()->Start();
		myCurrentLeftIndex = (myCurrentLeftIndex + 1) % AMOUNT_OF_FOOTSTEPS;
		break;
	case Foot::Right:
		jointWorld = myRightFoot.BindPose * *myRightFoot.JoinTransform;
		jointWorld = jointWorld * parentTransform;
		myRightSteps[myCurrentRightIndex]->GetTransform()->SetTransform(jointWorld);
		myRightSteps[myCurrentRightIndex]->GetComponent<ShaderObjectComponent>()->Start();
		myCurrentRightIndex = (myCurrentRightIndex + 1) % AMOUNT_OF_FOOTSTEPS;
	default:
		break;
	}
}

void FootStepsComponent::OnAnimationEventTrigger(std::string_view anEventName)
{
	if (anEventName == LEFT_FOOT_EVENT_NAME)
	{
		PlaceStep(Foot::Left);
	}
	else
	{
		PlaceStep(Foot::Right);
	}
}

