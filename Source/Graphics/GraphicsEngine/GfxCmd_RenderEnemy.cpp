//#include "GraphicsEngine.pch.h"
//#include "GfxCmd_RenderEnemy.h"
//
//#include "Material.h"
//#include "ObjectBuffer.h"
//
//
//GfxCmd_RenderEnemy::GfxCmd_RenderEnemy(std::shared_ptr<Mesh> aMesh, CU::Matrix4x4f aTransform, std::vector<std::shared_ptr<Material>> aMaterialList, CU::Vector4f aID, CU::Vector2f aMousePos)
//{
//	myMesh = aMesh;
//	myTransform = aTransform;
//	myMaterialList = aMaterialList;
//	myID = aID;
//	myMousePos = aMousePos;
//}
//
//GfxCmd_RenderEnemy::~GfxCmd_RenderEnemy()
//{
//}
//
//void GfxCmd_RenderEnemy::Execute()
//{
//	ObjectBuffer objectBufferData;
//	objectBufferData.World = myTransform;
//	objectBufferData.WorldInvT = myTransform.GetInverse().GetTranspose();
//	objectBufferData.HasSkinning = !myMesh->GetSkeleton().Joints.empty();
//	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objectBufferData);
//
//	for (auto& element : myMesh->GetElements())
//	{
//		GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::MaterialBuffer,
//			myMaterialList[element.MaterialIndex]->GetMaterialBuffer());
//	}
//
//	GraphicsEngine::Get().RenderMesh(*myMesh, myMaterialList);
//}
//
//void GfxCmd_RenderEnemy::Destroy()
//{
//	myMesh = nullptr;
//	for (auto material : myMaterialList)
//	{
//		material = nullptr;
//	}
//	myMaterialList.clear();
//}
