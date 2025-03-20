#pragma once

#include "NvAllocatorCallback.h"
#include "NvCTypes.h"
#include "PxJoint.h"
#include "foundation/PxVec3.h"

namespace Nv::Blast
{
	class TkJoint;
}

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
	physx::PxVec3 myCenter;
};

class BlastFrameWork
{
public:
	//Framework
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
	physx::PxRigidActor* CreateRigidActorFromGeometry(physx::PxGeometry** aGeometry, uint32_t aGeometryCount, const NvcVec3& aPosition, const NvcQuat& aRotation, void*
	                                                  aShapeUserData);
	physx::PxRigidActor* CreateRigidActorFromGeometry(GeometryData** aGeometryData, uint32_t* aIndicies, uint32_t aIndexCount, const NvcVec3& aPosition, const NvcQuat&
	                                                  aRotation, void* aShapeUserData);
	physx::PxJoint* CreatePhysxJoint(const nvidia::NvVec3* aAttatchpos, physx::PxRigidActor* aActor0, physx::PxRigidActor* aActor1);

	Nv::Blast::TkFramework* GetBlastFrameWork();
	//Util
	static nvidia::NvVec3 ToNVVec3(const float* aVec3);
	static NvcVec3 ToNvcVec3(const float* aVec3);

	static physx::PxVec3 toPxVec3(const float* aVec3);
	static physx::PxVec3 toPxVec3(const nvidia::NvVec3& aVec3);
	static physx::PxVec3 toPxVec3(const NvcVec3& aVec3);
private:
	BlastFrameWork();
	VHACD::IVHACD* myDecompoeser;
    BaseConvexMeshBuilder* myConvexHullGenerator;
	Nv::Blast::BlastBondGenerator* myBondGenerator;
};

