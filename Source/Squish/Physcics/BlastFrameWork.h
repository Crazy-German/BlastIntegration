#pragma once
#include "NvCTypes.h"

namespace VHACD
{
	class IVHACD;
}

class ConvexHullGenerator;

namespace Nv::Blast
{
	class BlastBondGenerator;
	struct AuthoringResult;
	struct CollisionHull;
	struct ConvexDecompositionParams;
	struct Triangle;
	class ConvexMeshBuilder;
}

#define SAFE_ARRAY_NEW(T, x) ((x) > 0) ? reinterpret_cast<T*>(NVBLAST_ALLOC(sizeof(T) * (x))) : nullptr;
#define SAFE_ARRAY_DELETE(x) if (x != nullptr) {NVBLAST_FREE(x); x = nullptr;}

class BlastFrameWork
{
public:
	static BlastFrameWork& GetInstance();
	~BlastFrameWork();
	void buildPhysxChunk(Nv::Blast::AuthoringResult& aResult, const Nv::Blast::ConvexDecompositionParams& iParams, 
						 uint32_t aChunksToProcessCount = 0, uint32_t* aChunksToProcess = nullptr);
	uint32_t buildConvexMeshDecomposition(Nv::Blast::Triangle* aMesh, uint32_t aTriangleCount, const Nv::Blast::ConvexDecompositionParams& iParams, Nv::Blast::
	                                      CollisionHull**& aOutHulls);
	void SetHullAndBondGenerator(Nv::Blast::ConvexMeshBuilder* aGen);
	void SetHullGenerator(Nv::Blast::ConvexMeshBuilder* aGen);
	void SetHullAndBondGeneratorDefault();
	static float CalculateGeometryVolumeAndCentroid(NvcVec3& aCentriod, const Nv::Blast::Triangle* aTris, size_t aTriCount); //Copy of function in blast impl as that cannot normally be accessed 
	static float CalculateCollisionVolumeAndCentroid(NvcVec3& aCentriod, const Nv::Blast::CollisionHull& aHull); //Copy of function in blast impl as that cannot normally be accessed 
private:
	BlastFrameWork();
	VHACD::IVHACD* myDecompoeser;
    Nv::Blast::ConvexMeshBuilder* myConvexHullGenerator;
	Nv::Blast::BlastBondGenerator* myBondGenerator;
};

