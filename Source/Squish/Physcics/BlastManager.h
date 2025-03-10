#pragma once
#include <unordered_map>

#include "BlastAsset.h"

namespace physx
{
	class PxScene;
	class PxRigidActor;
	class PxFixedJoint;
	class PxActor;
	class PxPhysics;
}

struct PhysxFamily
{
	std::unordered_map<uint32_t,physx::PxRigidActor*> myActors;
};
class BlastManager
{
public:
	BlastManager();
	void Init();
	BlastAsset* CreateNewActor(const std::vector<CommonUtilities::Vector3f>& aPosData, const std::vector<CommonUtilities::Vector3f>& aNormData, const
                     std::vector<CommonUtilities::Vector2f>& aUvData, const std::vector<uint32_t>& aIndicies, unsigned aNrOfPieces = 4);
	void Update();
	
private:
	void CreateActorInternal(Nv::Blast::TkActor* aActor);
	void DeleteActorInternal(NvBlastID aFamily, uint32_t aIndex);
	void NewJoint(Nv::Blast::TkJoint* aJoint);
	void UpdateJoint(Nv::Blast::TkJoint* aJoint);
	void DeleteJoint(Nv::Blast::TkJoint* aJoint);
	std::vector<BlastAsset> myAssets;
	physx::PxPhysics* myPhysicsRef;
	physx::PxScene* mySceneRef;
	//std::unordered_map<NvBlastID, PhysxFamily> myActors; //TKFamily id of TKActor, mapped to a physx family representing the different physx actors
	//std::unordered_map<uint32_t, physx::PxFixedJoint*> myJoints;
	uint32_t jointIndex;
};

