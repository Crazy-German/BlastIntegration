#pragma once
#include "RigidBody.h"
#include "CommonUtilities/Vector3.hpp"
#include "gpu/PxPhysicsGpu.h"


namespace Squish
{
	class PhysicsController : public RigidBody
	{
	public:
		PhysicsController();
		~PhysicsController();
		bool Init(RigidBodyType aType, TransformBase* aTransform, CommonUtilities::Vector3f aMaterial, float aRadius, float aHalfHeight);
		void Move(const CommonUtilities::Vector3f direction, float aDeltaTime);
	private:
	};

}
