#include "FeatherEngine.pch.h"
#include "ShaderObjectComponent.h"

#include "Graphics\GraphicsEngine\MiscBuffer.h"

#include "GraphicsEngine\GfxCmd_SetMiscBuffer.h"
#include <GameObjectSystem\PostMaster\GameObjectPostMaster.h>
#include <MainSingleton.h>

#include "MeshComponent.h"
#include <AssetManagement\AssetTypes\MeshAsset.h>
#include "Animations/Animator.h"



void ShaderObjectComponent::Awake()
{
}

void ShaderObjectComponent::Init(const float aDelay, const float aLifeTime, const char* aMesh, std::string_view aPixelShader, std::string_view aVertexShader)
{
	mySettings.StartDelay = aDelay;
	myTimer = -aDelay;
	myMiscBuffer = std::make_shared<MiscBuffer>();
	mySettings.LifeTime = aLifeTime;
	GetGameObject()->SetIsVisible(false);
	GetGameObject()->GetComponent<MeshComponent>()->SetPSShader(aPixelShader);
	if (!aVertexShader.empty())
	{
		GetGameObject()->GetComponent<MeshComponent>()->SetVSShader(aVertexShader);
	}
	if (aMesh)
	{
		GetGameObject()->GetComponent<MeshComponent>()->SetMesh(AssetManager::Get().GetAsset<MeshAsset>(aMesh)->myMesh);
	}
}

void ShaderObjectComponent::InitFromJson(nlohmann::json& aJsonObject)
{
	myMiscBuffer = std::make_shared<MiscBuffer>();
	GetGameObject()->SetIsVisible(false);

	std::shared_ptr<GameObject> parent;
	if (aJsonObject.contains("Name"))
	{
		myName = aJsonObject["Name"];
	}
	if (aJsonObject.contains("Parent"))
	{
		if (!aJsonObject["Parent"].empty())
		{
			if (aJsonObject["Parent"] == "Player")
			{
				mySettings.HasParent = true;
				parent = MainSingleton::Get().GetSceneManager().GetGlobalObjects().Player;
			}
			else
			{
				auto& gameObjects = MainSingleton::Get().GetSceneManager().GetSortedObjects();
				for (auto& object : gameObjects)
				{
					std::string parentName = aJsonObject["Parent"];
					if (object.GameObject->GetName() == parentName)
					{
						mySettings.HasParent = true;
						parent = object.GameObject;
						break;
					}
				}
			}
			if (mySettings.HasParent)
			{
				GetGameObject()->GetTransform()->AddObjectParent(parent->GetComponent<Transform>());
				if (aJsonObject.contains("ParentBone"))
				{
					if (!aJsonObject["ParentBone"].empty())
					{
						const auto& meshComponent = parent->GetComponent<MeshComponent>();
						const auto& animator = parent->GetComponent<Animator>();
						if (!parent->GetComponent<MeshComponent>()->GetMesh()->GetSkeleton().Joints.empty())
						{
							const int boneIndex = meshComponent->GetJoint(aJsonObject["ParentBone"]);
							if (boneIndex >= 0)
							{
								mySettings.HasBoneParent = true;
								GetGameObject()->GetTransform()->AddJointParent(&animator->GetAnimationBuffer()->JointTransforms[boneIndex], meshComponent->GetMesh()->GetSkeleton().Joints[boneIndex].BindPoseInverse);
							}
						}
					}
				}
			}
		}
	}
	if (aJsonObject.contains("Mesh"))
	{
		std::string mesh = aJsonObject["Mesh"];
		GetGameObject()->GetComponent<MeshComponent>()->SetMesh(AssetManager::Get().GetAsset<MeshAsset>(mesh)->myMesh);
	}
	if (aJsonObject.contains("StartDelay"))
	{
		mySettings.StartDelay = aJsonObject["StartDelay"];
		myTimer = -mySettings.StartDelay;
	}
	if (aJsonObject.contains("LifeTime"))
	{
		mySettings.LifeTime = aJsonObject["LifeTime"];
	}
	if (aJsonObject.contains("VertexShader"))
	{
		GetGameObject()->GetComponent<MeshComponent>()->SetVSShader(aJsonObject["VertexShader"]);
	}
	//if (aJsonObject.contains("GeometryShader"))
	//{
	//	GetGameObject()->GetComponent<MeshComponent>()->SetGSShader(aJsonObject["GeometryShader"]);
	//}
	if (aJsonObject.contains("PixelShader"))
	{
		std::string name = aJsonObject["PixelShader"];
		GetGameObject()->GetComponent<MeshComponent>()->SetPSShader(name);
	}
	if (aJsonObject.contains("IsTriggered"))
	{
		mySettings.IsTriggered = aJsonObject["IsTriggered"];
	}
	if (aJsonObject.contains("Retrigger"))
	{
		mySettings.Retrigger = aJsonObject["Retrigger"];
	}
	const float degreeToRadian = 3.14159265359f / 180.0f;
	float rotationX = 0.0f;
	float rotationY = 0.0f;
	float rotationZ = 0.0f;
	CU::Vector3f scale = { 1.0f, 1.0f, 1.0f };
	CU::Vector3f positionOffset = { .0f, .0f, .0f };

	if (aJsonObject.contains("RotationOffset"))
	{
		if (aJsonObject["RotationOffset"].contains("X"))
		{
			rotationX = aJsonObject["RotationOffset"]["X"];
		}
		if (aJsonObject["RotationOffset"].contains("Y"))
		{
			rotationY = aJsonObject["RotationOffset"]["Y"];
		}
		if (aJsonObject["RotationOffset"].contains("Z"))
		{
			rotationZ = aJsonObject["RotationOffset"]["Z"];
		}
		CU::Matrix4x4f rotationAroundX = CU::Matrix4x4f::CreateRotationAroundX(rotationX * degreeToRadian);
		CU::Matrix4x4f rotationAroundY = CU::Matrix4x4f::CreateRotationAroundY(rotationY * degreeToRadian);
		CU::Matrix4x4f rotationAroundZ = CU::Matrix4x4f::CreateRotationAroundZ(rotationZ * degreeToRadian);
		CU::Matrix4x4f transform;
		transform.SetRow(rotationAroundX.GetRow(1), 1);
		transform.SetRow(rotationAroundY.GetRow(2), 2);
		transform.SetRow(rotationAroundZ.GetRow(3), 3);
		SetTransformOffset(transform);
	}
	if (aJsonObject.contains("PositionOffset"))
	{
		if (aJsonObject["PositionOffset"].contains("X"))
		{
			positionOffset.x = aJsonObject["PositionOffset"]["X"];
		}
		if (aJsonObject["PositionOffset"].contains("Y"))
		{
			positionOffset.y = aJsonObject["PositionOffset"]["Y"];
		}
		if (aJsonObject["PositionOffset"].contains("Z"))
		{
			positionOffset.z = aJsonObject["PositionOffset"]["Z"];
		}
		SetPositionOffset(positionOffset);
	}
	if (aJsonObject.contains("Scale"))
	{
		if (aJsonObject["Scale"].contains("X"))
		{
			scale.x = aJsonObject["Scale"]["X"];
		}
		if (aJsonObject["Scale"].contains("Y"))
		{
			scale.y = aJsonObject["Scale"]["Y"];
		}
		if (aJsonObject["Scale"].contains("Z"))
		{
			scale.z = aJsonObject["Scale"]["Z"];
		}
		GetGameObject()->GetTransform()->SetScale(scale);
	}
}

void ShaderObjectComponent::AddEventTrigger(eLetterRegister aLetter)
{
	GameObjectPostMaster::Get()->Register(aLetter, this);
	myEventTrigger = aLetter;
}


void ShaderObjectComponent::Destroy()
{
	myIsMarkedForDestroy = true;
	myMiscBuffer = nullptr;
}

void ShaderObjectComponent::Update(const float& aDeltaTime)
{
	if (!myIsActive)
	{
		return;
	}
	if (myTimer > 0.0f && myTimer < mySettings.LifeTime)
	{
		myMiscBuffer->objectLifeTime += aDeltaTime;
		GetGameObject()->SetIsVisible(true);
	}
	if (myTimer > mySettings.LifeTime)
	{
		myIsActive = false;
		GetGameObject()->SetIsVisible(false);
	}
	myTimer += aDeltaTime;
}

void ShaderObjectComponent::Render()
{
	if (myIsEnabled)
	{
		MainSingleton::Get().GetMiscBuffer()->objectLifeTime = myMiscBuffer->objectLifeTime;
		MainSingleton::Get().GetRenderer().Enqueue<GfxCmd_SetMiscBuffer>(*MainSingleton::Get().GetMiscBuffer());
	}
}

void ShaderObjectComponent::Start()
{
	if (mySettings.Retrigger == false && myHasTriggered)
	{
		return;
	}
	myIsActive = true;
	myHasTriggered = true;
	myTimer = -mySettings.StartDelay;
	myMiscBuffer->objectLifeTime = 0.0f;
	GetGameObject()->SetIsVisible(false);
}

bool ShaderObjectComponent::GetIsActive()
{
	return myIsActive;
}

void ShaderObjectComponent::SetIdentifierIndex(int aIndex)
{
	myIdentifierIndex = aIndex;
}

void ShaderObjectComponent::Put(Letter aLetter)
{
	//This shouldn't be needed
	if (aLetter.myType == myEventTrigger)
	{
		Start();
	}
}

void ShaderObjectComponent::SetPositionOffset(const CU::Vector3f& aPositionOffset)
{
	GetGameObject()->GetTransform()->SetPosition(aPositionOffset);
}

void ShaderObjectComponent::SetTransformOffset(const CU::Matrix4x4f& aRotationMatrix)
{
	CU::Vector3f position = GetGameObject()->GetTransform()->GetPosition();
	GetGameObject()->GetTransform()->SetTransform(aRotationMatrix);
	GetGameObject()->GetTransform()->GetMatrixScale().SetRow(position.ToVector4One(), 4);
}

