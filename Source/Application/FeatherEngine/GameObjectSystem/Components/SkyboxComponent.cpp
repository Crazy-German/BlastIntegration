#include "FeatherEngine.pch.h"
#include "SkyboxComponent.h"

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
#include <GraphicsEngine/GfxCmd_RenderSkybox.h>

SkyboxComponent::SkyboxComponent() = default;

SkyboxComponent::~SkyboxComponent()
{
	myMesh = nullptr;
}

void SkyboxComponent::Awake()
{

}

void SkyboxComponent::Destroy()
{
	myMesh.reset();
	myIsMarkedForDestroy = true;
}

void SkyboxComponent::Update(const float& aDeltaTime)
{
	UNREFERENCED_PARAMETER(aDeltaTime);
}

void SkyboxComponent::Render()
{
	if (myIsEnabled)
	{

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
	renderer.Enqueue<GfxCmd_RenderSkybox>(myMesh, myGameObject->GetComponent<Transform>()->GetMatrixScale());
	//data.isOutlinedC = damage;
	//Needs to call renderer instead
	//GraphicsCommandList::Get().Enqueue<GfxCmd_RenderMesh>(myMesh, myGameObject->GetComponent<Transform>()->GetMatrixScale(), &myMaterials, myIsEnemy, myIsElite, damage);
}

void SkyboxComponent::SetMesh(const std::filesystem::path& aMeshName)
{
	myMesh = AssetManager::Get().GetAsset<MeshAsset>(aMeshName)->myMesh;
}

void SkyboxComponent::SetMesh(const std::shared_ptr<Mesh>& aMesh)
{
	myMesh = aMesh;
}

std::shared_ptr<Mesh> SkyboxComponent::GetMesh()
{
	return myMesh;
}


CU::AABB3Df SkyboxComponent::GetBoundingBox()
{
	auto transform = myGameObject->GetTransform();
	auto scale = transform->GetScale();
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
