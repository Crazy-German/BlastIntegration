#pragma once
#include "ColliderComponent.h"
#include "Collision/SphereCollider.h"
#include "GraphicsEngine/Mesh.h"

#define SPHERE_COLLIDER_COMPONENT 1000

class SphereColliderComponent : public ColliderComponent
{
public:
	SphereColliderComponent() = default;
	~SphereColliderComponent() override = default;
	void Init(const float aRadius, const CommonUtilities::Vector3f& aPosition, const CollisionLayer& aMode = Static)
	{
		myCollider.GetCollider().InitWithCenterAndRadius(aPosition, aRadius);
		myTransformOffset = aPosition.ToVector4One();
		myLayer = aMode;
		Update(0.f);
		myDebugMesh = std::make_shared<Mesh>();
		myDebugMesh->GenerateSphere(aRadius, 100);
	};
	void Awake() override{};
	//void Destroy() override{};
	void Update(const float& /*aDeltaTime*/) { myCollider.SetPosition((myTransformOffset*myGameObject->GetComponent<Transform>()->GetMatrix()).ToVector3());};
	void Put(Letter aLetter) override{aLetter;};
	Collider* GetCollider() override {return &myCollider;};

private:
	SphereCollider myCollider;
};