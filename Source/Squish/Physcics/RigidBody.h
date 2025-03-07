#pragma once
#include <vector>

//#include <PxSimulationEventCallback.h>
#include <CommonUtilities/TransformBase.h>

#include <Physx/include/PxRigidDynamic.h>

#include "CollisionBase.h"
#include "SquishTypeDefines.h"
#include "Util.h"
#include "Logger/Logger.h"

//Added by Viktor, may be moved to a better place
#ifdef _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(PhysXLog, "PhysXLog", Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(PhysXLog, "PhysXLog", Error);
#endif

inline DEFINE_LOG_CATEGORY(PhysXLog);

namespace Squish
{
	
	class RigidBody : public CollisionBase
	{
	public:
		RigidBody();
		~RigidBody();
		//Sphere
		bool Initialize(RigidBodyType aType, ShapeType aShape, TransformBase* aTransform, CommonUtilities::Vector3f aMaterial, float aRadius, 
						const TransformBase& aLocalTransform = TransformBase(), uint16_t aGroup = 0);
		//Box
		bool Initialize(RigidBodyType aType, ShapeType aShape, TransformBase* aTransform, CommonUtilities::Vector3f aMaterial, CommonUtilities::Vector3f aHalfWidths, 
						const TransformBase& aLocalTransform = TransformBase(), uint16_t aGroup = 0);
		//Capsule
		bool Initialize(RigidBodyType aType, ShapeType aShape, TransformBase* aTransform, CommonUtilities::Vector3f aMaterial, float aRadius, float aHalfHeight, 
						const TransformBase& aLocalTransform = TransformBase(), uint16_t aGroup = 0);
		//Convex/Mesh you rich guy
		bool Initialize(RigidBodyType aType, ShapeType aShape, TransformBase* aTransform, CommonUtilities::Vector3f aMaterial, std::vector<CommonUtilities::Vector3f>* aVertexlist, std::vector<uint32_t>* aIndexList,
						uint32_t aVertexLimit, uint32_t aPolygonLimit, const TransformBase& aLocalTransform = TransformBase(), uint16_t aGroup = 0);
		bool InitNoShape(RigidBodyType aType);
		void AddShape(RigidBodyType aType, ShapeType aShape, const TransformBase& aTransform, CommonUtilities::Vector3f aMaterial, float aRadius);
		void AddShape(RigidBodyType aType, ShapeType aShape, const TransformBase& aTransform, CommonUtilities::Vector3f aMaterial, CommonUtilities::Vector3f aHalfWidths);
		void AddShape(RigidBodyType aType, ShapeType aShape, const TransformBase& aTransform, CommonUtilities::Vector3f aMaterial, float aRadius, float aHalfHeight);
		void AddShape(RigidBodyType aType, ShapeType aShape, const TransformBase& aTransform, CommonUtilities::Vector3f aMaterial, std::vector<CommonUtilities::Vector3f>* aVertexlist, std::vector<uint32_t>* aIndexList,
		              uint32_t aVertexLimit, uint32_t aPolygonLimit);

		void AddForce(CommonUtilities::Vector3f aForce, uint8_t aForceType = 0);//0:Force, 1:Impulse, 2:velocity_change, 3:acceleration
		CommonUtilities::Vector3f GetVelocity() const;

		void AddRotationForce(CommonUtilities::Vector3f aForce, uint8_t aForceType = 0);//Force, Impulse, velocity_change, acceleration
		CommonUtilities::Vector3f GetRotationalVelocity() const;

		void SetCollisionGroup(uint16_t aGroup);
		uint16_t GetCollisionGroup();

		void SetCollisionLayer(const physx::PxFilterData* aLayer);
		physx::PxFilterData SetAndDefineCollisionLayer(physx::PxU32 aOwn, physx::PxU32 aToFilterWith); // aOwn = What layer this shape should be on, aToFilterWith what layers it should collide with
		//Varius sets and gets
		void SetMaxSpeed(const float& aMaxSpeed);
		float GetMaxSpeed() const;
		void SetMaxRotation(const float& aMaxRotation);
		float GetMaxRotation() const;
		void SetMass(const float& aMass);
		float GetMass() const;
		void SetLinearDamping(const float& aDampingCoefficient);
		float GetLinearDamping() const;
		void SetAngularDamping(const float aDampingCoefficient);
		float GetAngularDamping() const;
		//Various sets for flags
		void SetIsAffectedByGravity(bool aIsEffected);
		bool GetIsAffectedByGravity();

		void SetSolverIteratiorCount(uint32_t aPositionIteratorCount = 4, uint32_t aVelocityIteratorCount = 1);

		void PrintFrictions();

		void SetKinematic(bool aIsKinematic);

		//Stop it from moving horizontally
		void LockHorizontalMovement();
		void UnLockHorizontalMovement();

		void LockRotationAxis(physx::PxRigidDynamicLockFlag::Enum aLock) const; // locks a dynamic rigid body to not be able to move in a certain direction angular is rotation linear is position
		void UnLockRotationAxis(physx::PxRigidDynamicLockFlag::Enum aLock) const; // locks a dynamic rigid body to not be able to move in a certain direction angular is rotation linear is position

		void SetTransform(TransformBase* aTransform);
		void UpdateTransform();

		physx::PxRigidActor& GetPhysxActor();
		const ActorUserData* GetUserData() const override;
		void SetQueryFilterData(physx::PxFilterData aData);
		physx::PxFilterData GetQueryFilterData() const;
		const bool IsValid() const;
	protected:
		physx::PxRigidActor* myRigidBody;
		RigidBodyType myType;
		physx::PxFilterData myFilterData;
	};
}
