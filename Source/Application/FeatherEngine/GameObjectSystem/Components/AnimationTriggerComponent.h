#pragma once

#define ANIMATION_TRIGGER_COMPONENT 200
#include "CommonUtilities/InputObserver.h"

class AnimationTriggerComponent : public Component
{
public:
	AnimationTriggerComponent();
	~AnimationTriggerComponent() = default;

	void Update(const float& aDeltaTime) override;

	void Awake() override;

	void RecieveInternal(InternalMSG aEvent) override;

	void Put(Letter anEvent) override;
private:
	bool myHasTriggered;
	bool myNeedsToLoop;
	bool myIsPlayingFirstBridgeAnimation;
};

