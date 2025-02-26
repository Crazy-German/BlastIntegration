#pragma once

#include <string_view>

class AnimationEventListener
{
public:
	AnimationEventListener() = default;
	~AnimationEventListener() = default;

	virtual void OnAnimationEventTrigger(std::string_view anEventName) = 0;

private:

};

