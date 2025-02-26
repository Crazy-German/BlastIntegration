#pragma once
#include "PxSimulationEventCallback.h"
#include "CommonUtilities/Vector3.hpp"

class ContactHandler;

namespace physx
{
	class PxControllerDesc;
}

namespace physx
{
	class PxController;
	class PxControllerManager;
	class PxPvd;
	class PxDefaultCpuDispatcher;
	class PxPhysics;
	class PxFoundation;
	class PxTolerancesScale;
}

namespace Squish
{
	class PhysXScene;
		
	class PhysicsEngine
	{
	public:
		void Initialize(float aLength = 100, float aSpeed = 981*981, bool aCreateEmptyScene = true); //Typicall lenght of an object, typical speed of a object usually gravity, create
		PhysXScene* CreateScene(CommonUtilities::Vector3f aGravity = {0,-981,0});
		void SetScene(PhysXScene* aScene);
		void ResetScene();
		void RemoveController(physx::PxController* aController);
		PhysXScene* GetScene() const;
		void Update(const float aDeltaTime);
		physx::PxPhysics* GetPhysics();
		bool CreateController(physx::PxController** aController, const physx::PxControllerDesc* aDesc);
		static PhysicsEngine* Get();
	private:
		~PhysicsEngine();
		PhysicsEngine();
		physx::PxFoundation* myFoundation;
		physx::PxTolerancesScale* myWorldScale;
		physx::PxPhysics* myPhysics;
		physx::PxControllerManager* myControllers;
		PhysXScene* myScene;
		physx::PxDefaultCpuDispatcher*	myDispatcher;
		physx::PxPvd*					gPvd;
        
	};

	void InitializeSquish(float aLength = 100, float aSpeed = 981*981, bool aCreateEmptyScene = true);
	void Update(const float& aDeltaTime);
	void SetCallback(physx::PxSimulationEventCallback* aContactHanlder);
	
}
