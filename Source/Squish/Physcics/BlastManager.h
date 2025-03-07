#pragma once
#include <unordered_map>

#include "BlastAsset.h"
#include "toolkit/NvBlastTkEvent.h"

namespace physx
{
	class PxRigidActor;
	class PxFixedJoint;
	class PxActor;
}

namespace physx
{
	class PxPhysics;
}
struct PhysxFamily
{
	std::unordered_map<uint32_t,physx::PxRigidActor*> myActors;
};
class BlastManager : public Nv::Blast::TkEventListener
{
public:
	BlastManager();
	void Init();
	void CreateNewActor();
	void Update();
	void receive(const Nv::Blast::TkEvent* events, uint32_t eventCount) override;
private:
	void CreateActorInternal(Nv::Blast::TkActor* aActor);
	void DeleteActorInternal(NvBlastID aFamily, uint32_t aIndex);
	void NewJoint(Nv::Blast::TkJoint* aJoint);
	void UpdateJoint(Nv::Blast::TkJoint* aJoint);
	void DeleteJoint(Nv::Blast::TkJoint* aJoint);
	std::vector<BlastAsset> myAssets;
	physx::PxPhysics* myPhysicsRef;
	std::unordered_map<NvBlastID, PhysxFamily> myActors; //TKFamily id of TKActor, mapped to a physx family representing the different physx actors
	std::unordered_map<uint32_t, physx::PxFixedJoint*> myJoints;
	uint32_t jointIndex;
};

