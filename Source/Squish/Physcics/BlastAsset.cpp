#include "BlastAsset.h"
#include "globals/NvBlastGlobals.h"
#include "globals/NvBlastAllocator.h"
#include "extensions/authoring/NvBlastExtAuthoring.h"
#include "extensions/authoring/NvBlastExtAuthoringMeshCleaner.h"
#include "extensions/authoringCommon/NvBlastExtAuthoringMesh.h"
#include "extensions/authoring/NvBlastExtAuthoringFractureTool.h"
#include "toolkit/NvBlastTk.h"
#include "toolkit/NvBlastTkAsset.h"
#include "toolkit/NvBlastTkActor.h"
#include "extensions/PxTriangleMeshExt.h"
#include "extensions/PxConvexMeshExt.h"

#include <vector>

using namespace Nv::Blast;

BlastAsset::BlastAsset()
{
    std::vector<NvBlastChunkDesc> ChunkDescs;
    ChunkDescs.resize(3);
    ChunkDescs[0].parentChunkDescIndex = UINT32_MAX;
    ChunkDescs[0].centroid[0] = 0.0f;
    ChunkDescs[0].centroid[1] = 0.0f;
    ChunkDescs[0].centroid[2] = 0.0f;
    ChunkDescs[0].volume = 1.0f;
    ChunkDescs[0].flags = NvBlastChunkDesc::Flags::NoFlags;
    ChunkDescs[0].userData = 0;

    ChunkDescs[2].parentChunkDescIndex = 0;
    ChunkDescs[1].centroid[0] = 2.0f;
    ChunkDescs[1].centroid[1] = 4.0f;
    ChunkDescs[1].centroid[2] = 6.0f;
    ChunkDescs[1].volume = 1.0f;
    ChunkDescs[1].flags = NvBlastChunkDesc::Flags::SupportFlag;
    ChunkDescs[1].userData = 1;

    ChunkDescs[2].parentChunkDescIndex = 0;
    ChunkDescs[2].centroid[0] = -2.0f;
    ChunkDescs[2].centroid[1] = 4.0f;
    ChunkDescs[2].centroid[2] = 6.0f;
    ChunkDescs[2].volume = 1.0f;
    ChunkDescs[2].flags = NvBlastChunkDesc::Flags::SupportFlag;
    ChunkDescs[2].userData = 2;

    std::vector<NvBlastBondDesc> BondDescs;
    BondDescs.resize(2);
    BondDescs[0].chunkIndices[0] = 1;
    BondDescs[0].chunkIndices[1] = 2;
    BondDescs[0].bond.centroid[0] = 0.f;
    BondDescs[0].bond.centroid[1] = 4.f;
    BondDescs[0].bond.centroid[2] = 6.f;
    BondDescs[0].bond.normal[0] = 1;
    BondDescs[0].bond.normal[1] = 0;
    BondDescs[0].bond.normal[2] = 0;
    BondDescs[0].bond.area = 1.0f;
    BondDescs[0].bond.userData = 0;

    BondDescs[1].chunkIndices[0] = 1;
    BondDescs[1].chunkIndices[1] = ~0;
    BondDescs[1].bond.centroid[0] = 0.f;
    BondDescs[1].bond.centroid[1] = 4.f;
    BondDescs[1].bond.centroid[2] = 6.f;
    BondDescs[1].bond.normal[0] = 1;
    BondDescs[1].bond.normal[1] = 0;
    BondDescs[1].bond.normal[2] = 0;
    BondDescs[1].bond.area = 1.0f;
    BondDescs[1].bond.userData = 1;

    NvBlastAssetDesc desc;
    desc.bondCount = 2;
    desc.bondDescs = BondDescs.data();
    desc.chunkCount = 3;
    desc.chunkDescs = ChunkDescs.data();

    std::vector<char> scratch(3*sizeof(NvBlastChunkDesc));
    NvBlastEnsureAssetExactSupportCoverage(ChunkDescs.data(), 3, scratch.data(), BlastLog);
    std::vector<uint32_t> map(3);
    NvBlastReorderAssetDescChunks(ChunkDescs.data(), 3, BondDescs.data(), 2, map.data(), true, scratch.data(), BlastLog);
    scratch.resize(NvBlastGetRequiredScratchForCreateAsset(&desc, BlastLog));

    void* mem = malloc(NvBlastGetAssetMemorySize(&desc, BlastLog));

    myAsset =  NvBlastCreateAsset(mem, &desc, scratch.data(), BlastLog);

    AllocateFamily();

    CreateActor();
    MeshCleaner* myMeshCleaner = NvBlastExtAuthoringCreateMeshCleaner();
    
    Mesh* mesh = NvBlastExtAuthoringCreateMeshOnlyTriangles()
}

BlastAsset::BlastAsset(const BlastAsset& aAsset)
{
    uint32_t size = NvBlastAssetGetSize(aAsset.myAsset, BlastLog);
    myAsset = (NvBlastAsset*)malloc(size);
    memcpy(myAsset, aAsset.myAsset, size);
    
}

BlastAsset::~BlastAsset()
{
    free(myFamily);
    free(myAsset);
}

void BlastAsset::Hit()
{
    NvBlastGraphShaderActor Graph;
    NvBlastSubgraphShaderActor Subgraph;

    NvBlastActorGenerateFracture()
}

void BlastAsset::AllocateFamily()
{
    void* mem = malloc(NvBlastAssetGetFamilyMemorySize(myAsset, BlastLog));

    myFamily = NvBlastAssetCreateFamily(mem, myAsset, BlastLog);
}

void BlastAsset::CreateActor()
{
    NvBlastActorDesc actorDesc;
    actorDesc.uniformInitialBondHealth = 1;
    actorDesc.uniformInitialLowerSupportChunkHealth = 1;
    std::vector<char> scratch( NvBlastFamilyGetRequiredScratchForCreateFirstActor(myFamily, BlastLog));

    myFirstActor = NvBlastFamilyCreateFirstActor(myFamily, &actorDesc, scratch.data(), BlastLog);
    
}

void BlastLog(int type, const char* msg, const char* file, int line)
{
    printf("Error: %d, %s in file %s at line %d",type, msg, file, line);
    throw("Error: %d, %s in file %s at line %d",type, msg, file, line);
}
