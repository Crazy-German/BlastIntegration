#pragma once
#include "CommonUtilities\Vector.hpp"
namespace Squish
{
	//this is only for charactercontroller objects
	struct PhysicsData
	{
		float frictionalValue;
		float gravity;
		float maxFallSpeed;
		CU::Vector3f velocity;

		void ApplyNaturalForces(float aFrictionMultiplier, float aDeltaTime)
		{
			Gravity(aDeltaTime);
			Friction(aFrictionMultiplier, aDeltaTime);
		}

		void Gravity(float aDeltaTime)
		{
			velocity.y -= gravity * aDeltaTime;
			if (-velocity.y > maxFallSpeed)
			{
				velocity.y = -maxFallSpeed;
			}
		}

		void Friction(float aFrictionMultiplier, float aDeltaTime)
		{

			CU::Vector3f flatVelocity = velocity;
			flatVelocity.y = 0;
			flatVelocity.Normalize();
			flatVelocity *= (aFrictionMultiplier * aDeltaTime * frictionalValue);
			
			velocity -= flatVelocity;
		}

		void AddForce(CU::Vector3f aForce)
		{
			velocity += aForce;
		}

		void Stop()
		{
			velocity = { 0, velocity.y,0 };
		}
	};
}