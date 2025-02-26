#include "CharacterController.h"

#include "PhysicsEngine.h"
#include "PhysXScene.h"
#include "PxPhysics.h"
#include "PxRigidDynamic.h"
#include "Util.h"
#include "characterkinematic/PxBoxController.h"
#include "characterkinematic/PxCapsuleController.h"
#include "characterkinematic/PxController.h"
using namespace physx;
using namespace Squish;

CharacterController::CharacterController()
{
}

CharacterController::~CharacterController()
{
	if(myContoller != nullptr)
	{
		PhysicsEngine::Get()->RemoveController(myContoller);
	}
}

void CharacterController::Init(TransformBase* aTransform, const float& aRadiusWidth, const float& aHeight, const float& aDepth, ControllerType aControllerType, const float& aContactOffset,
                               const float & aStepOffset, const float& aSlopeLimit)
{
	myUserData.myTransform = aTransform;
	myUserData.aID = PhysXId++;
	myFilter.mFilterFlags = PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC;
	myFilter.mFilterData = new PxFilterData();
	auto vect = ToPhysxVect(aTransform->GetPosition());
	switch (aControllerType)
	{
	case CapsuleController:
		
		{
			myControllerDesc = new PxCapsuleControllerDesc();
			PxCapsuleControllerDesc* desc = static_cast<PxCapsuleControllerDesc*>(myControllerDesc);
			desc->position = PxExtendedVec3(vect.x, vect.y, vect.z);
			desc->radius = aRadiusWidth;
			desc->height = aHeight;
			desc->stepOffset = aStepOffset;
			desc->contactOffset = aContactOffset;
			desc->material = PhysicsEngine::Get()->GetPhysics()->createMaterial(1,1,0);
			desc->climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
			desc->nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
			desc->upDirection = {0,1,0};
			desc->slopeLimit = cosf(PxDegToRad(aSlopeLimit));
			desc->behaviorCallback = this;
			desc->reportCallback = this;
			break;
		}
	case BoxController:
		{
			myControllerDesc = new PxBoxControllerDesc();
			PxBoxControllerDesc* boxdesc = static_cast<PxBoxControllerDesc*>(myControllerDesc);
			boxdesc->position = PxExtendedVec3(vect.x, vect.y, vect.z);
			boxdesc->halfSideExtent = aRadiusWidth;
			boxdesc->halfHeight = aHeight;
			boxdesc->halfForwardExtent = aDepth;
			boxdesc->material = PhysicsEngine::Get()->GetPhysics()->createMaterial(1,1,0);
			boxdesc->nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
			boxdesc->upDirection = {0,1,0};
			boxdesc->slopeLimit = cosf(PxDegToRad(aSlopeLimit));
			break;
		}
	}
	if(!PhysicsEngine::Get()->CreateController(&myContoller,myControllerDesc))
	{
		printf("Failed to create controller\n");
	}

	myContoller->getActor()->userData = &myUserData;
	if(aControllerType == CapsuleController)// we need to rotate the shape wish is bs
	{
		PxU32 count = myContoller->getActor()->getNbShapes();
		PxShape** shapes = new PxShape*[count];
		myContoller->getActor()->getShapes(shapes, count);
		for(PxU32 index = 0; index <count; index++)
		{
			if(shapes[index]->getGeometry().getType() == PxGeometryType::eCAPSULE)
			{
				PxQuat rotation = PxQuat(PxPi / 2.0f, PxVec3(0, 0, 1)); // 90 degrees rotation around the Z-axis
				shapes[index]->setLocalPose(PxTransform(PxVec3(0), rotation));
			}
		}
	}
	//myContoller->getActor()->is<PxRigidActor>()->setGlobalPose(ToPhysXTransform(*aTransform));
	/*if(myContoller->getActor()->is<PxRigidDynamic>() != nullptr)
	{
		
	myContoller->getActor()->is<PxRigidDynamic>()->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
	myContoller->getActor()->is<PxRigidDynamic>()->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);
	}*/
}

void CharacterController::Update(float aDeltaTime)
{
	auto pos = static_cast<ActorUserData*>(myContoller->getActor()->userData)->myTransform->GetPosition();
	myContoller->setPosition(ToExtendedVec3(pos));
	
	myCollisionFlags = myContoller->move(ToPhysxVect(myForce), 0.05f, aDeltaTime, myFilter);
	myForce = {};
	static_cast<ActorUserData*>(myContoller->getActor()->userData)->myTransform->SetPosition(ToVector3f(myContoller->getPosition()));
}

void CharacterController::AddForce(const CommonUtilities::Vector3f& aForce)
{
	myForce+=aForce;
}

void CharacterController::onShapeHit(const physx::PxControllerShapeHit& hit)
{
	//printf("ShapeHit\n");
	auto flags = hit.shape->getFlags();
	if(flags & PxShapeFlag::eTRIGGER_SHAPE)
	{
		//printf("Hit trigger\n");
	}
	if(flags & PxShapeFlag::eVISUALIZATION)
	{
		//printf("Draw\n");
	}
	if(flags & PxShapeFlag::eSIMULATION_SHAPE)
	{
		//printf("SimShape\n");
	}
	if(flags & PxShapeFlag::eSCENE_QUERY_SHAPE)
	{
		
	}
}

void CharacterController::onControllerHit(const physx::PxControllersHit& /*hit*/)
{
}

void CharacterController::onObstacleHit(const physx::PxControllerObstacleHit& /*hit*/)
{
}

physx::PxControllerBehaviorFlags CharacterController::getBehaviorFlags(const physx::PxShape& /*shape*/,
	const physx::PxActor& /*actor*/)
{
	return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;
}

physx::PxControllerBehaviorFlags CharacterController::getBehaviorFlags(const physx::PxController& /*controller*/)
{
	return PxControllerBehaviorFlag::eCCT_SLIDE;
}

physx::PxControllerBehaviorFlags CharacterController::getBehaviorFlags(const physx::PxObstacle& /*obstacle*/)
{
	return PxControllerBehaviorFlag::eCCT_SLIDE;
}

auto CharacterController::GetCollisionFlags() const -> physx::PxControllerCollisionFlags
{
	return myCollisionFlags;
}

auto CharacterController::GetVelocity() const -> CU::Vector3f
{
	return myForce;
}

void CharacterController::SetFilterLayer(physx::PxFilterData* aFilterData)
{
	delete myFilter.mFilterData;
	myFilter.mFilterData = new PxFilterData(*aFilterData);
}

const Squish::ActorUserData* CharacterController::GetUserData() const
{
	return &myUserData;
}

CommonUtilities::Vector3f CharacterController::GetFotPosition() const
{
	return ToVector3f(myContoller->getFootPosition());
}
