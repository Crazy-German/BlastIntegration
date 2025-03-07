#pragma once
#include "NvBlastExtAuthoringConvexMeshBuilder.h"

class ConvexHullGenerator : public Nv::Blast::ConvexMeshBuilder
{
public:
    ConvexHullGenerator();
    ~ConvexHullGenerator() override = default;
    Nv::Blast::CollisionHull* buildCollisionGeometry(uint32_t verticesCount, const NvcVec3* vertexData) override;
    void release() override;
    void releaseCollisionHull(Nv::Blast::CollisionHull* hull) const override;
};

class BoundingBoxGenerator : public  Nv::Blast::ConvexMeshBuilder
{
public:
    BoundingBoxGenerator();
	~BoundingBoxGenerator() override;
	void release() override;
	Nv::Blast::CollisionHull* buildCollisionGeometry(uint32_t verticesCount, const NvcVec3* vertexData) override;
	void releaseCollisionHull(Nv::Blast::CollisionHull* hull) const override;
};
