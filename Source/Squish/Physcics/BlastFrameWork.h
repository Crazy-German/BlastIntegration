#pragma once

#include "NvCTypes.h"

namespace physx
{
	class PxRigidActor;
	class PxGeometry;
}

class BaseConvexMeshBuilder;

namespace VHACD
{
	class IVHACD;
}

namespace Nv::Blast
{
	class TkFramework;
	class BlastBondGenerator;
	struct AuthoringResult;
	struct CollisionHull;
	struct ConvexDecompositionParams;
	struct Triangle;
	class ConvexMeshBuilder;
}
#define SAFE_ARRAY_NEW(T, x) ((x) > 0) ? reinterpret_cast<T*>(NVBLAST_ALLOC(sizeof(T) * (x))) : nullptr;
#define SAFE_ARRAY_DELETE(x) if (x != nullptr) {NVBLAST_FREE(x); x = nullptr;}

struct GeometryData
{
	physx::PxGeometry** myGeometry;
	uint32_t myGeometryCount;
};

class BlastFrameWork
{
public:
	static BlastFrameWork& GetInstance();
	~BlastFrameWork();
	void buildPhysxChunk(Nv::Blast::AuthoringResult& aResult, const Nv::Blast::ConvexDecompositionParams& iParams, GeometryData**& aOutGeometry,
	                     uint32_t aChunksToProcessCount = 0, uint32_t* aChunksToProcess = nullptr);
	uint32_t buildConvexMeshDecomposition(Nv::Blast::Triangle* aMesh, uint32_t aTriangleCount, const Nv::Blast::ConvexDecompositionParams& iParams, Nv::Blast::
	                                      CollisionHull**& aOutHulls, GeometryData*& aOutGeometry);
	void SetHullAndBondGenerator(BaseConvexMeshBuilder* aGen);
	void SetHullGenerator(BaseConvexMeshBuilder* aGen);
	void SetHullAndBondGeneratorDefault();
	static float CalculateGeometryVolumeAndCentroid(NvcVec3& aCentriod, const Nv::Blast::Triangle* aTris, size_t aTriCount); //Copy of function in blast impl as that cannot normally be accessed 
	static float CalculateCollisionVolumeAndCentroid(NvcVec3& aCentriod, const Nv::Blast::CollisionHull& aHull); //Copy of function in blast impl as that cannot normally be accessed
	physx::PxRigidActor* CreateRigidActorFromGeometry(physx::PxGeometry** aGeometry, uint32_t aGeometryCount, const NvcVec3& aPosition, const NvcQuat& aRotation);
	physx::PxRigidActor* CreateRigidActorFromGeometry(GeometryData** aGeometryData, uint32_t* aIndicies, uint32_t aIndexCount, const NvcVec3& aPosition, const NvcQuat&
	                                                  aRotation);
	Nv::Blast::TkFramework* GetBlastFrameWork();
private:
	BlastFrameWork();
	VHACD::IVHACD* myDecompoeser;
    BaseConvexMeshBuilder* myConvexHullGenerator;
	Nv::Blast::BlastBondGenerator* myBondGenerator;
};

