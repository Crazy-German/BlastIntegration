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

void BlastManager::Init()
{
	myPhysicsRef = Squish::PhysicsEngine::Get()->GetPhysics();
}

void BlastManager::CreateNewActor()
{

}

void BlastManager::Update()
{
	for(auto& [blastId, physxFamily] : myActors)
	{
		for(auto& [index, actor] : physxFamily.myActors)
		{
			
		}
	}
}

void BlastManager::receive(const Nv::Blast::TkEvent* events, uint32_t eventCount)
{
	//const Nv::Blast::TkJointUpdateEvent* jointEvent;
	//const Nv::Blast::TkSplitEvent* split;

	//for(uint32_t i = 0; i<eventCount; i++)
	//{
	//	const Nv::Blast::TkEvent& event = events[i];
	//	switch (event.type)
	//	{
	//	case Nv::Blast::TkEvent::Split:
	//		split = event.getPayload<Nv::Blast::TkSplitEvent>();
	//		DeleteActorInternal(split->parentData.family->getID(), split->parentData.index);
	//		for(uint32_t j = 0; j<split->numChildren; j++)
	//		{
	//			CreateActorInternal(split->children[j]);
	//		}
	//		break;
	//	case Nv::Blast::TkEvent::FractureCommand:
	//		
	//		break;
	//	case Nv::Blast::TkEvent::FractureEvent:
	//		break;
	//	case Nv::Blast::TkEvent::JointUpdate:
 //               // Joint events have three subtypes, see which one we have
	//			jointEvent = event.getPayload<Nv::Blast::TkJointUpdateEvent>();  // Joint update event payload
 //               switch (jointEvent->subtype)
 //               {
 //               case Nv::Blast::TkJointUpdateEvent::External:
 //                   NewJoint(jointEvent->joint);   // An internal joint has been "exposed" (now joins two different actors).  Create a physics joint.
 //                   break;
 //               case Nv::Blast::TkJointUpdateEvent::Changed:
	//				
 //                   UpdateJoint(jointEvent->joint);   // A joint's actors have changed, so we need to update its corresponding physics joint.
 //                   break;
 //               case Nv::Blast::TkJointUpdateEvent::Unreferenced:
 //                   DeleteJoint(jointEvent->joint);  // This joint is no longer referenced, so we may delete the corresponding physics joint.
 //                   break;
 //               }
	//		break;
	//	case Nv::Blast::TkEvent::TypeCount:
	//		break;
	//	}
	//}
}

void BlastManager::CreateActorInternal(Nv::Blast::TkActor* aActor)
{
	/*uint32_t chunkCount = aActor->getAsset()->getChunkCount();
	physx::PxRigidActor* newActor = myPhysicsRef->createRigidDynamic(physx::PxTransform());
	PhysxFamily& family = myActors.at(aActor->getFamily().getID());
	family.myActors.emplace(aActor->getIndex(), newActor);
	const NvBlastChunk* chunks = aActor->getAsset()->getChunks();
	for(uint32_t i = 0; i<chunkCount; i++)
	{
		chunks[i].centroid;
	}*/
}

void BlastManager::DeleteActorInternal(NvBlastID aFamily, uint32_t aIndex)
{
	/*Squish::PhysicsEngine::Get()->GetScene()->RemoveActor(myActors.at(aFamily).myActors.at(aIndex));
	myActors.at(aFamily).myActors.at(aIndex)->release();
	myActors.at(aFamily).myActors.erase(aIndex);*/
	
}

void BlastManager::NewJoint(Nv::Blast::TkJoint* aJoint)
{
	//aJoint->userData = new uint32_t(jointIndex++);
	////Create a physx joint
	//myJoints.emplace(*static_cast<uint32_t*>(aJoint->userData),PxFixedJointCreate(*myPhysicsRef, 
	//	myActors.at(aJoint->getData().actors[0]->getFamily().getID()).myActors.at(aJoint->getData().actors[0]->getIndex()), //first actor
	//	physx::PxTransform(aJoint->getData().attachPositions[0].x, aJoint->getData().attachPositions[0].y, aJoint->getData().attachPositions[0].z), //Relative position
	//	myActors.at(aJoint->getData().actors[0]->getFamily().getID()).myActors.at(aJoint->getData().actors[0]->getIndex()), //second actor
	//	physx::PxTransform(aJoint->getData().attachPositions[0].x, aJoint->getData().attachPositions[0].y, aJoint->getData().attachPositions[0].z)));	
}

void BlastManager::UpdateJoint(Nv::Blast::TkJoint* aJoint)
{
	//Updates the physx joints actors
	
	//myJoints.at(*static_cast<uint32_t*>(aJoint->userData))->setActors(myActors.at(aJoint->getData().actors[0]->getFamily().getID()).myActors.at(aJoint->getData().actors[0]->getIndex())->is<physx::PxRigidActor>(), myActors.at(aJoint->getData().actors[1]->getFamily().getID()).myActors.at(aJoint->getData().actors[0]->getIndex())->is<physx::PxRigidActor>());
}

void BlastManager::DeleteJoint(Nv::Blast::TkJoint* aJoint)
{
	/*delete static_cast<uint32_t*>(aJoint->userData);
	myJoints.at(*static_cast<uint32_t*>(aJoint->userData))->release();
	*/
}