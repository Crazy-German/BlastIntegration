#pragma once
#include "AssetManagement/AssetTypes/ColliderAsset.h"
#include "Collision/ColliderComponentBase.h"
#include "Physcics/RigidBody.h"

struct ColliderAsset;

namespace physx
{
	struct PxContactPair;
	struct PxContactPairHeader;
	//struct PxController;
}

using namespace  physx;

#define RIGID_BODY_COMPONENT 5000

class RigidBodyComponent : public Squish::RigidBody, public ColliderComponentBase
{
public:
	RigidBodyComponent() = default;
	~RigidBodyComponent() override;
	void Destroy() override;
	void Awake() override;
	void Update(const float& aDeltaTime) override;
	void CreateFromAsset(std::shared_ptr<ColliderAsset> aAsset);

	const Squish::ActorUserData* GetUserData() const override;
private:
	void InternalInit(std::shared_ptr<ColliderData> aData);
	void InternalAddShape(std::shared_ptr<ColliderData> aData);
};
//
//class CharacterController : public Component
//{
//public:
//	CharacterController();
//	~CharacterController() override;
//	//void Update(const float& aDeltaTime) override;
//
//	void Move(const float& aDeltaTime, CommonUtilities::Vector3f aDirection);
//private:
//	PxController* myController;
//	Squish::RigidBody* myRigidBody;
//};