#include "GraphicsEngine.pch.h"
#include "GfxCmd_RenderMesh.h"

#include <utility>

#include "Material.h"
#include "ObjectBuffer.h"

GfxCmd_RenderMesh::GfxCmd_RenderMesh(std::shared_ptr<Mesh> aMesh, const CU::Matrix4x4f& aTransform,
	const std::vector<std::shared_ptr<Material>>* aMaterialList, bool aIsEnemy, bool aIsElite, float aHitValue)
{
	myMesh = std::move(aMesh);
	myTransform = aTransform;
	myMaterialList = aMaterialList;
	myIsEnemy = aIsEnemy;
	myIsElite = aIsElite;
	myHitValue = aHitValue;
}

GfxCmd_RenderMesh::~GfxCmd_RenderMesh() = default;

void GfxCmd_RenderMesh::Execute()
{
	if (!myMesh)
	{
		return;
	}
	ObjectBuffer objectBufferData;
	objectBufferData.World = myTransform;
	objectBufferData.WorldInvT = myTransform.GetInverse().GetTranspose();
	objectBufferData.MaterialCount = static_cast<unsigned>(myMaterialList->size());

	objectBufferData.IsOutlinedA = static_cast<unsigned>(myIsEnemy);
	objectBufferData.IsOutLinedB = static_cast<unsigned>(myIsElite);
	objectBufferData.HasSkinning = !myMesh->GetSkeleton().Joints.empty();
	objectBufferData.IsHit = myHitValue;
	auto& engine = GraphicsEngine::Get();
	engine.UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objectBufferData);
	for (auto& element : myMesh->GetElements())
	{
		if (myMaterialList->size() > element.MaterialIndex)
		{
			GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::MaterialBuffer,
				myMaterialList->at(element.MaterialIndex)->GetMaterialBuffer());
		}
	}

	engine.RenderMesh(*myMesh, *myMaterialList);

}

void GfxCmd_RenderMesh::Destroy()
{
	myMaterialList = nullptr;
	myMesh = nullptr;
}
