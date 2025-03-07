#include "BlastFrameWork.h"

#include <set>

#include "ConvexHullGenerator.h"
#include "NvBlastExtAuthoring.h"
#include "NvBlastExtAuthoringBondGenerator.h"
#include "NvBlastExtAuthoringConvexMeshBuilder.h"
#include "NvBlastExtAuthoringTypes.h"
#include "NvBlastGlobals.h"
#include "NvBounds3.h"
#include "NvCTypes.h"
#include "NvCMath.h"
#include "shared/NvFoundation/Nv.h"
#include "toolkit/NvBlastTkFramework.h"
#define ENABLE_VHACD_IMPLEMENTATION 
#include "VHACD.h"

BlastFrameWork& BlastFrameWork::GetInstance()
{
	static BlastFrameWork instance;
	return instance;
}

BlastFrameWork::~BlastFrameWork()
{
    myDecompoeser->Release();
	myBondGenerator->release();
	delete myConvexHullGenerator;
}

void BlastFrameWork::buildPhysxChunk(Nv::Blast::AuthoringResult& aResult, const Nv::Blast::ConvexDecompositionParams& iParams, uint32_t aChunksToProcessCount, uint32_t* aChunksToProcess)
{
    uint32_t chunkCount = (uint32_t)aResult.chunkCount;
    if (iParams.maximumNumberOfHulls == 1)
    {
        aResult.collisionHullOffset = SAFE_ARRAY_NEW(uint32_t, chunkCount + 1);
        aResult.collisionHullOffset[0] = 0;
        aResult.collisionHull = SAFE_ARRAY_NEW(Nv::Blast::CollisionHull*, chunkCount);
        for (uint32_t i = 0; i < chunkCount; ++i)
        {
            std::vector<NvcVec3> vertices;
            for (uint32_t p = aResult.geometryOffset[i]; p < aResult.geometryOffset[i + 1]; ++p)
            {
                Nv::Blast::Triangle& tri = aResult.geometry[p];
                vertices.push_back(tri.a.p);
                vertices.push_back(tri.b.p);
                vertices.push_back(tri.c.p);
            }
            aResult.collisionHullOffset[i + 1] = aResult.collisionHullOffset[i] + 1;
            aResult.collisionHull[i] = myConvexHullGenerator->buildCollisionGeometry((uint32_t)vertices.size(), vertices.data());
        }
    }
    else
    {
        std::set<int32_t> chunkSet;
        for (uint32_t c = 0; c < aChunksToProcessCount; c++)
        {
            chunkSet.insert(aChunksToProcess[c]);
        }
        std::vector<std::vector<Nv::Blast::CollisionHull*> > hulls(chunkCount);
        int32_t totalHulls = 0;
        for (uint32_t i = 0; i < chunkCount; ++i)
        {
            if (chunkSet.size() > 0 && chunkSet.find(i) == chunkSet.end())
            {
                int32_t newHulls = aResult.collisionHullOffset[i + 1] - aResult.collisionHullOffset[i];
                int32_t off = aResult.collisionHullOffset[i];
                for (int32_t subhull = 0; subhull < newHulls; ++subhull)
                {
                    hulls[i].push_back(aResult.collisionHull[off + subhull]);
                }
                totalHulls += newHulls;
                continue;
            }

            Nv::Blast::CollisionHull** tempHull;

            int32_t newHulls = buildConvexMeshDecomposition(aResult.geometry + aResult.geometryOffset[i], aResult.geometryOffset[i + 1] - aResult.geometryOffset[i], 
                                                            iParams, tempHull);
            totalHulls += newHulls;         
            for (int32_t h = 0; h < newHulls; ++h)
            {
                hulls[i].push_back(tempHull[h]);
            }
            SAFE_ARRAY_DELETE(tempHull);
        }

        aResult.collisionHullOffset = SAFE_ARRAY_NEW(uint32_t, chunkCount + 1);
        aResult.collisionHullOffset[0] = 0;
        aResult.collisionHull = SAFE_ARRAY_NEW(Nv::Blast::CollisionHull*, totalHulls);

        for (uint32_t i = 0; i < chunkCount; ++i)
        {
            aResult.collisionHullOffset[i + 1] = aResult.collisionHullOffset[i] + hulls[i].size();
            int32_t off = aResult.collisionHullOffset[i];
            for (uint32_t subhull = 0; subhull < hulls[i].size(); ++subhull)
            {
                aResult.collisionHull[off + subhull] = hulls[i][subhull];
            }           
        }
    }
}

uint32_t BlastFrameWork::buildConvexMeshDecomposition(Nv::Blast::Triangle* aMesh, uint32_t aTriangleCount, const Nv::Blast::ConvexDecompositionParams& iParams, Nv::Blast::CollisionHull**& aOutHulls)
{
	std::vector<float> points(aTriangleCount*9);
	std::vector<uint32_t> indices(aTriangleCount*3);

	uint32_t indx = 0;
    uint32_t indxCoord = 0;
    //Transfer to usable data
	nvidia::NvBounds3 chunkBound = nvidia::NvBounds3::empty();
    for (uint32_t i = 0; i < aTriangleCount; ++i)
    {
        for (auto& t : { aMesh[i].a.p , aMesh[i].b.p , aMesh[i].c.p })
        {

            chunkBound.include(nvidia::NvVec3(t.x, t.y, t.z));
            points[indxCoord] = t.x;
            points[indxCoord + 1] = t.y;
            points[indxCoord + 2] = t.z;
            indxCoord += 3;
        }
        indices[indx] = indx;
        indices[indx + 1] = indx + 1;
        indices[indx + 2] = indx + 2;
        indx += 3;
    }
	nvidia::NvVec3 rsc = chunkBound.getDimensions();
	rsc.x = 1 / rsc.x;
	rsc.y = 1 / rsc.y;
	rsc.z = 1 / rsc.z;
    for (uint32_t i = 0; i < points.size(); i += 3)
    {
        points[i] = (points[i] - chunkBound.minimum.x) * rsc.x;
        points[i + 1] = (points[i + 1] - chunkBound.minimum.y) * rsc.y;
        points[i + 2] = (points[i + 2] - chunkBound.minimum.z) * rsc.z;
    }


    VHACD::IVHACD::Parameters vhacdParam;
    vhacdParam.m_maxConvexHulls = iParams.maximumNumberOfHulls;
    vhacdParam.m_resolution = iParams.voxelGridResolution;
	vhacdParam.m_minimumVolumePercentErrorAllowed = iParams.concavity;  
    //compute the hulls
    myDecompoeser->Compute(points.data(), aTriangleCount*3, indices.data(), aTriangleCount, vhacdParam);
    //allocate memory for collision hulls
	const uint32_t nConvexHulls = myDecompoeser->GetNConvexHulls();
	aOutHulls = SAFE_ARRAY_NEW(Nv::Blast::CollisionHull*, nConvexHulls);
    //Creates the collision hulls
	for (uint32_t i = 0; i < nConvexHulls; ++i)
	{
		VHACD::IVHACD::ConvexHull hl;
        myDecompoeser->GetConvexHull(i, hl);
        std::vector<NvcVec3> vertices;
        for (uint32_t v = 0; v < hl.m_points.size(); ++v)
        {
            vertices.push_back(NvcVec3(hl.m_points[v].mX, hl.m_points[v + 1].mY, hl.m_points[v + 2].mZ));
            vertices.back().x = vertices.back().x * rsc.x + chunkBound.minimum.x;
            vertices.back().y = vertices.back().y * rsc.y + chunkBound.minimum.y;
            vertices.back().z = vertices.back().z * rsc.z + chunkBound.minimum.z;

        }
		aOutHulls[i] = myConvexHullGenerator->buildCollisionGeometry(vertices.size(), vertices.data());
	}
    myDecompoeser->Clean();

	return nConvexHulls;
}

void BlastFrameWork::SetHullAndBondGenerator(Nv::Blast::ConvexMeshBuilder* aGen)
{
	myBondGenerator->release();//Release memory allocated to previous generator
    myBondGenerator = NvBlastExtAuthoringCreateBondGenerator(aGen);
}

void BlastFrameWork::SetHullGenerator(Nv::Blast::ConvexMeshBuilder* aGen)
{
    delete myConvexHullGenerator;
	myConvexHullGenerator = dynamic_cast<ConvexHullGenerator*>(aGen);
}

void BlastFrameWork::SetHullAndBondGeneratorDefault()
{
	myBondGenerator->release();//Release memory allocated to previous generator
	myBondGenerator = NvBlastExtAuthoringCreateBondGenerator(myConvexHullGenerator);
}

float BlastFrameWork::CalculateGeometryVolumeAndCentroid(NvcVec3& aCentriod, const Nv::Blast::Triangle* aTris,
	size_t aTriCount)
{
	aCentriod = NvcVec3(0.0f, 0.0f, 0.0f);

    // First find an approximate centroid for a more accurate calculation
    size_t N = 0;
    NvcVec3 disp = { 0.0f, 0.0f, 0.0f };
    for (size_t i = 0; i < aTriCount; ++i)
    {
        const Nv::Blast::Triangle& tri = aTris[i];
        disp = disp + tri.a.p + tri.b.p + tri.c.p;
        N += 3;
    }

    if (N == 0)
    {
        return 0.0f;
    }

    disp = disp / static_cast<float>(N);

    float sixV = 0.0f;
    for (size_t i = 0; i < aTriCount; ++i)
    {
        const Nv::Blast::Triangle& tri = aTris[i];
        const NvcVec3 a = tri.a.p - disp;
        NvcVec3 b = tri.b.p - disp;
        const NvcVec3 c = tri.c.p - disp;

        const float sixTetV =
            a.x * b.y * c.z - a.x * b.z * c.y - a.y * b.x * c.z +
            a.y * b.z * c.x + a.z * b.x * c.y - a.z * b.y * c.x;

        sixV += sixTetV;

        aCentriod = aCentriod + sixTetV * (a + b + c);
    }

    // Extra factor of four to average tet vertices
    aCentriod = aCentriod / (4.0f * sixV) + disp;

    return std::abs(sixV) / 6.0f;
}

float BlastFrameWork::CalculateCollisionVolumeAndCentroid(NvcVec3& aCentriod, const Nv::Blast::CollisionHull& aHull)
{
	aCentriod = {0.0f, 0.0f, 0.0f};
    float volume = 0.0f;

    for (uint32_t i = 0; i < aHull.polygonDataCount; ++i)
    {
        const Nv::Blast::HullPolygon& poly = aHull.polygonData[i];
        const NvcVec3& p0 = aHull.points[poly.indexBase];

        for (uint32_t j = 1; j < poly.vertexCount - 1; ++j)
        {
            const NvcVec3& p1 = aHull.points[poly.indexBase + j];
            const NvcVec3& p2 = aHull.points[poly.indexBase + j + 1];

            // Calculate the volume of the tetrahedron formed by the triangle and the origin
            float tetraVolume = (p0.x * (p1.y * p2.z - p1.z * p2.y) -
                                 p0.y * (p1.x * p2.z - p1.z * p2.x) +
                                 p0.z * (p1.x * p2.y - p1.y * p2.x)) / 6.0f;

            volume += tetraVolume;

            // Calculate the centroid of the tetrahedron
            NvcVec3 tetraCentroid = {(p0.x + p1.x + p2.x) / 4.0f,
                                     (p0.y + p1.y + p2.y) / 4.0f,
                                     (p0.z + p1.z + p2.z) / 4.0f};

            aCentriod.x += tetraVolume * tetraCentroid.x;
            aCentriod.y += tetraVolume * tetraCentroid.y;
            aCentriod.z += tetraVolume * tetraCentroid.z;
        }
    }
    if (volume != 0.0f)
    {
        aCentriod.x /= volume;
        aCentriod.y /= volume;
        aCentriod.z /= volume;
    }

    return std::abs(volume);
}

BlastFrameWork::BlastFrameWork()
{
	NvBlastTkFrameworkCreate();
	myDecompoeser = VHACD::CreateVHACD();
	myConvexHullGenerator = new ConvexHullGenerator();
    myBondGenerator =  NvBlastExtAuthoringCreateBondGenerator(myConvexHullGenerator);
}
