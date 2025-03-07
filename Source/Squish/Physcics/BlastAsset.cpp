#include "BlastAsset.h"
#include "globals/NvBlastGlobals.h"
#include "globals/NvBlastAllocator.h"
#include "extensions/authoring/NvBlastExtAuthoring.h"
#include "extensions/authoring/NvBlastExtAuthoringMeshCleaner.h"
#include "extensions/authoringCommon/NvBlastExtAuthoringMesh.h"
#include "extensions/authoring/NvBlastExtAuthoringFractureTool.h"
#include "extensions/authoringCommon/NvBlastExtAuthoringConvexMeshBuilder.h"

#include "extensions/authoring/NvBlastExtAuthoringBondGenerator.h"

#include "toolkit/NvBlastTk.h"
#include "toolkit/NvBlastTkAsset.h"
#include "toolkit/NvBlastTkActor.h"
#include "extensions/PxTriangleMeshExt.h"
#include "extensions/authoring/NvBlastExtAuthoring.h"

#include "extensions/PxConvexMeshExt.h"

#include <vector>
#include <extensions/assetutils/NvBlastExtAssetUtils.h>
#include <lowlevel/NvBlastTypes.h>
#include <shared/NvFoundation/NvCTypes.h>


#include "BlastFrameWork.h"
#include "ConvexHullGenerator.h"
#include "NvBlast.h"
#include "PhysicsEngine.h"
#include "PhysXRandomNumberGenerator.h"
#include "RigidBody.h"
#include "cooking/PxCooking.h"
#include "geometry/PxGeometryInternal.h"
#include "shaders/NvBlastExtDamageShaders.h"
#include "NvCMath.h"

using namespace Nv::Blast;

BlastAsset::BlastAsset()
{
	myAuthoringResult = new AuthoringResult();
}

BlastAsset::BlastAsset(const BlastAsset& aAsset)
{
    /*uint32_t size = NvBlastAssetGetSize(aAsset.myAsset, BlastLog);
    myAsset = (NvBlastAsset*)malloc(size);
    memcpy(myAsset, aAsset.myAsset, size);*/
   delete myAuthoringResult;
}

BlastAsset::~BlastAsset()
{
    myFractureTool->release();
    myAsset->release();
    myActor->release();
    free(myAsset);
}

void BlastAsset::Hit()
{
    NvBlastExtRadialDamageDesc desc;
    desc.damage = 100;
    desc.position[0] = 0;
    desc.position[1] = 5;
    desc.position[2] = 0;
    desc.minRadius = 0.1f;
    desc.maxRadius = 10.f;
    
    NvBlastDamageProgram dmgProgram;
    NvBlastExtProgramParams params(&desc);
    //dmgProgram.graphShaderFunction = GraphShader(),
    myActor->damage(dmgProgram, &params);
    TkGroupDesc group;
    group.workerCount = 1;
    auto actorgroup = NvBlastTkFrameworkGet()->createGroup(group);
    actorgroup->addActor(*myActor);
    actorgroup->process();
    actorgroup->release();
    
}

bool BlastAsset::CreateAsset(const std::vector<CommonUtilities::Vector3f>& aPosData,
                            const std::vector<CommonUtilities::Vector3f>& aNormData, const std::vector<CommonUtilities::Vector2f>& aUvData, const
                             std::vector<uint32_t>& aIndicies, unsigned aNrOfPieces)
{
    BlastMesh mesh;
    for(size_t i = 0; i<aPosData.size(); i++)
    {
        mesh.pos.push_back({aPosData[i].x, aPosData[i].y, aPosData[i].z});
        mesh.norm.push_back({aNormData[i].x, aNormData[i].y, aNormData[i].z});
        mesh.uv.push_back({aUvData[i].x, aUvData[i].y});
    }
    MeshCleaner* cleaner = NvBlastExtAuthoringCreateMeshCleaner();
    Mesh* aMesh = NvBlastExtAuthoringCreateMesh(mesh.pos.data(), mesh.norm.data(), mesh.uv.data(),aPosData.size(), aIndicies.data(), aIndicies.size());
    
    aMesh = cleaner->cleanMesh(aMesh);
    myFractureTool = NvBlastExtAuthoringCreateFractureTool();
    myFractureTool->setSourceMeshes(&aMesh,1);
   
    Mesh* donemesh = myFractureTool->createChunkMesh(0);
    RandomGeneratorBase* rng = new PhysxRNG();
    auto voregen =  NvBlastExtAuthoringCreateVoronoiSitesGenerator(aMesh, rng);
    
    const NvcVec3* points = new NvcVec3[aNrOfPieces];
    //NvBlastExtAuthoringBuildMeshConvexDecomposition();
    voregen->uniformlyGenerateSitesInMesh(aNrOfPieces);
    voregen->getVoronoiSites(points);
    /*
    const NvcVec3* sitespoints;;
    uint32_t sites =  voregen->getVoronoiSites(sitespoints);*/
    int32_t fractRes = myFractureTool->voronoiFracturing(0, aNrOfPieces, points, false);
    if(fractRes != 0)
    {
	    printf("Error fracturing failed with code %d\n", fractRes);
    }
    myFractureTool->finalizeFracturing();
    Triangle* tri;
    myFractureTool->getBaseMesh(0, tri);
    if(tri==nullptr)
    {
	    printf("Error tri is nullptr\n");
    }

    voregen->release();
    
    ConvexDecompositionParams convexDecompositionParams;
    //AuthoringResult* result = NvBlastExtAuthoringProcessFracture(*myFractureTool, *bondGenerator, myConvexHullGenerator, convexDecompositionParams);
	FinalizeAuthoring();
  
    return myActor!=nullptr;
}

BlastMesh BlastAsset::GetRenderData()
{
	BlastMesh retVal;
    for(uint32_t i = 0; i<myFractureTool->getChunkCount(); i++)
    {
            //Mesh* donemesh = myFractureTool->createChunkMesh(i);
			Triangle* mesh;
			uint32_t count = myFractureTool->getBaseMesh((int32_t)i, mesh);
			for(uint32_t j = 0; j<count; j++)
			{
				retVal.pos.push_back(mesh[j].a.p);
				retVal.norm.push_back(mesh[j].a.n);
				retVal.uv.push_back(mesh[j].a.uv[0]);
                retVal.indicies.push_back(retVal.indicies.size());
				retVal.pos.push_back(mesh[j].b.p);
				retVal.norm.push_back(mesh[j].b.n);
				retVal.uv.push_back(mesh[j].b.uv[0]);
                retVal.indicies.push_back(retVal.indicies.size());
				retVal.pos.push_back(mesh[j].b.p);
				retVal.norm.push_back(mesh[j].b.n);
				retVal.uv.push_back(mesh[j].b.uv[0]);
                retVal.indicies.push_back(retVal.indicies.size());

			}
    		/*const Vertex* vertexArray = donemesh->getVertices();
            for(uint32_t j = 0; j<donemesh->getVerticesCount(); j++)
            {
                
	            retVal.pos.push_back(vertexArray[j].p);
	            retVal.norm.push_back(vertexArray[j].p);
	            retVal.uv.push_back(vertexArray[j].uv[0]);
                retVal.indicies.push_back(retVal.indicies.size());
            }*/
    }
    return retVal;
}

void BlastAsset::FinalizeAuthoring(int32_t defaultSupportDepth)
{
    
    const uint32_t chunkCount = myFractureTool->getChunkCount();
    if (chunkCount == 0)
    {
        printf("Error no chunks\n");
    }
    
    AuthoringResult& aResult = *myAuthoringResult;
    aResult.chunkCount = chunkCount;

    std::shared_ptr<bool> isSupport(new bool[chunkCount], [](bool* b) {delete[] b; });
    memset(isSupport.get(), 0, sizeof(bool) * chunkCount);
    for (uint32_t i = 0; i < myFractureTool->getChunkCount(); ++i)
    {
        if (defaultSupportDepth < 0 || myFractureTool->getChunkDepth(myFractureTool->getChunkId(i)) < defaultSupportDepth)
        {
            isSupport.get()[i] = myFractureTool->getChunkInfo(i).isLeaf;
        }
        else if (myFractureTool->getChunkDepth(myFractureTool->getChunkId(i)) == defaultSupportDepth)
        {
            isSupport.get()[i] = true;
        }
    }
    // Generate bonds and chunkmap inverse
    ConvexHullGenerator gen;
    BlastBondGenerator* bondGenerator =  NvBlastExtAuthoringCreateBondGenerator(&gen);
    ConvexDecompositionParams convexDecompositionParams;
    const uint32_t bondCount = bondGenerator->buildDescFromInternalFracture(myFractureTool, isSupport.get(), myAuthoringResult->bondDescs, myAuthoringResult->chunkDescs);
    aResult.bondCount = bondCount;
    if (bondCount == 0)
    {
        aResult.bondDescs = nullptr;
    }
    std::vector<uint32_t> chunkReorderInvMap; //Order chunks, build map
    {
        std::vector<uint32_t> chunkReorderMap(chunkCount);
        std::vector<char> scratch(chunkCount * sizeof(NvBlastChunkDesc));
        NvBlastEnsureAssetExactSupportCoverage(aResult.chunkDescs, chunkCount, scratch.data(), logLL);
        NvBlastBuildAssetDescChunkReorderMap(chunkReorderMap.data(), aResult.chunkDescs, chunkCount, scratch.data(), logLL);
        NvBlastApplyAssetDescChunkReorderMapInPlace(aResult.chunkDescs, chunkCount, aResult.bondDescs, bondCount, chunkReorderMap.data(), true, scratch.data(), logLL);
        chunkReorderInvMap.resize(chunkReorderMap.size());
    	chunkReorderInvMap.assign(chunkReorderMap.begin(), chunkReorderMap.end());
        std::reverse(chunkReorderInvMap.begin(), chunkReorderInvMap.end());
    }
    //get Geometry

    aResult.geometryOffset = SAFE_ARRAY_NEW(uint32_t, chunkCount + 1);
    aResult.assetToFractureChunkIdMap = SAFE_ARRAY_NEW(uint32_t, chunkCount + 1);
    aResult.geometryOffset[0] = 0;
    std::vector<Nv::Blast::Triangle*> chunkGeometry(chunkCount);
    for (uint32_t i = 0; i < chunkCount; ++i)
    {
        uint32_t chunkInfoIndex = chunkReorderInvMap[i];
        aResult.geometryOffset[i+1] = aResult.geometryOffset[i] + myFractureTool->getBaseMesh(chunkInfoIndex, chunkGeometry[i]);
        aResult.assetToFractureChunkIdMap[i] = myFractureTool->getChunkId(chunkInfoIndex);
    }
    aResult.geometry = SAFE_ARRAY_NEW(Triangle, aResult.geometryOffset[chunkCount]);
    for (uint32_t i = 0; i < chunkCount; ++i)
    {
        uint32_t trianglesCount = aResult.geometryOffset[i + 1] - aResult.geometryOffset[i];
        memcpy(aResult.geometry + aResult.geometryOffset[i], chunkGeometry[i], trianglesCount * sizeof(Nv::Blast::Triangle));
        delete chunkGeometry[i];
        chunkGeometry[i] = nullptr;
    }
    //Calculate bond desc
     float maxX = FLT_MAX;
    float maxY = FLT_MAX;
    float maxZ = FLT_MAX;

    float minX = -FLT_MAX;
    float minY = -FLT_MAX;
    float minZ = -FLT_MAX;

    for (uint32_t i = 0; i < bondCount; i++)
    {
        NvBlastBondDesc& bondDesc = aResult.bondDescs[i];

        minX = std::min(minX, bondDesc.bond.centroid[0]);
        maxX = std::max(maxX, bondDesc.bond.centroid[0]);

        minY = std::min(minY, bondDesc.bond.centroid[1]);
        maxY = std::max(maxY, bondDesc.bond.centroid[1]);

        minZ = std::min(minZ, bondDesc.bond.centroid[2]);
        maxZ = std::max(maxZ, bondDesc.bond.centroid[2]);
    }

    BlastFrameWork::GetInstance().buildPhysxChunk(*myAuthoringResult, convexDecompositionParams);

    // set NvBlastChunk volume and centroid from CollisionHull
    for (uint32_t i = 0; i < chunkCount; i++)
    {
        float totalVolume = 0.f;
        NvcVec3 totalCentroid = {0.0f, 0.0f, 0.0f};
        for (uint32_t k = aResult.collisionHullOffset[i]; k < aResult.collisionHullOffset[i+1]; k++)
        {
            const CollisionHull* hull = aResult.collisionHull[k];
            if (hull)
            {
                NvcVec3 centroid;
                
                const float volume = BlastFrameWork::CalculateCollisionVolumeAndCentroid(centroid, *hull);
                totalVolume += volume;
                totalCentroid = totalCentroid + volume*centroid;
            }
            else
            {
                totalVolume = 0.0f; // Found a null hull, signal this with zero volume
                break;
            }
        }
        if (totalVolume > 0.0f)
        {
            totalCentroid = totalCentroid / totalVolume;
            aResult.chunkDescs[i].volume = totalVolume;
            aResult.chunkDescs[i].centroid[0] = totalCentroid.x;
            aResult.chunkDescs[i].centroid[1] = totalCentroid.y;
            aResult.chunkDescs[i].centroid[2] = totalCentroid.z;
        }
        else
        {
            // Fallback to using mesh
            size_t triCount = aResult.geometryOffset[i+1] - aResult.geometryOffset[i];
            const Nv::Blast::Triangle* tris = aResult.geometry + aResult.geometryOffset[i];
            NvcVec3 centroid;
            aResult.chunkDescs[i].volume = BlastFrameWork::CalculateGeometryVolumeAndCentroid(centroid, tris, triCount);
            aResult.chunkDescs[i].centroid[0] = centroid.x;
            aResult.chunkDescs[i].centroid[1] = centroid.y;
            aResult.chunkDescs[i].centroid[2] = centroid.z;
        }
        NvBlastAssetDesc desc;
		desc.bondDescs = aResult.bondDescs;
		desc.bondCount = aResult.bondCount;
		desc.chunkDescs = aResult.chunkDescs;
		desc.chunkCount = aResult.chunkCount;

        std::vector<uint8_t> scratch(static_cast<unsigned int>(NvBlastGetRequiredScratchForCreateAsset(&desc, logLL)));
		void* mem = NVBLAST_ALLOC(NvBlastGetAssetMemorySize(&desc, logLL));
		aResult.asset = NvBlastCreateAsset(mem, &desc,scratch.data(), logLL);

        aResult.materialCount = 0;
		aResult.materialNames = nullptr;
    }

    TkActorDesc desc;
    desc.uniformInitialBondHealth = 100;
    desc.uniformInitialLowerSupportChunkHealth = 100;
    desc.asset = NvBlastTkFrameworkGet()->createAsset(myAuthoringResult->asset, nullptr, 0, true);
    bondGenerator->release();
    myActor = NvBlastTkFrameworkGet()->createActor(desc);
}

physx::PxConvexMeshGeometry BlastAsset::CreateChunkShape()
{
    return NULL;
}


void BlastLog(int type, const char* msg, const char* file, int line)
{
    printf("Error: %d, %s in file %s at line %d",type, msg, file, line);
    throw("Error: %d, %s in file %s at line %d",type, msg, file, line);
}
