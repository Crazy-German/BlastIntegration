#pragma once
#include "Component.h"

#define LEVEL_SWAP_COMPONENT 5

class LevelSwapComponent : public Component
{
public:
	LevelSwapComponent() = default;
	~LevelSwapComponent() override = default;
	void Init(const std::string& aScene);
	void RecieveInternal(InternalMSG aEvent) override;

private:
	std::string myScene;
};