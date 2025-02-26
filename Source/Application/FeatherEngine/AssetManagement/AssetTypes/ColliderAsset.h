#pragma once
#include "Asset.h"
#include "Physcics/SquishTypeDefines.h"

struct ColliderData
{
	Squish::ShapeType myShapeType;
	CommonUtilities::Vector3f aOffset;
};

struct BoxColliderData : ColliderData
{
	BoxColliderData(CommonUtilities::Vector3f aHalfWidths)
	{
		myHalfWidths = aHalfWidths;
		myShapeType = Squish::Box;
	}
	CommonUtilities::Vector3f myHalfWidths;
};

struct SphereColliderData : ColliderData
{
	SphereColliderData(float aRadius){myRadius =aRadius; myShapeType=Squish::Sphere;};
	SphereColliderData();
	float myRadius;
};
struct CapsuleColliderData : ColliderData
{
	CapsuleColliderData();
	CapsuleColliderData(float aRadius, float aHalfHeight):myRadius(aRadius), myHalfHeight(aHalfHeight){myShapeType=Squish::Capsule;};
	float myRadius;
	float myHalfHeight;
};
struct ConvexColliderData : ColliderData
{
	ConvexColliderData(){myShapeType = Squish::Convex;}
	std::vector<CommonUtilities::Vector3f> myVerticies;
};
struct MeshColliderData : ColliderData
{
	MeshColliderData(){myShapeType = Squish::Mesh;}
	std::vector<CommonUtilities::Vector3f> myVerticies;
	std::vector<uint32_t> myIndiciess;
};


struct ColliderAsset : public Asset
{
	std::vector<std::shared_ptr<ColliderData>> myColliderAssets;
};
