#pragma once
#include "Util.h"

namespace Squish
{
	class CollisionBase
	{
	public:
		virtual ~CollisionBase() = default;
		CollisionBase() = default;
		virtual const Squish::ActorUserData* GetUserData() const = 0;
	protected:
		ActorUserData myUserData;
	};
}