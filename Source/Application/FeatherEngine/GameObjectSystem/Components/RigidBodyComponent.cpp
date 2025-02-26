#include "FeatherEngine.pch.h"
#include "RigidBodyComponent.h"

#include "AssetManagement/AssetTypes/ColliderAsset.h"
//#include "characterkinematic/PxCapsuleController.h"
//#include "characterkinematic/PxController.h"
#include "Physcics/PhysicsEngine.h"
#include "Physcics/PhysXScene.h"

RigidBodyComponent::~RigidBodyComponent()
{
}

void RigidBodyComponent::Destroy()
{
	Squish::PhysicsEngine::Get()->GetScene()->RemoveActor(myRigidBody);
	myIsMarkedForDestroy = true;
	myIsEnabled = false;
}

void RigidBodyComponent::Awake()
{
	Component::Awake();
}

void RigidBodyComponent::Update(const float& /*aDeltaTime*/)
{
	if(myRigidBody == nullptr)
	{
		//printf("WHY IN ALL HELLS DOES THIS HAVE A UNINITIALIZED RIGIDBODY");
		return;
	}
	myGameObject->GetTransform()->SetPosition(Squish::ToVector3f(myRigidBody->getGlobalPose().p));
	myGameObject->GetTransform()->SetRotation(Squish::ToQuaternion(myRigidBody->getGlobalPose().q));
	//UpdateTransform();
}
void RigidBodyComponent::InternalInit(std::shared_ptr<ColliderData> aData)
{
	switch (aData->myShapeType)
	{
	case Squish::Sphere:
		Initialize(Squish::Dynamic, aData->myShapeType, myGameObject->GetTransform(), {1,1,0}, std::static_pointer_cast<SphereColliderData>(aData)->myRadius, TransformBase(aData->aOffset, {0,1,0,0}));
		break;
	case Squish::Box:
		Initialize(Squish::Static, aData->myShapeType, myGameObject->GetTransform(), {1,1,0}, std::static_pointer_cast<BoxColliderData>(aData)->myHalfWidths, TransformBase(aData->aOffset, {0,1,0,0}));
		break;
	case Squish::Capsule:
		
		Initialize(Squish::Static, aData->myShapeType, myGameObject->GetTransform(), {1,1,0}, std::static_pointer_cast<CapsuleColliderData>(aData)->myRadius, std::static_pointer_cast<CapsuleColliderData>(aData)->myHalfHeight, TransformBase(aData->aOffset, {0,1,0,0}));
		break;
	case Squish::Mesh:
		Initialize(Squish::Static, aData->myShapeType, myGameObject->GetTransform(), {1,1,0}, 
					&std::static_pointer_cast<MeshColliderData>(aData)->myVerticies, &std::static_pointer_cast<MeshColliderData>(aData)->myIndiciess, 
		static_cast<uint32_t>(std::static_pointer_cast<MeshColliderData>(aData)->myVerticies.size()),static_cast<uint32_t>(std::static_pointer_cast<MeshColliderData>(aData)->myIndiciess.size()/3), TransformBase(aData->aOffset/*, {0,1,0,0}*/));
		break;
	case Squish::Convex:
		Initialize(Squish::Static, aData->myShapeType, myGameObject->GetTransform(), {1,1,0},
					&std::static_pointer_cast<ConvexColliderData>(aData)->myVerticies, nullptr, 32, 64, TransformBase(aData->aOffset, {0,1,0,0}));
		break;
	}
}
void RigidBodyComponent::InternalAddShape(std::shared_ptr<ColliderData> aData)
{
	
	switch (aData->myShapeType)
	{
	case Squish::Sphere:
		AddShape(Squish::Static, std::static_pointer_cast<SphereColliderData>(aData)->myShapeType, TransformBase(aData->aOffset, {0,1,0,0}), {1,1,0}, 
		         std::static_pointer_cast<SphereColliderData>(aData)->myRadius);
		break;
	case Squish::Box:
		AddShape(Squish::Static, std::static_pointer_cast<BoxColliderData>(aData)->myShapeType, TransformBase(aData->aOffset, {0,1,0,0}), {1,1,0}, 
		         std::static_pointer_cast<BoxColliderData>(aData)->myHalfWidths);
		break;
	case Squish::Capsule:
		AddShape(Squish::Static, aData->myShapeType, TransformBase(aData->aOffset, {0,1,0,0}), {1,1,0}, 
		         std::static_pointer_cast<CapsuleColliderData>(aData)->myRadius, std::static_pointer_cast<CapsuleColliderData>(aData)->myHalfHeight);
		break;
	case Squish::Mesh:
		AddShape(Squish::Static, aData->myShapeType, TransformBase(aData->aOffset /*, {0,1,0,0}*/), {1,1,0}, 
		         &std::static_pointer_cast<MeshColliderData>(aData)->myVerticies, &std::static_pointer_cast<MeshColliderData>(aData)->myIndiciess,
		         static_cast<uint32_t>(std::static_pointer_cast<MeshColliderData>(aData)->myVerticies.size()), static_cast<uint32_t>(std::static_pointer_cast<MeshColliderData>(aData)->myIndiciess.size())/3);
		break;
	case Squish::Convex:
		AddShape(Squish::Static, aData->myShapeType, TransformBase(aData->aOffset, {0,1,0,0}), {1,1,0}, 
		         &std::static_pointer_cast<ConvexColliderData>(aData)->myVerticies, nullptr, 32, 64);
		break;
	}
}
//
//CharacterController::CharacterController()
//{
//	
//	PxCapsuleControllerDesc desc;
//	desc.radius = 50;
//	desc.height = 50;
//	desc.upDirection = {0,1,0};
//	desc.stepOffset = 10;
//	desc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
//	desc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
//	desc.contactOffset = 1;
//	desc.slopeLimit = 60;
//	Squish::PhysicsEngine::Get()->CreateController(myController, &desc);
//	
//}
//
//CharacterController::~CharacterController()
//{
//	myController->release();
//}
//
//void CharacterController::Move(const float& aDeltaTime, CommonUtilities::Vector3f aDirection)
//{
//	
//
//}

void RigidBodyComponent::CreateFromAsset(std::shared_ptr<ColliderAsset> aAsset)
{
	if(aAsset == nullptr)
	{
		printf("Create from asset failure\n");
		return;
	}
	for(int index = 0; index < aAsset->myColliderAssets.size(); index++)
	{
		if(index == 0)
		{
			InternalInit(aAsset->myColliderAssets.at(index));
		}
		else
		{
			InternalAddShape(aAsset->myColliderAssets.at(index));
		}
	}
}

const Squish::ActorUserData* RigidBodyComponent::GetUserData() const
{
	return RigidBody::GetUserData();
}
