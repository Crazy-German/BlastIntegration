#pragma once
#include "CollisionBase.h"
#include "Util.h"
#include "characterkinematic/PxController.h"
#include "characterkinematic/PxControllerBehavior.h"
#include "CommonUtilities/TransformBase.h"
#include "CommonUtilities/Vector3.hpp"

namespace physx
{
	class PxControllerDesc;
	class PxControllerFilters;
	class PxController;
}
namespace Squish
{
	enum ControllerType
	{
		CapsuleController,
		BoxController,
	};
	class CharacterController : public CollisionBase,public physx::PxUserControllerHitReport, public physx::PxControllerBehaviorCallback
	{
	public:
		CharacterController();
		~CharacterController() override;
		/*
		 [in] aTransform: aPointer to the object transform that the controller should be applied to
		 [in] aRadiusWidth: aRadius for capsules and half width for box
		 [in] aHeight: Height for capsule and half height for box
		 [in] aDepth : HalfDepth of box warning this is only used for box and is otherwise ignored
		 [in] aControllerType: Specifies what type the controller is either Capsule or Box, capsule is recommended
		 [in] aContactOffset: ContactOffset of the shape, meaning how far from its bound it should start detecting collision
		 [in] aStepOffset: max Height of auto step
		 [in] aSlopeLimit: a limit in degrees of how steep a surface can be before it becomes non-walkable
		 */
		void Init(TransformBase* aTransform, const float& aRadiusWidth, const float& aHeight, const float& aDepth = 0,ControllerType aControllerType = CapsuleController, const
		          float& aContactOffset = 0.1f, const
		          float& aStepOffset = 0.5f, const float& aSlopeLimit = 45.f);
		void Update(float aDeltaTime);
		void AddForce(const CommonUtilities::Vector3f& aForce);
		/**
		 * 
		 * @param hit 
		 */
		virtual void onShapeHit(const physx::PxControllerShapeHit& hit) override;
		virtual void onControllerHit(const physx::PxControllersHit& hit) override;
		virtual void onObstacleHit(const physx::PxControllerObstacleHit& hit) override;
		virtual physx::PxControllerBehaviorFlags getBehaviorFlags(const physx::PxShape& shape, const physx::PxActor& actor) override;
		virtual physx::PxControllerBehaviorFlags getBehaviorFlags(const physx::PxController& controller) override;
		virtual physx::PxControllerBehaviorFlags getBehaviorFlags(const physx::PxObstacle& obstacle) override;


		[[nodiscard]] auto GetCollisionFlags() const -> physx::PxControllerCollisionFlags;
		[[nodiscard]] auto GetVelocity() const -> CU::Vector3f;
		void SetFilterLayer(physx::PxFilterData* aFilterData);
		const Squish::ActorUserData* GetUserData() const override;
		CommonUtilities::Vector3f GetFotPosition() const;
	protected:
		physx::PxController* myContoller;
		physx::PxControllerDesc* myControllerDesc;
		physx::PxControllerFilters myFilter;
		physx::PxControllerCollisionFlags myCollisionFlags;
		CommonUtilities::Vector3f myForce;
	};
}