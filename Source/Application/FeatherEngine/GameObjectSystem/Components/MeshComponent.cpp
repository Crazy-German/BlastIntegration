#include "FeatherEngine.pch.h"
#include "MeshComponent.h"

#include "EnemyBehavior.h"
#include "MainSingleton.h"
#include "GraphicsEngine/Mesh.h"
#include "GraphicsEngine/Material.h"

#include "AssetManagement/AssetManager.h"
#include "AssetManagement/AssetTypes/MeshAsset.h"
#include "AssetManagement/AssetTypes/MaterialAsset.h"

#include "GraphicsEngine/RenderData.h"
#include "Transform.h"
#include "Animations/Animator.h"
#include <GraphicsEngine/GfxCmd_RenderMesh.h>
#include <GraphicsEngine/GfxCmd_DrawDebugLines.h>

MeshComponent::MeshComponent() = default;

MeshComponent::~MeshComponent()
{
	myMesh = nullptr;
}

void MeshComponent::Awake()
{
	myIsOutlinedA = false;
	myIsOutlinedB = false;
}

void MeshComponent::Destroy()
{
	myMesh.reset();
	myIsMarkedForDestroy = true;
}

void MeshComponent::Update(const float& aDeltaTime)
{
	UNREFERENCED_PARAMETER(aDeltaTime);
}

void MeshComponent::Render()
{
	//float damage = 0;
	if (myIsEnabled)
	{
		if (myGameObject->GetComponent<Animator>())
		{
			myGameObject->GetComponent<Animator>()->UpdateAnimationBuffer();
		}

	}


	//MeshRenderData data;
	//data.type = GraphicsCommandType::RenderMesh;
	//data.mesh = *myMesh;
	//data.matrix = GetGameObject()->GetComponent<Transform>()->GetMatrixScale();
	//data.materials = myMaterials;
	//data.isOutlinedA = myIsOutlinedA;
	//data.isOutlinedB = myIsOutlinedB;
	//data.isOutlinedC = myIsOutlinedC;
	auto& renderer = RENDERER;
	//GraphicsCommandList& commandList = renderer;
	//commandList.Enqueue<GfxCmd_RenderMesh>(myMesh, myGameObject->GetComponent<Transform>()->GetMatrixScale(), &myMaterials);
	renderer.Enqueue<GfxCmd_RenderMesh>(myMesh, myGameObject->GetComponent<Transform>()->GetMatrixScale(), &myMaterials);
}

void MeshComponent::DebugRender()
{
	bool hasBone = false;
	if (GetGameObject()->GetComponent<Animator>())
	{
		hasBone = true;
		myGameObject->GetComponent<Animator>()->UpdateAnimationBuffer();
	}
	//GraphicsCommandList::Get().Enqueue<GfxCmd_DrawDebugLines>(myMesh, myGameObject->GetComponent<Transform>()->GetMatrixScale(), hasBone);
	//DebugObjectData data;
	//data.mesh = *myMesh;
	//data.transform = myGameObject->GetComponent<Transform>()->GetMatrixScale();
	//data.hasBones = hasBone;
	auto& renderer = RENDERER
	renderer.Enqueue<GfxCmd_DrawDebugLines>(myMesh, myGameObject->GetComponent<Transform>()->GetMatrixScale(), hasBone);
	//Needs to call renderer instead
	//GraphicsCommandList::Get().Enqueue<GfxCmd_DrawDebugLines>(myMesh, myGameObject->GetComponent<Transform>()->GetMatrixScale(), hasBone);
}

void MeshComponent::SetMesh(const std::filesystem::path& aMeshName)
{
	myMesh = AssetManager::Get().GetAsset<MeshAsset>(aMeshName)->myMesh;
}

void MeshComponent::SetMesh(const std::shared_ptr<Mesh>& aMesh)
{
	myMesh = aMesh;
}

std::shared_ptr<Mesh> MeshComponent::GetMesh()
{
	return myMesh;
}

void MeshComponent::SetVSShader(std::string_view aShaderName)
{
	myVertexShader = aShaderName;
}

void MeshComponent::SetPSShader(std::string_view aShaderName)
{
	myPixelShader = aShaderName;
}

std::string_view MeshComponent::GetVertexShader() const
{
	if (myVertexShader.empty())
	{
		return "Default_VS";
	}
	return myVertexShader;
}

std::string_view MeshComponent::GetPixelShader() const
{
	if (myPixelShader.empty())
	{
		return "Default_PS";
	}
	return myPixelShader;
}

CU::AABB3Df MeshComponent::GetBoundingBox()
{
	auto transform = myGameObject->GetTransform();
	auto scale = transform->GetScale();
	if (!myMesh)
	{
		return CU::AABB3Df();
	}
	auto box = myMesh->GetBoundingBox();
	CU::AABB3Df resultBox;
	CU::Vector3f min;
	CU::Vector3f max;
	resultBox.SetCenterPoistion(transform->GetPosition());
	for (int i = 0; i < 3; i++)
	{
		auto directionalscale = scale[i];
		min[i] = box.GetMin()[i] * directionalscale;
		max[i] = box.GetMax()[i] * directionalscale;
	}
	resultBox.InitWithMinAndMax(min, max);
	return resultBox;
}
void MeshComponent::SetMaterials(const std::vector<std::filesystem::path>& aMaterialNameList)
{
	if(myMesh->GetElements().size()*4<aMaterialNameList.size())
	{
		std::cout << "We have more material than we support\n";
		std::wstring msg = L"Mesh has More Materials than the engine supports.\nMaterial List is\n";
		for(auto mat : aMaterialNameList)
		{
			msg.append(mat);
			msg.append(L"\n");
		}
		MessageBox(0,msg.c_str(), L"FATAL ERROR!!", MB_ICONERROR | MB_OK);
	}
	ResetMaterialListToDefault();
	size_t endPos = 0;
	for (size_t i = 0; i< aMaterialNameList.size(); i++, endPos++)
	{
		myMaterials.at(i) = (AssetManager::Get().GetAsset<MaterialAsset>(aMaterialNameList[i])->myMaterial);
	}
	if(endPos+1<myMesh->GetElements().size()*4)
	{
		for (size_t i = endPos; i< myMesh->GetElements().size()*4; i++)
		{
			myMaterials.at(i) = (AssetManager::Get().GetAsset<MaterialAsset>(aMaterialNameList[0])->myMaterial);
		}
	}
}

void MeshComponent::SetMaterials(std::filesystem::path aMaterialName)
{
	ResetMaterialListToDefault();
	myMaterials.at(0) = AssetManager::Get().GetAsset<MaterialAsset>(aMaterialName)->myMaterial;
}

void MeshComponent::SetMaterials(std::vector<std::shared_ptr<Material>> aMaterialVector)
{
	myMaterials = aMaterialVector;
}

std::vector<std::shared_ptr<Material>> MeshComponent::GetMaterials()
{
	return myMaterials;
}

int MeshComponent::GetJoint(const std::string& aJointName) const
{
	if (myMesh->GetSkeleton().JointNameToIndex.contains(aJointName))
	{
		return myMesh->GetSkeleton().JointNameToIndex.at(aJointName);
	}
	return -1;
}

void MeshComponent::SetElite()
{
	myIsOutlinedA = false;
	myIsOutlinedB = true;
}

void MeshComponent::SetEnemy()
{
	if (myIsOutlinedB)
	{
		return;
	}
	myIsOutlinedA = true;
}

bool MeshComponent::IsElite()
{
	return myIsOutlinedB;
}

void MeshComponent::ResetMaterialListToDefault()
{
	myMaterials.clear();
	myMaterials.resize(myMesh->GetElements().size()*4);
	for(auto& mat : myMaterials)
	{
		mat = AssetManager::Get().GetAsset<MaterialAsset>("DefaultMaterial")->myMaterial;
	}
}
