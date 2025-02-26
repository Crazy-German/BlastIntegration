#pragma once
#include "Component.h"
#include "GameObjectSystem\Components\Transform.h"
#include "Collision/Collider.h"
#include "GameObjectSystem\GameObject.h"

class DebugLineObject;
class Mesh;

enum CollisionLayer : unsigned
{
	Static = 1,
	Player = 2,
	Enemy = 4,
	Trigger = 8,
	Boss = 16,
	BossHurtBox = 32
};

class ColliderComponent : public Component
{
public:

	ColliderComponent() = default;
	~ColliderComponent() = default;
	void SetPosition(const Vec3 aPosition);
	const CollisionLayer& GetLayer() const;
	const float& GetValue() const;
	void SetValue(float aValue = -1);
	virtual Collider* GetCollider() = 0;
	void SetEventType(InternalEvent aEventType);
	InternalEvent GetEventType() const;
	void Render() override;
	void DebugRender() override;

protected:
	CU::Vector4f myTransformOffset;
	CollisionLayer myLayer;
	float myValue = -1;
	InternalEvent myEventType = InternalEvent::eHit;
	std::shared_ptr<Mesh> myDebugMesh;
	std::shared_ptr<DebugLineObject> myDebugLines;
};