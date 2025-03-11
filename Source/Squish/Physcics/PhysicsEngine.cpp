#include "PhysicsEngine.h"
#include <Physx/include/PxPhysicsAPI.h>

#include "PhysXScene.h"
#include "Util.h"
#include "CommonUtilities/AABB3D.hpp"

using namespace physx;

static physx::PxDefaultAllocator		gAllocator;
static physx::PxDefaultErrorCallback	gErrorCallback;

PxFilterFlags FilterShader(PxFilterObjectAttributes attr0, PxFilterData filter0,PxFilterObjectAttributes attr1, PxFilterData filter1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	constantBlockSize;
	constantBlock;
	if(PxFilterObjectIsTrigger(attr0) || PxFilterObjectIsTrigger(attr1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eCALLBACK;
	}
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;
	if((filter0.word0 & filter1.word1) && (filter1.word0 & filter0.word1))
        pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
        pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT;
        pairFlags |= PxPairFlag::eNOTIFY_TOUCH_CCD;
        pairFlags |= PxPairFlag::ePOST_SOLVER_VELOCITY;
        pairFlags |= PxPairFlag::ePRE_SOLVER_VELOCITY;
		pairFlags |= PxPairFlag::eCONTACT_EVENT_POSE;
        pairFlags |= PxPairFlag::eNOTIFY_CONTACT_POINTS;
	
		
    return PxFilterFlag::eCALLBACK;
}

Squish::PhysicsEngine::PhysicsEngine()
	:myScene(nullptr), myPhysics(nullptr), myWorldScale(nullptr)
{
	myFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	gPvd = PxCreatePvd(*myFoundation);
	
	
	
	myDispatcher = PxDefaultCpuDispatcherCreate(2);
	myWorldScale = new PxTolerancesScale();
}

void Squish::InitializeSquish(float aLength, float aSpeed, bool aCreateEmptyScene)
{
	PhysicsEngine::Get()->Initialize(aLength, aSpeed, aCreateEmptyScene);
}

void Squish::Update(const float& aDeltaTime)
{
	PhysicsEngine::Get()->Update(aDeltaTime);
}

void Squish::SetCallback(physx::PxSimulationEventCallback* aContactHanlder)
{
	PhysicsEngine::Get()->GetScene()->AddCallback(aContactHanlder);
}

Squish::PhysicsEngine::~PhysicsEngine()
{
	//Handle Scene
	delete myWorldScale;
	PX_RELEASE(myDispatcher);
	PX_RELEASE(myPhysics);
	if(gPvd)
	{
		PxPvdTransport* transport = gPvd->getTransport();
		PX_RELEASE(gPvd);
		PX_RELEASE(transport);
	}
	PX_RELEASE(myFoundation);
	printf("Shutdown physics");
}

void Squish::PhysicsEngine::Initialize(float aLength, float aSpeed, bool aCreatemptyScene)
{
	myWorldScale->length = aLength;
	myWorldScale->speed = aSpeed;
	myPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *myFoundation, *myWorldScale,true, gPvd);
	PxInitExtensions(*myPhysics, gPvd);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	if(!transport)
	{
		printf("Failed to create transport\n");
	}
	if(gPvd->connect(*transport,PxPvdInstrumentationFlag::eALL))
	{
		printf("Failed to connect to PVD\n");
	}
	if(aCreatemptyScene)
	{
		myScene = CreateScene();
		myControllers = PxCreateControllerManager(*myScene->myScene);
	}
}

Squish::PhysXScene* Squish::PhysicsEngine::CreateScene(CommonUtilities::Vector3f aGravity)
{
	PxSceneDesc sceneDesc(myPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(aGravity.x, aGravity.y, aGravity.z);
	myDispatcher = PxDefaultCpuDispatcherCreate(8);
	sceneDesc.cpuDispatcher	= myDispatcher;
	sceneDesc.filterShader	= FilterShader;
	sceneDesc.ccdMaxPasses = 2;
	PxVec3 maxExtent(PX_MAX_BOUNDS_EXTENTS,PX_MAX_BOUNDS_EXTENTS, PX_MAX_BOUNDS_EXTENTS);
	sceneDesc.sanityBounds = PxBounds3(/*PxVec3(-10000,-10000,-10000)*/-maxExtent,  maxExtent/*(10000,10000,10000)*/);
	sceneDesc.solverType = PxSolverType::eTGS;
	sceneDesc.flags = PxSceneFlag::eENABLE_CCD|PxSceneFlag::eENABLE_BODY_ACCELERATIONS | PxSceneFlag::eENABLE_SOLVER_RESIDUAL_REPORTING |PxSceneFlag::eENABLE_EXTERNAL_FORCES_EVERY_ITERATION_TGS;// |PxSceneFlag::eENABLE_PCM;
	PhysXScene* scene = new PhysXScene(myPhysics->createScene(sceneDesc));
	return scene;
}

void Squish::PhysicsEngine::SetScene(PhysXScene* aScene)
{
	myScene = aScene;
	if(myControllers)
	{
		PX_RELEASE(myControllers);
	}
	myControllers = PxCreateControllerManager(*myScene->myScene);
}

void Squish::PhysicsEngine::ResetScene()
{
	myScene->ClearScene();
	PhysXId = 0;
}

void Squish::PhysicsEngine::RemoveController(physx::PxController* aController)
{
	aController->release();
}

Squish::PhysXScene* Squish::PhysicsEngine::GetScene() const
{
	return myScene;
}

void Squish::PhysicsEngine::Update(const float aDeltaTime)
{
	myScene->Update(aDeltaTime);
}

Squish::PhysicsEngine* Squish::PhysicsEngine::Get()
{
	static PhysicsEngine myEngine;
	return &myEngine;
}

physx::PxPhysics* Squish::PhysicsEngine::GetPhysics()
{
	return myPhysics;
}

bool Squish::PhysicsEngine::CreateController(physx::PxController** aController, const physx::PxControllerDesc* aDesc)
{
	*aController = myControllers->createController(*aDesc);
	return *aController != nullptr;
}
