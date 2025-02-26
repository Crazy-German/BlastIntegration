#pragma once
#include "CommonUtilities/Vector3.hpp"

using Vec3 = CommonUtilities::Vector3<float>;

enum ColliderType: unsigned
{
	Sphere = 1,
	OBB = 2,
	AABB = 4,
	Capsule = 8,
	Hull = 16
};

class Collider
{
public:
	Collider() = default;
	virtual ~Collider() = default;
	virtual Vec3 FindFurthestPoint(Vec3 aDirection) const = 0;
	virtual void SetPosition(Vec3 aPosition) = 0;
	void SetCollisionLayers(const unsigned& aLayers);
	unsigned GetLayers() const;
	ColliderType GetType() const;
protected:
	ColliderType myType;
	unsigned myCollisionLayers;
};

inline void Collider::SetCollisionLayers(const unsigned& aLayers)
{
	myCollisionLayers = aLayers;
}

inline unsigned Collider::GetLayers() const
{
	return myCollisionLayers;
}

inline ColliderType Collider::GetType() const
{
	return myType;
}
