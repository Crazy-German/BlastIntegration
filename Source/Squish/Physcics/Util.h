#pragma once

#include "characterkinematic/PxExtended.h"
#include "CommonUtilities/TransformBase.h"
#include "foundation/PxTransform.h"

#include <queue>

namespace Squish
{
	struct ActorUserData
	{
		TransformBase* myTransform;
		uint32_t aID;
	};
	inline uint32_t PhysXId = 0;
    inline std::queue<uint32_t> reusableID;
	[[nodiscard]] inline uint32_t GetID()
	{
	    if(reusableID.empty())
	    {
	        return PhysXId++;
	    }
        else
        {
            uint32_t retval = reusableID.front();
            reusableID.pop();
            return retval;
        }
	}

	[[nodiscard]] inline physx::PxTransform ToPhysXTransform(const TransformBase& aTransform)
	{
		physx::PxTransform retVal;
		auto pos = aTransform.GetPosition();
		auto rot = aTransform.GetRotationQuaternion();
		retVal.p = physx::PxVec3(pos.x, pos.y, pos.z);
		retVal.q = physx::PxQuat(rot.x, rot.y, rot.z, rot.w);
		return retVal;
	}

	[[nodiscard]] inline TransformBase ToTransformbase(const physx::PxTransform& aTransform)
	{
		TransformBase retVal;
		retVal.SetPosition({aTransform.p.x, aTransform.p.y, aTransform.p.z});
		retVal.SetRotation({aTransform.q.x, aTransform.q.y,aTransform.q.z, aTransform.q.w});
		return retVal;
	}
	[[nodiscard]] inline physx::PxVec3 ToPhysxVect(const CommonUtilities::Vector3f& aVector)
	{
		return {aVector.x, aVector.y, aVector.z};
	}

	[[nodiscard]] inline  CommonUtilities::Vector3f ToVector3f(const physx::PxVec3& aVec3)
	{
		return {aVec3.x, aVec3.y, aVec3.z};
	}
	[[nodiscard]] inline  CommonUtilities::Vector3f ToVector3f(const physx::PxExtendedVec3& aVec3)
	{
		return {(float)aVec3.x, (float)aVec3.y, (float)aVec3.z};
	}
	[[nodiscard]] inline physx::PxExtendedVec3 ToExtendedVec3(const CommonUtilities::Vector3f& aVector)
	{
		return  {aVector.x, aVector.y, aVector.z};
	}
	[[nodiscard]] inline  physx::PxQuat ToPhysxQuat(const CommonUtilities::Quaternionf& aQuaternion)
	{
		return {aQuaternion.x, aQuaternion.y, aQuaternion.z, aQuaternion.w};
	}
	[[nodiscard]] inline CommonUtilities::Quaternionf ToQuaternion(const  physx::PxQuat& aQuaternion)
	{
		return {aQuaternion.x, aQuaternion.y, aQuaternion.z, aQuaternion.w};
	}
}
