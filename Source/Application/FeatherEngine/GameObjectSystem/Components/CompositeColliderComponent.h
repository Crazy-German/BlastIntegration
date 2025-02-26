#pragma once
#include <vector>

#include "ColliderComponent.h"
#include "Collision/Collider.h"

#define COMPOSITE_COLLIDER_COMPONENT 50

class CompositeColliderComponent : public ColliderComponent
{
public:
	CompositeColliderComponent() = default;
	~CompositeColliderComponent() override = default;
	void Awake() override{};
	void Destroy() override{};
	void Update(const float& aDeltaTime) override{};
	void Render() override{};
	void Put(Letter aLetter) override{};
	void AddCollider(std::shared_ptr<ColliderComponent> aCollider){ myCollider.push_back(aCollider);};
	std::vector<std::shared_ptr<ColliderComponent>>& GetColliders() const{return myCollider;};
	Collider* GetCollider() override{};
private:
	std::vector<std::shared_ptr<ColliderComponent>> myCollider;
};
