#pragma once
#include "ColliderComponent.h"
#include "Collision/CapsuleCollider.h"
#include "Collision/Collider.h"
#include "GraphicsEngine/Mesh.h"
#include "GraphicsEngine/RenderData.h"
#include "MainSingleton.h"
#include <GraphicsEngine/GfxCmd_DrawDebugLines.h>
#define CAPSULE_COLLLIDER_COMPONENT 350

class CapsuleColliderComponent : public ColliderComponent
{
public:
	CapsuleColliderComponent() = default;
	~CapsuleColliderComponent() override = default;

	void Awake() override{};
	//void Destroy() override{};
	void Init(const Vec3& aPosition, const float& aRadius, const float& aHeight, const CommonUtilities::Matrix4x4f& aMatrix = {}, const CollisionLayer& aMode = Static)
	{
		myCollider.GetCollider().Init(aPosition, aRadius, aHeight, aMatrix);
		myTransformOffset = aPosition.ToVector4One();
		myLayer = aMode;
		Update(0.f);
		myDebugMesh = std::make_shared<Mesh>();
		myDebugMesh->GenerateCapsule(myCollider.GetCollider().GetRadius(), myCollider.GetCollider().GetHeight());
	}
	void Update(const float& /*aDeltaTime*/) override
	{
		myCollider.SetPosition((myTransformOffset*myGameObject->GetComponent<Transform>()->GetMatrix()).ToVector3());
		myCollider.GetCollider().SetRotationMatrix(myGameObject->GetComponent<Transform>()->GetMatrix());
	};
	void Put(Letter /*aLetter*/) override{};
	Collider* GetCollider() override
	{
		return &myCollider;
	}

	void DebugRender() override
	{
		CommonUtilities::Vector3f pos = myCollider.GetCollider().GetPosition();
		CommonUtilities::Matrix4x4f matrix = myCollider.GetCollider().GetRotationMatrix();
		matrix.SetTranslation(pos);
		//GraphicsCommandList::Get().Enqueue<GfxCmd_DrawDebugLines>(myDebugMesh,matrix, false, DebugColor::Orange, "Collider");
		//DebugObjectData data;
		//data.type = GraphicsCommandType::DrawDebugLines;
		//data.mesh = *myDebugMesh;
		//data.transform = matrix;
		//data.hasBones = false;
		//data.color = DebugColor::Orange;
		//data.debugName = "Collider";
		MainSingleton::Get().GetRenderer().Enqueue<GfxCmd_DrawDebugLines>(myDebugMesh, matrix, false, DebugColor::Orange, "Collider");
		//Needs to call renderer instead
		//GraphicsCommandList::Get().Enqueue<GfxCmd_DrawDebugLines>(myDebugMesh,matrix, false, DebugColor::Orange, "Collider");
	};

private:
	CapsuleCollider myCollider;
};

