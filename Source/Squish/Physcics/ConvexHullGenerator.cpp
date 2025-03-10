#include "ConvexHullGenerator.h"
#define ENABLE_VHACD_IMPLEMENTATION 1
#include "cooking/PxCooking.h"

#include "PhysicsEngine.h"
#include "PxDefaultStreams.h"
#include "PxPhysics.h"
#include "PxRigidActor.h"
#include "geometry/PxConvexMeshGeometry.h"
#include "extensions/authoringCommon/NvBlastExtAuthoringTypes.h"




ConvexHullGenerator::ConvexHullGenerator()
{
}

void ConvexHullGenerator::release()
{
    printf("Release stupid\n");
}

Nv::Blast::CollisionHull* ConvexHullGenerator::buildCollisionGeometry(uint32_t verticesCount, const NvcVec3* vertexData)
{
	physx::PxTolerancesScale scale;
	scale = Squish::PhysicsEngine::Get()->GetPhysics()->getTolerancesScale();
	physx::PxCookingParams cook(scale);
    physx::PxConvexMeshDesc desc;
	desc.points.count = verticesCount;
	desc.points.stride = sizeof(physx::PxVec3);
	desc.points.data = vertexData;
	desc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;
	desc.vertexLimit = 255;//hard limit
	desc.polygonLimit = 255;//hard limit
	physx::PxDefaultMemoryOutputStream buf;
	PxCookConvexMesh(cook, desc, buf);
	physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	physx::PxConvexMesh* mesh = Squish::PhysicsEngine::Get()->GetPhysics()->createConvexMesh(input);
	physx::PxMeshScale Scale(physx::PxVec3(1,1,1));
	physx::PxConvexMeshGeometry geom(mesh, Scale);
	Nv::Blast::CollisionHull* hull = new Nv::Blast::CollisionHull();

	physx::PxU32 vertexCount = mesh->getNbVertices();
    const physx::PxVec3* physxVertices = mesh->getVertices();

    hull->pointsCount = vertexCount;
    hull->points = new NvcVec3[vertexCount];
	for (physx::PxU32 i = 0; i < vertexCount; ++i)
    {
        hull->points[i] = NvcVec3(physxVertices[i].x, physxVertices[i].y, physxVertices[i].z);
    }
	const physx::PxU8* indices = mesh->getIndexBuffer();
	uint32_t indexCount = mesh->getNbPolygons()*3;
    hull->indicesCount = indexCount;
    hull->indices = new uint32_t[indexCount];
	for(uint32_t i = 0; i<indexCount; i++)
	{
		hull->indices[i] = indices[i];
	}
	hull->polygonDataCount = mesh->getNbPolygons();
	hull->polygonData = new Nv::Blast::HullPolygon[hull->polygonDataCount];
	for(uint32_t i = 0; i < hull->polygonDataCount; i++)
	{
		physx::PxHullPolygon data;
		if(mesh->getPolygonData(i, data))
		{
			hull->polygonData[i].indexBase = data.mIndexBase;
			hull->polygonData[i].vertexCount = data.mNbVerts;
			hull->polygonData[i].plane[0] = data.mPlane[0];
			hull->polygonData[i].plane[1] = data.mPlane[1];
			hull->polygonData[i].plane[2] = data.mPlane[2];
			hull->polygonData[i].plane[3] = data.mPlane[3];
			
		}
		else
		{
			printf("Error getting data at index %d\n", i);
		}
	}

    return hull;
}

Nv::Blast::CollisionHull* ConvexHullGenerator::buildCollisionGeometry(uint32_t verticesCount, const NvcVec3* vertexData,
	physx::PxGeometry*& aGeometry)
{
	
	physx::PxTolerancesScale scale;
	scale = Squish::PhysicsEngine::Get()->GetPhysics()->getTolerancesScale();
	physx::PxCookingParams cook(scale);
    physx::PxConvexMeshDesc desc;
	desc.points.count = verticesCount;
	desc.points.stride = sizeof(physx::PxVec3);
	desc.points.data = vertexData;
	desc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;
	desc.vertexLimit = 255;//hard limit
	desc.polygonLimit = 255;//hard limit
	physx::PxDefaultMemoryOutputStream buf;
	PxCookConvexMesh(cook, desc, buf);
	physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	physx::PxConvexMesh* mesh = Squish::PhysicsEngine::Get()->GetPhysics()->createConvexMesh(input);

	physx::PxMeshScale Scale(physx::PxVec3(1,1,1));
	aGeometry =  new physx::PxConvexMeshGeometry(mesh, Scale);//Set the geometry of the convex 

	Nv::Blast::CollisionHull* hull = new Nv::Blast::CollisionHull();

	physx::PxU32 vertexCount = mesh->getNbVertices();
    const physx::PxVec3* physxVertices = mesh->getVertices();

    hull->pointsCount = vertexCount;
    hull->points = new NvcVec3[vertexCount];
	for (physx::PxU32 i = 0; i < vertexCount; ++i)
    {
        hull->points[i] = NvcVec3(physxVertices[i].x, physxVertices[i].y, physxVertices[i].z);
    }
	const physx::PxU8* indices = mesh->getIndexBuffer();
	uint32_t indexCount = mesh->getNbPolygons()*3;
    hull->indicesCount = indexCount;
    hull->indices = new uint32_t[indexCount];
	for(uint32_t i = 0; i<indexCount; i++)
	{
		hull->indices[i] = indices[i];
	}
	hull->polygonDataCount = mesh->getNbPolygons();
	hull->polygonData = new Nv::Blast::HullPolygon[hull->polygonDataCount];
	for(uint32_t i = 0; i < hull->polygonDataCount; i++)
	{
		physx::PxHullPolygon data;
		if(mesh->getPolygonData(i, data))
		{
			hull->polygonData[i].indexBase = data.mIndexBase;
			hull->polygonData[i].vertexCount = data.mNbVerts;
			hull->polygonData[i].plane[0] = data.mPlane[0];
			hull->polygonData[i].plane[1] = data.mPlane[1];
			hull->polygonData[i].plane[2] = data.mPlane[2];
			hull->polygonData[i].plane[3] = data.mPlane[3];
			
		}
		else
		{
			printf("Error getting data at index %d\n", i);
		}
	}

    return hull;
}

void ConvexHullGenerator::releaseCollisionHull(Nv::Blast::CollisionHull* hull) const
{
	delete hull;
}
