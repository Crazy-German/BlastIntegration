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
#include "extensions/PxConvexMeshExt.h"

#include <vector>
#include <extensions/assetutils/NvBlastExtAssetUtils.h>
#include <lowlevel/NvBlastTypes.h>
#include <shared/NvFoundation/NvCTypes.h>
#include <extensions/stress/NvBlastExtStressSolver.h>



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
#include "PhysXScene.h"
#include "PxFixedJoint.h"
#include "PxPhysics.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

using namespace Nv::Blast;
using namespace physx;
using namespace Squish;

uint32_t GLOBALINDEX;

BlastAsset::BlastAsset()
{
	myAuthoringResult = new AuthoringResult();
    myTransform = new TransformBase();
}

BlastAsset::BlastAsset(const BlastAsset& aAsset)
{
    aAsset;
    /*uint32_t size = NvBlastAssetGetSize(aAsset.myAsset, BlastLog);
    myAsset = (NvBlastAsset*)malloc(size);
    memcpy(myAsset, aAsset.myAsset, size);*/
   delete myAuthoringResult;
}

BlastAsset::~BlastAsset()
{
    myFractureTool->release();
    for(const auto& actor : myBlastActors)
    {
	    actor.second->release();
    }
    myBlastActors.clear();
    delete myTransform;
}

void BlastAsset::Hit(physx::PxVec3 aWorldPosition, float aDamageVal, float aMinRadius, float aMaxRadius, uint32_t aIndex, physx::PxVec3 aImpulse)
{
    TkGroupDesc group;
    group.workerCount = 1;
    auto actorgroup = NvBlastTkFrameworkGet()->createGroup(group);

    TkActor* actor = myBlastActors.at(aIndex);
    PxRigidDynamic* pxActor = static_cast<PxRigidDynamic*>(actor->userData);
    
    PxTransform transform = pxActor->getGlobalPose();
    physx::PxVec3 pos = transform.transformInv(aWorldPosition);//ToPhysXTransform(*myTransform).transformInv(aWorldPosition);
    NvBlastExtRadialDamageDesc desc;
    desc.damage = aDamageVal;
    desc.position[0] = pos.x;
    desc.position[1] = pos.y;
    desc.position[2] = pos.z;
    desc.minRadius = aMinRadius;
    desc.maxRadius = aMaxRadius;
    NvBlastDamageProgram dmgProgram {
        NvBlastExtFalloffGraphShader,
		
        nullptr
    };
    NvBlastExtProgramParams	params(&desc);
    //dmgProgram.graphShaderFunction = GraphShader(),
    /*uint32_t nodeCount = myActor->getGraphNodeCount();
    if(nodeCount>1)
    {
	    printf("More than one graph node, nr of nodes %d\n", nodeCount);
    }*/
    actorgroup->addActor(*actor);
	/*if (myActor->getGroup() == nullptr)
	{
		printf("Actor has no group\n");
	}*/
    actor->damage(dmgProgram, &params);
    

    actorgroup->process();
	actorgroup->release();
}

bool BlastAsset::CreateAsset(const std::vector<CommonUtilities::Vector3f>& aPosData, const std::vector<CommonUtilities::Vector3f>& aNormData, const std::vector<CommonUtilities::Vector2f>& aUvData, 
							 const std::vector<uint32_t>& aIndicies, unsigned aNrOfPieces)
{
    std::vector<NvcVec3> pos;
    std::vector<NvcVec3> norm;
    std::vector<NvcVec2> uv;
    for(size_t i = 0; i<aPosData.size(); i++)
    {
        pos.push_back({aPosData[i].x, aPosData[i].y, aPosData[i].z});
        norm.push_back({aNormData[i].x, aNormData[i].y, aNormData[i].z});
        uv.push_back({aUvData[i].x, aUvData[i].y});
    }
    MeshCleaner* cleaner = NvBlastExtAuthoringCreateMeshCleaner();
    Nv::Blast::Mesh* aMesh = NvBlastExtAuthoringCreateMesh(pos.data(), norm.data(), uv.data(),aPosData.size(), aIndicies.data(), aIndicies.size());
    
    aMesh = cleaner->cleanMesh(aMesh);
    myFractureTool = NvBlastExtAuthoringCreateFractureTool();
    myFractureTool->setSourceMeshes(&aMesh,1);
   
    Nv::Blast::Mesh* donemesh = myFractureTool->createChunkMesh(0);
    
    RandomGeneratorBase* rng = new PhysxRNG();
    auto voregen =  NvBlastExtAuthoringCreateVoronoiSitesGenerator(aMesh, rng);
    
    const NvcVec3* points = new NvcVec3[aNrOfPieces];
    //NvBlastExtAuthoringBuildMeshConvexDecomposition();
    voregen->uniformlyGenerateSitesInMesh(aNrOfPieces);
    voregen->getVoronoiSites(points);

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
    
	FinalizeAuthoring();
	myAsset = myActor->getAsset();
    myBlastActors.emplace(myActor->getIndex(),myActor);
    return myActor!=nullptr;
}

BlastMesh BlastAsset::GetRenderData()
{
	BlastMesh retVal;
    for(const auto& [index, actor ]: myBlastActors)
    {
	    uint32_t chunkCount = actor->getVisibleChunkCount();
        uint32_t* indicies = new uint32_t[chunkCount];
        actor->getVisibleChunkIndices(indicies,chunkCount);//Get chunks associated with actor
		PxTransform transform = static_cast<PxRigidDynamic*>(actor->userData)->getGlobalPose();
	    for(uint32_t i = 0; i<chunkCount; i++)
	    {
	            //Mesh* donemesh = myFractureTool->createChunkMesh(i);
			Triangle* mesh;
			uint32_t count = myFractureTool->getBaseMesh((int32_t)indicies[i], mesh);
			for(uint32_t j = 0; j<count; j++)
			{
                PxVec3 pos(mesh[j].a.p.x, mesh[j].a.p.y, mesh[j].a.p.z);
                PxVec3 norm(mesh[j].a.n.x, mesh[j].a.n.y, mesh[j].a.n.z);
                pos = transform.transform(pos);
                norm = transform.transform(norm);
                retVal.pos.emplace_back(ToVector3f(pos));
				retVal.norm.emplace_back(ToVector3f(norm));
                retVal.uv.emplace_back(mesh[j].a.uv[0].x, mesh[j].a.uv[0].y);
				retVal.indicies.emplace_back(retVal.indicies.size());

                pos = {mesh[j].b.p.x, mesh[j].b.p.y, mesh[j].b.p.z};
                norm = {mesh[j].b.n.x, mesh[j].b.n.y, mesh[j].b.n.z};
                pos = transform.transform(pos);
                norm = transform.transform(norm);
                retVal.pos.emplace_back(ToVector3f(pos));
				retVal.norm.emplace_back(ToVector3f(norm));
                retVal.uv.emplace_back(mesh[j].b.uv[0].x, mesh[j].b.uv[0].y);
                retVal.indicies.emplace_back(retVal.indicies.size());

                pos = {mesh[j].c.p.x, mesh[j].c.p.y, mesh[j].c.p.z};
                norm = {mesh[j].c.n.x, mesh[j].c.n.y, mesh[j].c.n.z};
                pos = transform.transform(pos);
                norm = transform.transform(norm);
                retVal.pos.emplace_back(ToVector3f(pos));
				retVal.norm.emplace_back(ToVector3f(norm));
                retVal.uv.emplace_back(mesh[j].c.uv[0].x, mesh[j].c.uv[0].y);
                retVal.indicies.emplace_back(retVal.indicies.size());
			}
	    }
    }
    return retVal;
}

Nv::Blast::TkActor* BlastAsset::GetActor()
{
	return myActor;
}

void BlastAsset::receive(const Nv::Blast::TkEvent* events, uint32_t eventCount)
{
    const Nv::Blast::TkJointUpdateEvent* jointEvent;
	const Nv::Blast::TkSplitEvent* split;
	const Nv::Blast::TkFractureCommands* fractureCommands;

	for(uint32_t i = 0; i<eventCount; i++)
	{
		const Nv::Blast::TkEvent& event = events[i];
		switch (event.type)
		{
		case Nv::Blast::TkEvent::Split:
			split = event.getPayload<Nv::Blast::TkSplitEvent>();
			Split(*split);
			break;
		case Nv::Blast::TkEvent::FractureCommand:
			fractureCommands = event.getPayload<Nv::Blast::TkFractureCommands>();
            //Squish::PhysicsEngine::Get()->GetScene()->RemoveActor(myPhysXActors.at(fractureCommands->tkActorData.index));
           /* for(uint32_t bondFract = 0; bondFract < fractureCommands->buffers.bondFractureCount; bondFract++);
			{
				
			}*/
            for(uint32_t chunkIndex = 0; chunkIndex < fractureCommands->buffers.chunkFractureCount; chunkIndex++)
            {
				uint32_t chunkAssetIndex = fractureCommands->buffers.chunkFractures[chunkIndex].chunkIndex;
	            printf("ChunkAssetIndex %d\n", chunkAssetIndex);
            }
            
			break;
		case Nv::Blast::TkEvent::FractureEvent:
			//event.getPayload<Nv::Blast::TkFractureEvents>()->EVENT_TYPE;
			break;
		case Nv::Blast::TkEvent::JointUpdate:
        {      // Joint events have three subtypes, see which one we have
				jointEvent = event.getPayload<Nv::Blast::TkJointUpdateEvent>();  // Joint update event payload
                switch (jointEvent->subtype)
                {
	                case Nv::Blast::TkJointUpdateEvent::External:
					{ // An internal joint has been "exposed" (now joins two different actors).  Create a physics joint.
	                    PxRigidActor* actor0 = static_cast<PxRigidActor*>(jointEvent->joint->getData().actors[0]->userData);
	                    PxRigidActor* actor1 = static_cast<PxRigidActor*>(jointEvent->joint->getData().actors[1]->userData);
	                        const TkJointData& data = jointEvent->joint->getData();
                        jointEvent->joint->userData = BlastFrameWork::GetInstance().CreatePhysxJoint(jointEvent->joint->getData().attachPositions, actor0, actor1);
	                    if(jointEvent->joint->userData == nullptr)
	                    {
		                    printf("Failed to create physx Joint\n");
	                        __debugbreak();
	                    }
	                    break;
	                }
	                case Nv::Blast::TkJointUpdateEvent::Changed:
	                {  // A joint's actors have changed, so we need to update its corresponding physics joint.
	                    if(jointEvent->joint->userData != nullptr)
	                    {
		                    PxRigidActor* pxActors[2];
	                        const TkJointData& data = jointEvent->joint->getData();
	                        for(int j = 0; j<2;j++)
	                        {
		                        if(data.actors[j] != nullptr)
		                        {
			                        pxActors[j] = static_cast<PxRigidActor*>(data.actors[j]->userData);
		                        }
	                            else
	                            {
		                            pxActors[j] = nullptr;
	                            }
	                        }
							PxFixedJoint* joint =  static_cast<PxFixedJoint*>(jointEvent->joint->userData);
							joint->setActors(pxActors[0] != nullptr ? pxActors[0] : nullptr, pxActors[1] != nullptr ? pxActors[1] : nullptr);
	                        
	                    }
	                    else // No physx joint existed create one
	                    {
		                    PxRigidActor* actor0 = static_cast<PxRigidActor*>(jointEvent->joint->getData().actors[0]->userData);
		                    PxRigidActor* actor1 = static_cast<PxRigidActor*>(jointEvent->joint->getData().actors[1]->userData);

		                    jointEvent->joint->userData = BlastFrameWork::GetInstance().CreatePhysxJoint(jointEvent->joint->getData().attachPositions, actor0, actor1);
                            if(jointEvent->joint->userData == nullptr)
		                    {
			                    printf("Failed to create physx Joint\n");
		                        __debugbreak();
		                    }
	                    }
	                    break;
	                }
	                case Nv::Blast::TkJointUpdateEvent::Unreferenced:
	                {   // This joint is no longer referenced, so we may delete the corresponding physics joint.
	                    if(jointEvent->joint->userData != nullptr)
	                    {
		                    static_cast<PxFixedJoint*>(jointEvent->joint->userData)->release();
	                        jointEvent->joint->userData = nullptr;
	                    }
	                    break;
	                }
				}
			break;
        }
		case Nv::Blast::TkEvent::TypeCount:
			break;
		}
	}
}

void BlastAsset::SetPosition(const CommonUtilities::Vector3f aPos)
{
    myTransform->SetPosition(aPos);
    myTransform->RecalculateCache();
    for(auto& actor : myPhysXActors)
    {
	    actor.second->setGlobalPose(Squish::ToPhysXTransform(*myTransform));
    }
}

void BlastAsset::FinalizeAuthoring(int32_t defaultSupportDepth)
{
    
    const uint32_t chunkCount = myFractureTool->getChunkCount();
    if (chunkCount == 0)
    {
        printf("Error no chunks\n");
    }
    
    AuthoringResult& aResult = *myAuthoringResult;
    TkAssetDesc AssetDesc;
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
    convexDecompositionParams.voxelGridResolution = 10000;
    convexDecompositionParams.maximumNumberOfHulls = 8;
    convexDecompositionParams.maximumNumberOfVerticesPerHull = 128;
    const uint32_t bondCount = bondGenerator->buildDescFromInternalFracture(myFractureTool, isSupport.get(), aResult.bondDescs, aResult.chunkDescs);
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
    	for(uint32_t i = 0; i<chunkReorderMap.size(); ++i)
    	{
    		chunkReorderInvMap[chunkReorderMap[i]] = i;
    	}
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

        minX = min(minX, bondDesc.bond.centroid[0]);
        maxX = max(maxX, bondDesc.bond.centroid[0]);

        minY = min(minY, bondDesc.bond.centroid[1]);
        maxY = max(maxY, bondDesc.bond.centroid[1]);

        minZ = min(minZ, bondDesc.bond.centroid[2]);
        maxZ = max(maxZ, bondDesc.bond.centroid[2]);
    }
	
	myGeometryData = new GeometryData*[chunkCount];
    BlastFrameWork::GetInstance().buildPhysxChunk(*myAuthoringResult, convexDecompositionParams, myGeometryData);

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

        myGeometryData[i]->myCenter.x = aResult.chunkDescs[i].centroid[0];
        myGeometryData[i]->myCenter.y = aResult.chunkDescs[i].centroid[1];
        myGeometryData[i]->myCenter.z = aResult.chunkDescs[i].centroid[2];
    }

    TkActorDesc desc;
    desc.uniformInitialBondHealth = 100;
    desc.uniformInitialLowerSupportChunkHealth = 100;

    uint8_t* flags = new uint8_t[myAuthoringResult->bondCount];
    for(uint32_t i = 0; i<myAuthoringResult->bondCount; i++)
    {
       if(10.f <= aResult.bondDescs[i].bond.area)
       {
       		flags[i] |= TkAssetDesc::BondJointed;        
			printf("Internal Joint %d and %d created, area %f\n", aResult.bondDescs[i].chunkIndices[0], aResult.bondDescs[i].chunkIndices[1], aResult.bondDescs->bond.area);
       }
       else
       {
			printf("Bond area to small: %f\n", aResult.bondDescs[i].bond.area);
       }
    }
    AssetDesc.bondFlags = flags;
    AssetDesc.bondDescs = aResult.bondDescs;
    AssetDesc.bondCount = aResult.bondCount;
    AssetDesc.chunkCount = aResult.chunkCount;
    AssetDesc.chunkDescs = aResult.chunkDescs;

    myAsset = NvBlastTkFrameworkGet()->createAsset(AssetDesc);
    //desc.asset = NvBlastTkFrameworkGet()->createAsset(myAuthoringResult->asset, nullptr, 0,true);
    desc.asset = myAsset;
    bondGenerator->release();
    myActor = NvBlastTkFrameworkGet()->createActor(desc);
   
	myActor->getFamily().addListener(*this);

	myAsset = myActor->getAsset();
    myIndex = GLOBALINDEX++;
	myPhysXActors.emplace(myActor->getIndex(), BlastFrameWork::GetInstance().CreateRigidActorFromGeometry(myGeometryData[0]->myGeometry, myGeometryData[0]->myGeometryCount, NvcVec3(0,0,0), NvcQuat(0,0,0,1), &myIndex));
    myActor->userData = myPhysXActors.at(myActor->getIndex());
	//myPhysXActors.at(0)->setName((std::string("Chunk ")+std::to_string(0)).c_str());
	Squish::PhysicsEngine::Get()->GetScene()->AddActor(myPhysXActors.at(myActor->getIndex()));
    myPhysXActors.at(myActor->getIndex())->setGlobalPose(Squish::ToPhysXTransform(*myTransform));
    
	
}

void BlastAsset::Split(const Nv::Blast::TkSplitEvent& aSplit)
{
    Squish::PhysicsEngine::Get()->GetScene()->RemoveActor(myPhysXActors.at(aSplit.parentData.index));
	*myTransform = ToTransformbase(myPhysXActors.at(aSplit.parentData.index)->getGlobalPose());
    PxRigidDynamic* temp = myPhysXActors.at(aSplit.parentData.index)->is<PxRigidDynamic>();
    myPhysXActors.erase(aSplit.parentData.index);
    myBlastActors.erase(aSplit.parentData.index);
	for (uint32_t i = 0; i < aSplit.numChildren; i++)
	{
        uint32_t visableChunks = aSplit.children[i]->getVisibleChunkCount();
		uint32_t* chunks = new uint32_t[visableChunks];
        TkActor* child = aSplit.children[i];
        uint32_t index = child->getIndex();
		child->getVisibleChunkIndices(chunks, visableChunks);
        if(0<child->getJointCount())
        {
	        printf("joints %d\n", child->getJointCount());
        }
        myPhysXActors.emplace(index, BlastFrameWork::GetInstance().CreateRigidActorFromGeometry(myGeometryData, chunks, visableChunks,NvcVec3(myTransform->GetPosition().x,myTransform->GetPosition().y,myTransform->GetPosition().z), NvcQuat(myTransform->GetRotationQuaternion().x,myTransform->GetRotationQuaternion().y,myTransform->GetRotationQuaternion().z,myTransform->GetRotationQuaternion().w), nullptr));
        child->userData = myPhysXActors.at(index);
		//myPhysXActors.at(index)->setGlobalPose(temp->getGlobalPose());
        myBlastActors.emplace(index, child);
        Squish::PhysicsEngine::Get()->GetScene()->AddActor(myPhysXActors.at(index));
        delete[] chunks;
	}
}


void BlastLog(int type, const char* msg, const char* file, int line)
{
    printf("Error: %d, %s in file %s at line %d",type, msg, file, line);
    throw("Error: %d, %s in file %s at line %d",type, msg, file, line);
}
