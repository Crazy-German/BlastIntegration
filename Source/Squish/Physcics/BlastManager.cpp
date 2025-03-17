#include "BlastManager.h"

#include "BlastFrameWork.h"
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
#include "toolkit/NvBlastTkFramework.h"
#include "toolkit/NvBlastTkGroup.h"
#include "toolkit/NvBlastTkJoint.h"
#include "PxSimulationEventCallback.h"

BlastManager::BlastManager()
{
	myPhysicsRef = nullptr;
	jointIndex = 0;
	Nv::Blast::TkGroupDesc desc;
	desc.workerCount = 1;
	myGroup = BlastFrameWork::GetInstance().GetBlastFrameWork()->createGroup(desc);
}

BlastManager::~BlastManager()
{
	myGroup->release();
}

void BlastManager::Init()
{
	myPhysicsRef = Squish::PhysicsEngine::Get()->GetPhysics();
}

BlastAsset* BlastManager::CreateNewAsset(const std::vector<CommonUtilities::Vector3f>& aPosData,
	const std::vector<CommonUtilities::Vector3f>& aNormData, const std::vector<CommonUtilities::Vector2f>& aUvData,
	const std::vector<uint32_t>& aIndicies, unsigned aNrOfPieces)
{
	myAssets.emplace_back();
	myAssets.back().CreateAsset(aPosData, aNormData, aUvData, aIndicies, aNrOfPieces);
	//myGroup->addActor(*myAssets.back().GetActor());
	//CreateActorInternal(myAssets.back().GetActor());
	return &myAssets.back();
}


void BlastManager::Update()
{
	
	/*for(auto& asset : myAssets)
	{
		asset.Hit();
	}*/
	/*uint32_t jobCount = myGroup->startProcess();
	Nv::Blast::TkGroupWorker* worker = myGroup->acquireWorker();
    for(uint32_t i = 0; i<jobCount; i++)
    {
        worker->process(i);
    }

    myGroup->returnWorker(worker);
    myGroup->endProcess();*/
	/*myGroup->process();
	myGroup->release();*/
}

BlastManager* BlastManager::Get()
{
	static BlastManager instance;
	return &instance;
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

void BlastManager::OnContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs,
	uint32_t numPairs)
{
	for (uint32_t i = 0; i < numPairs; ++i)
    {
        const physx::PxContactPair& contactPair = pairs[i];
       physx::PxActor* actor0 = contactPair.shapes[0]->getActor();
        physx::PxActor* actor1 = contactPair.shapes[1]->getActor();
	/*	if(actor0 == nullptr || actor1 == nullptr)
		{
			printf("Actor was nullptr\n");
			return;
		}*/
	
        for (uint32_t j = 0; j < contactPair.contactCount; ++j)
        {
	        physx::PxContactPairPoint* point = new physx::PxContactPairPoint[contactPair.contactCount];
            contactPair.extractContacts(point, contactPair.contactCount);
			uint32_t* index1 = static_cast<uint32_t*>(contactPair.shapes[0]->userData);
			uint32_t* index2 = static_cast<uint32_t*>(contactPair.shapes[1]->userData);
			float dmg = (point->impulse/Squish::PhysicsEngine::Get()->GetScene()->GetTimeStep()).magnitude();
			if(index1 != nullptr && *index1<myAssets.size())
			{
				myAssets.at(*index1).Hit(physx::PxVec3(0,0,0),dmg*0.1f, 0.1f, dmg*0.01f);
			}
        	if(index2 != nullptr && *index2<myAssets.size())
			{
				myAssets.at(*index2).Hit(point->position,(point->impulse/Squish::PhysicsEngine::Get()->GetScene()->GetTimeStep()).magnitude(), 0.1f, 100.f);
			}
        }
    }
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