#include "PhysicsController.h"

#include "PxForceMode.h"
#include "PxRigidActor.h"
#include "PxRigidDynamic.h"
#include "characterkinematic/PxCapsuleController.h"
#include "characterkinematic/PxController.h"
#include "extensions/PxDefaultSimulationFilterShader.h"
#include "foundation/PxAllocator.h"

using namespace physx;
using namespace Squish;

PhysicsController::PhysicsController()
{
}

PhysicsController::~PhysicsController()
{
}

bool PhysicsController::Init(RigidBodyType aType, TransformBase* aTransform, CommonUtilities::Vector3f aMaterial,
	float aRadius, float aHalfHeight)
{
	bool result = RigidBody::Initialize(aType, ShapeType::Capsule, aTransform, aMaterial, aRadius, aHalfHeight);
	if(result)
	{
		myRigidBody->is<PxRigidDynamic>()->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);
		myRigidBody->is<PxRigidDynamic>()->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
	}
	return result;
}

void PhysicsController::Move(const CommonUtilities::Vector3f direction, float aDeltaTime)
{
	AddForce(direction*aDeltaTime);
}



