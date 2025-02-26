#pragma once
#include "ColliderComponent.h"
#include "Collision/OBBCollider.h"
#include "GraphicsEngine/DebugLineObject.h"

#include "MainSingleton.h"
#include "GraphicsEngine/GfxCmd_DrawDebugLines.h"

#define OOB_COLLIDER_COMPONENT 50

class OBBColliderComponent : public ColliderComponent
{
public:
	OBBColliderComponent() = default;
	~OBBColliderComponent() override = default;

	void Awake() override{};
	//void Destroy() override{};
	void Init(const Vec3& aPosition, const Vec3& aSize, const CommonUtilities::Matrix4x4f& aRotationMatrix = {}, const CollisionLayer& aMode = Static)
	{
		myCollider.GetCollider().Init(aPosition, aSize, aRotationMatrix);
		myTransformOffset = aPosition.ToVector4One();
		myLayer = aMode;
		Update(0.f);
		//myDebugLines = std::make_shared<DebugLineObject>();
		myDebugMesh = std::make_shared<Mesh>();
		myDebugMesh->GenerateCube(aSize);
	}
	void Update(const float& aDeltaTime) override
	{
		aDeltaTime;
		myCollider.SetPosition((myTransformOffset*myGameObject->GetComponent<Transform>()->GetMatrix()).ToVector3());
		myCollider.GetCollider().SetRotationMatrix(myGameObject->GetComponent<Transform>()->GetMatrix());
	};
	void Put(Letter aLetter) override{aLetter;};
	Collider* GetCollider() override
	{
		return &myCollider;
	}

	void DebugRender() override
	{
		CommonUtilities::Vector3f pos = myCollider.GetCollider().GetPosition();
		CommonUtilities::Matrix4x4f matrix = myCollider.GetCollider().GetRotationMatrix();
		matrix.SetTranslation(pos);
		MainSingleton::Get().GetRenderer().Enqueue<GfxCmd_DrawDebugLines>(myDebugMesh, matrix, false, DebugColor::Orange, "Collider");;

	};
	
private:
	OBBCollider myCollider;
};