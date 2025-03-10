#include "BlastManager.h"

#include "PhysicsEngine.h"
#include "PhysXScene.h"
#include "PxFixedJoint.h"
#include "PxJoint.h"
#include "PxPhysics.h"
#include "PxRevoluteJoint.h"
#include "PxRigidBodyExt.h"
#include "PxRigidDynamic.h"
#include "toolkit/NvBlastTkAsset.h"
#include "toolkit/NvBlastTkFamily.h"
#include "toolkit/NvBlastTkJoint.h"

BlastManager::BlastManager()
{
	myPhysicsRef = nullptr;
	jointIndex = 0;
}

void BlastManager::Init()
{
	myPhysicsRef = Squish::PhysicsEngine::Get()->GetPhysics();
}

BlastAsset* BlastManager::CreateNewActor(const std::vector<CommonUtilities::Vector3f>& aPosData,
	const std::vector<CommonUtilities::Vector3f>& aNormData, const std::vector<CommonUtilities::Vector2f>& aUvData,
	const std::vector<uint32_t>& aIndicies, unsigned aNrOfPieces)
{
	BlastAsset newAsset;
	newAsset.CreateAsset(aPosData, aNormData, aUvData, aIndicies, aNrOfPieces);
	myAssets.push_back(newAsset);
	//CreateActorInternal(myAssets.back().GetActor());
	return &myAssets.back();
}


void BlastManager::Update()
{
	/*for(auto& [blastId, physxFamily] : myActors)
	{
		for(auto& [index, actor] : physxFamily.myActors)
		{
		}
	}
	*/	
	
}

void BlastManager::CreateActorInternal(Nv::Blast::TkActor* aActor)
{
	uint32_t chunkCount = aActor->getAsset()->getChunkCount();
	physx::PxRigidActor* newActor = myPhysicsRef->createRigidDynamic(physx::PxTransform());
	//sPhysxFamily& family = myActors.at(aActor->getFamily().getID());
	/*family.myActors.emplace(aActor->getIndex(), newActor);
	const NvBlastChunk* chunks = aActor->getAsset()->getChunks();
	for(uint32_t i = 0; i<chunkCount; i++)
	{
		chunks[i].centroid;
	}*/
}

void BlastManager::DeleteActorInternal(NvBlastID aFamily, uint32_t aIndex)
{
	aFamily;aIndex;
	/*Squish::PhysicsEngine::Get()->GetScene()->RemoveActor(myActors.at(aFamily).myActors.at(aIndex));
	myActors.at(aFamily).myActors.at(aIndex)->release();
	myActors.at(aFamily).myActors.erase(aIndex);*/
	
}

void BlastManager::NewJoint(Nv::Blast::TkJoint* aJoint)
{
	aJoint;
	aJoint->userData = new uint32_t(jointIndex++);
	//Create a physx joint
	//myJoints.emplace(*static_cast<uint32_t*>(aJoint->userData),PxFixedJointCreate(*myPhysicsRef, 
	//	myActors.at(aJoint->getData().actors[0]->getFamily().getID()).myActors.at(aJoint->getData().actors[0]->getIndex()), //first actor
	//	physx::PxTransform(aJoint->getData().attachPositions[0].x, aJoint->getData().attachPositions[0].y, aJoint->getData().attachPositions[0].z), //Relative position
	//	myActors.at(aJoint->getData().actors[0]->getFamily().getID()).myActors.at(aJoint->getData().actors[0]->getIndex()), //second actor
	//	physx::PxTransform(aJoint->getData().attachPositions[0].x, aJoint->getData().attachPositions[0].y, aJoint->getData().attachPositions[0].z)));	
}

void BlastManager::UpdateJoint(Nv::Blast::TkJoint* aJoint)
{
	aJoint;
	//Updates the physx joints actors
	
	//myJoints.at(*static_cast<uint32_t*>(aJoint->userData))->setActors(myActors.at(aJoint->getData().actors[0]->getFamily().getID()).myActors.at(aJoint->getData().actors[0]->getIndex())->is<physx::PxRigidActor>(), myActors.at(aJoint->getData().actors[1]->getFamily().getID()).myActors.at(aJoint->getData().actors[0]->getIndex())->is<physx::PxRigidActor>());
}

void BlastManager::DeleteJoint(Nv::Blast::TkJoint* aJoint)
{
	aJoint;
	/*delete static_cast<uint32_t*>(aJoint->userData);
	myJoints.at(*static_cast<uint32_t*>(aJoint->userData))->release();
	*/
}