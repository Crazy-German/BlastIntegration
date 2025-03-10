#pragma once
#include "NvBlastExtAuthoringConvexMeshBuilder.h"

namespace physx
{
	class PxGeometry;
}

class BaseConvexMeshBuilder : public Nv::Blast::ConvexMeshBuilder
{
	/*
	 [in] verticesCount - The number of vertices in the input mesh.
	 [in] vertexData - The array of vertices in the input mesh.
	 [out] aGeometry - The geometry to be used for the collision hull.
	 returns - The collision hull generated from the input mesh.
	 */
public:
     virtual Nv::Blast::CollisionHull* buildCollisionGeometry(uint32_t verticesCount, const NvcVec3* vertexData, physx::PxGeometry*& aGeometry) = 0;
};

class ConvexHullGenerator : public BaseConvexMeshBuilder
{
public:
    ConvexHullGenerator();
    ~ConvexHullGenerator() override = default;
    Nv::Blast::CollisionHull* buildCollisionGeometry(uint32_t verticesCount, const NvcVec3* vertexData) override;
	Nv::Blast::CollisionHull* buildCollisionGeometry(uint32_t verticesCount, const NvcVec3* vertexData, physx::PxGeometry*& aGeometry) override;
    void release() override;
    void releaseCollisionHull(Nv::Blast::CollisionHull* hull) const override;
};

class BoundingBoxGenerator : public  BaseConvexMeshBuilder
{
public:
    BoundingBoxGenerator();
	~BoundingBoxGenerator() override;
	void release() override;
	Nv::Blast::CollisionHull* buildCollisionGeometry(uint32_t verticesCount, const NvcVec3* vertexData) override;
	Nv::Blast::CollisionHull* buildCollisionGeometry(uint32_t verticesCount, const NvcVec3* vertexData, physx::PxGeometry*& aGeometry) override;
	void releaseCollisionHull(Nv::Blast::CollisionHull* hull) const override;
};
