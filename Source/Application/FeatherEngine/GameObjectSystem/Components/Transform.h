#pragma once
#include "CommonUtilities/TransformBase.h"
#include "Component.h"

#define TRANSFORM 25000
class Transform : public Component, public TransformBase // <---- This class is what you are looking for
{
public:
	Transform() = default;
	void Update(const float& aDeltaTime) override;
};

