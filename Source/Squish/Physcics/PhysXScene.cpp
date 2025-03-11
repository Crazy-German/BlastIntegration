#include "Physx/include/PxPhysics.h"
#include "include/PxPhysicsAPI.h"
#include <Physx/include/cooking/PxCooking.h>
#include <Physx/include/cooking/PxConvexMeshDesc.h>
#include <vector>

#include "PhysXScene.h"

#include "RigidBody.h"
#include "Util.h"
#include "include/extensions/PxDefaultCpuDispatcher.h"
#include "include/extensions/PxDefaultSimulationFilterShader.h"
#include "include/extensions/PxDefaultStreams.h"
#include "include/extensions/PxSimpleFactory.h"
#include "include/foundation/PxPhysicsVersion.h"
#include "include/pvd/PxPvd.h"
#include "include/pvd/PxPvdSceneClient.h"
#include "include/pvd/PxPvdTransport.h"
#include "include/PxScene.h"
#include "include/extensions/PxDefaultAllocator.h"
#include "include/extensions/PxDefaultErrorCallback.h"
#include "include/PxRigidActor.h"
#include "include/PxRigidStatic.h"


using namespace physx;
using namespace Squish;

static physx::PxDefaultAllocator		gAllocator;
 static physx::PxDefaultErrorCallback	gErrorCallback;

//
//
//PhysXScene::PhysXScene()
//{
//	
//	
//	//gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
//	//PxConvexMeshDesc meshdesc;
//	//meshdesc.points.count = 20;
//	//meshdesc.points.stride = sizeof(PxVec3);
//	//std::vector<PxVec3> test;
//	//test.emplace_back(PxVec3());
//	//test.emplace_back(PxVec3());
//	//test.emplace_back(PxVec3());
//	//test.emplace_back(PxVec3());
//	//meshdesc.points.data = test.data();
//	//meshdesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
//	//meshdesc.vertexLimit = 10;
//	//PxTolerancesScale scale;
//	//PxCookingParams cook(scale);
//	//PxDefaultMemoryOutputStream buf;
//	//PxCookConvexMesh(cook, meshdesc, buf);
//	//PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
//	//PxConvexMesh* mesh = gPhysics->createConvexMesh(input);
//	//PxMeshScale Scale(PxVec3(1,1,1));
//	//PxConvexMeshGeometry geom(mesh, Scale);
//	//PxTransform Transform;
//	////PxRigidActor* ground = PxCreateStatic(*gPhysics,Transform, geom, *gMaterial);
//	//PxRigidActor* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0,1,0,0), *gMaterial);
//	//myScene->addActor(*groundPlane);
//	//myScene->addActor(*ground);
//}

PhysXScene::PhysXScene(physx::PxScene* aScene)
{
	myScene = aScene;
	myScene->setFlag(PxSceneFlag::eENABLE_ACTIVE_ACTORS, true);
	myScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
	myScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
	myTimeStep = 1.f/60.f;
	myAccumulator = 0;
	PxPvdSceneClient* pvdClient = myScene->getScenePvdClient();
	if(pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
}

PhysXScene::~PhysXScene()
{
	PX_RELEASE(myScene);
	
	printf("Shutdown Scene");
}

void PhysXScene::Update(float aDeltaTime)
{
	PxU32 actoramount = myScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC);
	PxActor** actors = new PxActor*[actoramount];
	myScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC, actors, actoramount);
	for (PxU32 i=0; i < actoramount; ++i)
	{
		/*if(!actors[i]->userData)
		{
			continue;
		}
		TransformBase* transform = static_cast<ActorUserData*>(actors[i]->userData)->myTransform;
		PxVec3 pos = PxVec3(transform->GetPosition());
		PxQuat quat = PxQuat(transform->GetRotationQuaternion().x, transform->GetRotationQuaternion().y, transform->GetRotationQuaternion().z, transform->GetRotationQuaternion().w);
		actors[i]->is<PxRigidActor>()->setGlobalPose(PxTransform(pos, quat));*/
	}
	delete[] actors;
	myAccumulator+=aDeltaTime;
	if(myAccumulator<myTimeStep)
	{
		return;
	}
	myAccumulator-=myTimeStep;
	myScene->simulate(myTimeStep);
	myScene->fetchResults(true);

	PxU32 nbActiveActors;
	PxActor** activeActors = myScene->getActiveActors(nbActiveActors);
	for (PxU32 i=0; i < nbActiveActors; ++i)
	{
		/*ActorUserData* uData = static_cast<ActorUserData*>(activeActors[i]->userData);
		TransformBase* transform = uData->myTransform;
		PxVec3 pos = activeActors[i]->is<PxRigidActor>()->getGlobalPose().p;
		PxQuat rot = activeActors[i]->is<PxRigidActor>()->getGlobalPose().q;
		transform->SetPosition({pos.x, pos.y, pos.z});*/
		//transform->SetRotation({rot.x, rot.y, rot.z, rot.w});
	}
}

void PhysXScene::AddActor(physx::PxBase* aActor)
{
	if(aActor->is<PxActor>())
	{
		myScene->addActor(*aActor->is<PxActor>());
	}
}

void PhysXScene::RemoveActor(physx::PxBase* aActor)
{
	if(aActor != nullptr && aActor->is<PxActor>())
	{
		myScene->removeActor(*aActor->is<PxActor>());
	}
}

void PhysXScene::AddActor(RigidBody* aActor)
{
	if(!aActor->IsValid())
	{
		printf("Failed to ad actor to scene\n");
		return;
	}
	myScene->addActor(aActor->GetPhysxActor());
}

void PhysXScene::AddCallback(physx::PxSimulationEventCallback* aCallback)
{
	myScene->setSimulationEventCallback(aCallback);
}

void PhysXScene::SetGravity(CU::Vector3f aGravity)
{
	myScene->setGravity({aGravity.x, aGravity.y, aGravity.z});
}

auto PhysXScene::GetGravity() const -> CU::Vector3f
{
	return ToVector3f(myScene->getGravity());
}

void PhysXScene::ClearScene()
{
	uint32_t actorCount = myScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC );
	PxActor** actors = new PxActor*[actorCount];
	myScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, actors, actorCount);
	for(uint32_t index = 0; index < actorCount; index++)
	{
		if(actors[index]->isReleasable())
		{
		myScene->removeActor(*actors[index]);
		}
	}
}

void PhysXScene::SetTimeStep(const float& aTimeStep)
{
	myTimeStep = aTimeStep;
}

bool PhysXScene::Raycast(const CommonUtilities::Vector3f& aOrigin, const CommonUtilities::Vector3f aDirection, float aMaxDistance, RayCastBuffer& aRayCastBuffer, const PxFilterData& aFilter, const PxQueryFlags aFlags) const
{
	PxRaycastBuffer aBuffer;
	PxQueryFilterData FilterData;
	FilterData.data = aFilter;
	FilterData.flags = aFlags;
	bool retval = myScene->raycast(ToPhysxVect(aOrigin), ToPhysxVect(aDirection.GetNormalized()), aMaxDistance, aBuffer, PxHitFlag::eANY_HIT |PxHitFlag::eNORMAL | PxHitFlag::eFACE_INDEX, FilterData);
	
	if(retval && aBuffer.hasAnyHits())
	{
		aRayCastBuffer.hasBlock = aBuffer.hasBlock;
		if(aBuffer.hasBlock)
		{
			aRayCastBuffer.Block = {ToVector3f(aBuffer.block.position), ToVector3f(aBuffer.block.normal)};
		}
		for(unsigned index = 0; index < aBuffer.getNbAnyHits(); index++)
		{
			aRayCastBuffer.Hits.emplace_back(ToVector3f(aBuffer.getAnyHit(index).position), ToVector3f(aBuffer.getAnyHit(index).normal));
		}
	}
	return retval;
	
}
