#pragma once
namespace Squish
{
	enum RigidBodyType : uint8_t
	{
		Static = 1,
		Dynamic = 1<<1,
		Trigger = 1<<2
	};
	enum ShapeType : uint8_t
	{
		Sphere = 1,
		Box = 1<<1,
		Capsule = 1<<2,
		Mesh = 1<< 3,
		Convex = 1<<4

	};
}
