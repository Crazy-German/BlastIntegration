#pragma once
#include "ColliderComponent.h"
#include "CommonUtilities/AABB3D.hpp"

#define AABB_COLLIDER_COMPONENT 50

class AABBColliderComponent : public ColliderComponent
{
public:
	void Awake() override{};
	void Destroy() override{};
	void Update(const float& aDeltaTime) override{};
	void Render() override{};
	void Put(Letter aLetter) override{};

private:
	CommonUtilities::AABB3D<float> myCollider;
};
