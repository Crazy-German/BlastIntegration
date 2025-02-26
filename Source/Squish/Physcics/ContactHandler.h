#pragma once
#include <cstdio>

#include "Physx/include/PxContactModifyCallback.h"
#include "Physx/include/PxSimulationEventCallback.h"
#include "Physx/include/geomutils/PxContactPoint.h"

class ContactHandler : public physx::PxSimulationEventCallback, public physx::PxContactModifyCallback
{
public:
	void onConstraintBreak(physx::PxConstraintInfo* /*constraints*/, physx::PxU32 /*count*/) override{};
	void onWake(physx::PxActor** /*actors*/, physx::PxU32 /*count*/) override{};
	void onSleep(physx::PxActor** /*actors*/, physx::PxU32 /*count*/) override{};
	virtual void onContact(const physx::PxContactPairHeader& /*pairHeader*/, const physx::PxContactPair* /*pairs*/, uint32_t /*numPairs*/) override
	{
		printf("Inbase why");
	}
	void onTrigger(physx::PxTriggerPair* /*pairs*/, physx::PxU32 /*count*/) override{};
	void onAdvance(const physx::PxRigidBody* const* /*bodyBuffer*/, const physx::PxTransform* /*poseBuffer*/,
		const physx::PxU32 /*count*/) override{};
	virtual void onContactModify(PxContactModifyPair* const /*pairs*/, PxU32 /*count*/) override
	{
		printf("mod");
	}
	~ContactHandler() override = default;
	ContactHandler() = default;
};
