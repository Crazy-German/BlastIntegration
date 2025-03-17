#pragma once
#include <queue>

#include "Raycast.h"
#include "CommonUtilities/InputMapper.h"
#include "PxQueryFiltering.h"
namespace Squish
{
	class RigidBody;
}

namespace physx
{
	class PxScene;
	class PxBase;
	class PxSimulationEventCallback;
}

namespace Squish
{
	
	class PhysicsEngine;

	class PhysXScene
	{
			friend class PhysicsEngine;
	public:
		PhysXScene(physx::PxScene* aScene);
		~PhysXScene();
		void Update(float aDeltaTime);
		void AddActor(physx::PxBase* aActor);
		void RemoveActor(physx::PxBase* aActor);
		void AddActor(RigidBody* aActor);
		void SetCallback(physx::PxSimulationEventCallback* aCallback);
		void SetGravity(CU::Vector3f aGravity);
		auto GetGravity() const -> CU::Vector3f;
		void ClearScene();
		void SetTimeStep(const float& aTimeStep);
		float GetTimeStep() const;
		bool Raycast(const CommonUtilities::Vector3f& aOrigin, CommonUtilities::Vector3f aDirection, float aMaxDistance, RayCastBuffer& aRayCastBuffer, const physx::PxFilterData& aFilter = physx::PxFilterData(1,0,0,0), physx::PxQueryFlags aFlags = physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::eDYNAMIC) const;
	private:
		struct ActorQueueCommand
		{
			physx::PxActor* myActor;
			uint32_t myType;
		};
		std::queue<ActorQueueCommand> myActorActionQueue;
		physx::PxScene* myScene;
		float myTimeStep;
		float myAccumulator;
		bool mySimulationIsRunning;
	};

}