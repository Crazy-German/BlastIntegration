#include "RigidBody.h"

#include <vector>

#include "PhysicsEngine.h"
#include "PxPhysics.h"
#include "PxRigidBody.h"
#include "PxRigidDynamic.h"
#include "PxRigidStatic.h"
#include "cooking/PxConvexMeshDesc.h"
#include "cooking/PxCooking.h"
#include "extensions/PxDefaultSimulationFilterShader.h"
#include "extensions/PxDefaultStreams.h"
#include "Logger/Logger.h"

using namespace Squish;
using namespace physx;


RigidBody::RigidBody()
	: myRigidBody(nullptr), myType(Static)
{
	myFilterData.word0 = 1;
}

RigidBody::~RigidBody()
{
}

bool RigidBody::Initialize(RigidBodyType aType, ShapeType aShape, TransformBase* aTransform,
                           CommonUtilities::Vector3f aMaterial, float aRadius, const TransformBase& aLocalTransform, uint16_t aGroup)
{
	if(aShape != Sphere)
	{
		printf("Wrong shape type\n");
		return false;
	}
	myUserData.aID = PhysXId++;
	myUserData.myTransform = aTransform;
	myType = aType;
	physx::PxPhysics& Physics = PxGetPhysics();
	physx::PxShape* shape = Physics.createShape(physx::PxSphereGeometry(aRadius), *Physics.createMaterial(aMaterial.x, aMaterial.y, aMaterial.z),
	                                            true, physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSCENE_QUERY_SHAPE | (aType == RigidBodyType::Trigger ? physx::PxShapeFlag::eTRIGGER_SHAPE : physx::PxShapeFlag::eSIMULATION_SHAPE));
	shape->setLocalPose(PxTransform(ToPhysxVect(aLocalTransform.GetPosition()*aLocalTransform.GetRotationMatrix()), ToPhysxQuat(aLocalTransform.GetRotationQuaternion())));
	physx::PxTransform Transform;
	Transform.p= ToPhysxVect(aLocalTransform.GetPosition()*aLocalTransform.GetRotationMatrix());
	Transform.q = physx::PxQuat(aTransform->GetRotationQuaternion().x, aTransform->GetRotationQuaternion().y, aTransform->GetRotationQuaternion().z, aTransform->GetRotationQuaternion().w);
	shape->setQueryFilterData(myFilterData);
	switch (myType)
	{
	case Static:
		myRigidBody = Physics.createRigidStatic(Transform)->is<physx::PxRigidActor>();
		break;
	case Dynamic:
		myRigidBody = Physics.createRigidDynamic(Transform)->is<physx::PxRigidActor>();
		myRigidBody->is<PxRigidBody>()->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
		break;
	default: ;
	}
	if(myRigidBody == nullptr)
	{
		return false;
	}
	myRigidBody->attachShape(*shape);
	physx::PxSetGroup(*myRigidBody, aGroup);
	myRigidBody->userData =  &myUserData;
	return myRigidBody != nullptr;
}

bool RigidBody::Initialize(RigidBodyType aType, ShapeType aShape, TransformBase* aTransform,
                           CommonUtilities::Vector3f aMaterial, CommonUtilities::Vector3f aHalfWidths, const TransformBase& aLocalTransform, uint16_t
                           aGroup)
{
	if(aShape != Box)
	{
		printf("Wrong shape type\n");
		return false;
	}
	myUserData.aID = PhysXId++;
	myUserData.myTransform = aTransform;
	myType = aType;
	physx::PxPhysics& Physics = PxGetPhysics();
	
	physx::PxShape* shape = Physics.createShape(physx::PxBoxGeometry(aHalfWidths.x, aHalfWidths.y, aHalfWidths.z), *Physics.createMaterial(aMaterial.x, aMaterial.y, aMaterial.z),
	                                            true, physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSCENE_QUERY_SHAPE | (aType == RigidBodyType::Trigger ? physx::PxShapeFlag::eTRIGGER_SHAPE : physx::PxShapeFlag::eSIMULATION_SHAPE));
	shape->setLocalPose(PxTransform(ToPhysxVect(aLocalTransform.GetPosition()*aLocalTransform.GetRotationMatrix()+aHalfWidths*aLocalTransform.GetRotationMatrix()), ToPhysxQuat(aLocalTransform.GetRotationQuaternion())));
	physx::PxTransform Transform;
	Transform.p= ToPhysxVect(aTransform->GetPosition());
	Transform.q = physx::PxQuat(aTransform->GetRotationQuaternion().x, aTransform->GetRotationQuaternion().y, aTransform->GetRotationQuaternion().z, aTransform->GetRotationQuaternion().w);
	shape->setQueryFilterData(myFilterData);

	switch (myType)
	{
	case Static:
	{
		auto pxStatic = Physics.createRigidStatic(Transform);
		if (pxStatic == nullptr)
		{
			//LOG(PhysXLog, Error, "Rigidbody doesn't have a valid transform");
			return false;
		}
		myRigidBody = pxStatic->is<physx::PxRigidActor>();
		break;
	}
	case Dynamic:
	{
		myRigidBody = Physics.createRigidDynamic(Transform)->is<physx::PxRigidActor>();
		myRigidBody->is<PxRigidBody>()->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
		myRigidBody->is<PxRigidBody>()->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD_FRICTION, true);
		break;
	}
	default:
	{
		myRigidBody = Physics.createRigidStatic(Transform)->is<physx::PxRigidActor>();
		break;
	}
	}

	if(myRigidBody == nullptr)
	{
		return false;
	}

	myRigidBody->attachShape(*shape);
	physx::PxSetGroup(*myRigidBody, aGroup);
	myRigidBody->userData = &myUserData;
	return myRigidBody != nullptr;
}

bool RigidBody::Initialize(RigidBodyType aType, ShapeType aShape, TransformBase* aTransform,
                           CommonUtilities::Vector3f aMaterial, float aRadius, float aHalfHeight, const TransformBase& aLocalTransform, uint16_t
                           aGroup)
{
	if(aShape != Capsule)
	{
		printf("Wrong shape type\n");
		return false;
	}
	myUserData.aID = PhysXId++;
	myUserData.myTransform = aTransform;
	myType = aType;
	physx::PxPhysics& Physics = PxGetPhysics();
	PxMaterial* mat = Physics.createMaterial(aMaterial.x, aMaterial.y, aMaterial.z);
	
	physx::PxShape* shape = Physics.createShape(physx::PxCapsuleGeometry(aRadius, aHalfHeight), *mat,
	                                            true, physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSCENE_QUERY_SHAPE | (aType == RigidBodyType::Trigger ? physx::PxShapeFlag::eTRIGGER_SHAPE : physx::PxShapeFlag::eSIMULATION_SHAPE));
	PxQuat rotation = PxQuat(PxPi / 2.0f, PxVec3(0, 0, 1)); // 90 degrees rotation around the Z-axis
	PxTransform local;
	local.p = ToPhysxVect(aLocalTransform.GetPosition()*aLocalTransform.GetRotationMatrix());
	local.q+=rotation;
	local.q = local.getNormalized().q;
	shape->setLocalPose(local);
	shape->setQueryFilterData(myFilterData);
	
	physx::PxTransform Transform = ToPhysXTransform(*aTransform);
	//shape->setContactOffset(10);
	switch (myType)
	{
	case Static:
		myRigidBody = Physics.createRigidStatic(Transform)->is<physx::PxRigidActor>();
		break;
	case Dynamic:
		myRigidBody = Physics.createRigidDynamic(Transform)->is<physx::PxRigidActor>();
		myRigidBody->is<PxRigidBody>()->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
		myRigidBody->is<PxRigidBody>()->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD_FRICTION, true);
		
		//myRigidBody->is<PxRigidBody>()->setRigidBodyFlag(PxRigidBodyFlag::eRETAIN_ACCELERATIONS, true);
		break;
	default: ;
	}

	if(myRigidBody == nullptr)
	{
		return false;
	}
	
	myRigidBody->attachShape(*shape);
	physx::PxSetGroup(*myRigidBody, aGroup);
	myRigidBody->userData = &myUserData;
	return myRigidBody != nullptr;
}

bool RigidBody::Initialize(RigidBodyType aType, ShapeType aShape, TransformBase* aTransform,
                           CU::Vector3f aMaterial, std::vector<CommonUtilities::Vector3f>* aVertexlist, std::vector<uint32_t>* aIndexList, uint32_t
                           aVertexLimit, uint32_t
                           aPolygonLimit, const TransformBase& aLocalTransform, uint16_t aGroup)
{
	myUserData.aID = PhysXId++;
	myUserData.myTransform = aTransform;
	myType = aType;
	if(aShape!= ShapeType::Convex && aShape!=ShapeType::Mesh)
	{
		printf("Bozzo you used the wrong Shape type in rigidbody Init\n");
		return false; 
	}
	physx::PxPhysics& Physics = PxGetPhysics();

	physx::PxTolerancesScale scale;
	scale = Physics.getTolerancesScale();
	physx::PxCookingParams cook(scale);
	
	physx::PxShape* shape = nullptr;
	if(aShape == ShapeType::Convex)
	{
		physx::PxConvexMeshDesc desc;
		desc.points.count = (PxU32)aVertexlist->size();
		desc.points.stride = sizeof(physx::PxVec3);
		std::vector<physx::PxVec3> data;
		for(const CommonUtilities::Vector3f& vertex : *aVertexlist)
		{
			data.push_back(physx::PxVec3(vertex.x, vertex.y, vertex.z));
		}
		desc.points.data = data.data();
		desc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;
		desc.vertexLimit = (PxU16)aVertexLimit;
		desc.polygonLimit = (PxU16)aPolygonLimit;
		physx::PxDefaultMemoryOutputStream buf;
		PxCookConvexMesh(cook, desc, buf);
		physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
		physx::PxConvexMesh* mesh = Physics.createConvexMesh(input);
		physx::PxMeshScale Scale(physx::PxVec3(1,1,1));
		physx::PxConvexMeshGeometry geom(mesh, Scale);

		shape = Physics.createShape(geom, *Physics.createMaterial(aMaterial.x, aMaterial.y, aMaterial.z),true, 
			physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSCENE_QUERY_SHAPE | (aType == RigidBodyType::Trigger ? physx::PxShapeFlag::eTRIGGER_SHAPE : physx::PxShapeFlag::eSIMULATION_SHAPE));
	}
	if(aShape == ShapeType::Mesh)
	{
		if(aIndexList == nullptr)
		{
			printf("Missing index list for tri mesh\n");
			return false;
		}
		physx::PxTriangleMeshDesc desc;
		desc.points.count = aVertexLimit;
		desc.points.stride = sizeof(physx::PxVec3);
		std::vector<physx::PxVec3> data;
		for(const CommonUtilities::Vector3f& vertex : *aVertexlist)
		{
			data.push_back(physx::PxVec3(vertex.x, vertex.y, vertex.z));
		}
		desc.points.data = data.data();
		PxArray<PxU32> test;
		std::vector<PxU32> triData;
		for(const auto& vertex : *aIndexList)
		{
			test.pushBack(vertex);
			triData.push_back(vertex);
		}
		desc.triangles.count        = aPolygonLimit;
		desc.triangles.stride       = 3*sizeof(PxU32);
		desc.triangles.data         = test.begin();
		
		if(!desc.isValid())
		{
			printf("Invalid mesh collider\n");
			return false;
		}
		physx::PxDefaultMemoryOutputStream buf;
		PxTriangleMeshCookingResult::Enum cond;
		if(!PxCookTriangleMesh(cook, desc, buf, &cond))
		{
			printf("Failed att cooking, get new chef\n");
			return false;
		}
		physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
		physx::PxTriangleMesh* mesh = Physics.createTriangleMesh(input);
		physx::PxMeshScale Scale(physx::PxVec3(1,1,1));
		physx::PxTriangleMeshGeometry geom(mesh, Scale);
		
		shape = Physics.createShape(geom, *Physics.createMaterial(aMaterial.x, aMaterial.y, aMaterial.z),true, 
			physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSCENE_QUERY_SHAPE | (aType == RigidBodyType::Trigger ? physx::PxShapeFlag::eTRIGGER_SHAPE : physx::PxShapeFlag::eSIMULATION_SHAPE));
		//shape->setContactOffset(10.0f);
	}
	shape->setLocalPose(PxTransform(ToPhysxVect(aLocalTransform.GetPosition()*aLocalTransform.GetRotationMatrix()), ToPhysxQuat(aLocalTransform.GetRotationQuaternion())));
	shape->setQueryFilterData(myFilterData);
	
	physx::PxTransform Transform;
	Transform.p= physx::PxVec3(aTransform->GetPosition().x, aTransform->GetPosition().y, aTransform->GetPosition().z);
	Transform.q = physx::PxQuat(aTransform->GetRotationQuaternion().x, aTransform->GetRotationQuaternion().y, aTransform->GetRotationQuaternion().z, aTransform->GetRotationQuaternion().w);

	switch (myType)
	{
	case Static:
		myRigidBody = Physics.createRigidStatic(Transform)->is<physx::PxRigidActor>();
		break;
	case Dynamic:
		myRigidBody = Physics.createRigidDynamic(Transform)->is<physx::PxRigidActor>();
		break;
	default: ;
	}

	if(myRigidBody == nullptr || shape == nullptr)
	{
		return false;
	}
	
	if(shape!= nullptr)
	{
		if(!myRigidBody->attachShape(*shape))
		{
			printf("Atatch shape failed\n");
			return false;
		}
	}
	
	physx::PxSetGroup(*myRigidBody, aGroup);
	myRigidBody->userData = &myUserData;
	
	return myRigidBody != nullptr;
}

bool RigidBody::InitNoShape(RigidBodyType aType = RigidBodyType::Dynamic)
{
	myUserData.aID = PhysXId++;
	myUserData.myTransform = nullptr;
	myType = aType;
	
	physx::PxPhysics& Physics = PxGetPhysics();
	switch (myType)
	{
	case Static:
		myRigidBody = Physics.createRigidStatic(PxTransform())->is<physx::PxRigidActor>();
		break;
	case Dynamic:
		myRigidBody = Physics.createRigidDynamic(PxTransform())->is<physx::PxRigidActor>();
		break;
	default: ;
	}

	myRigidBody->userData = &myUserData;
	return myRigidBody!=nullptr;
}

void RigidBody::AddShape(RigidBodyType aType, ShapeType aShape,
                         const TransformBase& aTransform, CommonUtilities::Vector3f aMaterial, float aRadius)
{
	if(aShape != Sphere)
	{
		printf("Add shape wrong shape should be sphere");
		return;
	}
	physx::PxPhysics& Physics = PxGetPhysics();
	physx::PxShape* shape = Physics.createShape(physx::PxSphereGeometry(aRadius), *Physics.createMaterial(aMaterial.x, aMaterial.y, aMaterial.z),
	                                            true, physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSCENE_QUERY_SHAPE | (aType == RigidBodyType::Trigger ? physx::PxShapeFlag::eTRIGGER_SHAPE : physx::PxShapeFlag::eSIMULATION_SHAPE));
	shape->setLocalPose(PxTransform(ToPhysxVect(aTransform.GetPosition()*aTransform.GetRotationMatrix()), ToPhysxQuat(aTransform.GetRotationQuaternion())));
	shape->setQueryFilterData(myFilterData);

	myRigidBody->attachShape(*shape);
}

void RigidBody::AddShape(RigidBodyType aType, ShapeType aShape,
                         const TransformBase& aTransform, CommonUtilities::Vector3f aMaterial, CommonUtilities::Vector3f aHalfWidths)
{
	if(aShape != Box)
	{
		printf("Add shape wrong shape should be box");
		return;
	}
	physx::PxPhysics& Physics = PxGetPhysics();
	physx::PxShape* shape = Physics.createShape(physx::PxBoxGeometry(aHalfWidths.x, aHalfWidths.y, aHalfWidths.z), *Physics.createMaterial(aMaterial.x, aMaterial.y, aMaterial.z),
	                                            true, physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSCENE_QUERY_SHAPE | (aType == RigidBodyType::Trigger ? physx::PxShapeFlag::eTRIGGER_SHAPE : physx::PxShapeFlag::eSIMULATION_SHAPE));
	
	shape->setLocalPose(PxTransform(ToPhysxVect(aTransform.GetPosition()*aTransform.GetRotationMatrix()+aHalfWidths*aTransform.GetRotationMatrix()), ToPhysxQuat(aTransform.GetRotationQuaternion())));
	shape->setQueryFilterData(myFilterData);

	myRigidBody->attachShape(*shape);
}

void RigidBody::AddShape(RigidBodyType aType, ShapeType aShape, const TransformBase& aTransform,
                         CommonUtilities::Vector3f aMaterial, float aRadius, float aHalfHeight)
{
	if(aShape != Capsule)
	{
		printf("Add shape wrong shape should be capsule");
		return;
	}
	physx::PxPhysics& Physics = PxGetPhysics();
	PxMaterial* mat = Physics.createMaterial(aMaterial.x, aMaterial.y, aMaterial.z);
	
	physx::PxShape* shape = Physics.createShape(physx::PxCapsuleGeometry(aRadius, aHalfHeight), *mat,
	                                            true, physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSCENE_QUERY_SHAPE | (aType == RigidBodyType::Trigger ? physx::PxShapeFlag::eTRIGGER_SHAPE : physx::PxShapeFlag::eSIMULATION_SHAPE));
	PxQuat rotation = PxQuat(PxPi / 2.0f, PxVec3(0, 0, 1)); // 90 degrees rotation around the Z-axis so capsule is standing
	shape->setLocalPose(PxTransform(PxVec3(0), rotation));
	
	physx::PxTransform Transform;
	Transform.p= physx::PxVec3(aTransform.GetPosition().x, aTransform.GetPosition().y, aTransform.GetPosition().z);
	Transform.q = physx::PxQuat(aTransform.GetRotationQuaternion().x, aTransform.GetRotationQuaternion().y, aTransform.GetRotationQuaternion().z, aTransform.GetRotationQuaternion().w);

	shape->setLocalPose(shape->getLocalPose()*Transform);
	shape->setQueryFilterData(myFilterData);

	myRigidBody->attachShape(*shape);
}

void RigidBody::AddShape(RigidBodyType aType, ShapeType aShape, const TransformBase& aTransform,
                         CommonUtilities::Vector3f aMaterial, std::vector<CommonUtilities::Vector3f>* aVertexlist, std::vector<uint32_t>*
                         aIndexList, uint32_t aVertexLimit,
                         uint32_t aPolygonLimit)
{
	if(aShape != Convex && aShape != Mesh)
	{
		printf("Add shape wrong shape should be convex or mesh");
		return;
	}
	physx::PxPhysics& Physics = PxGetPhysics();

	physx::PxTolerancesScale scale;
	physx::PxCookingParams cook(scale);
	
	physx::PxShape* shape = nullptr;
	if(aShape == ShapeType::Convex)
	{
		physx::PxConvexMeshDesc desc;
		desc.points.count = (PxU32)aVertexlist->size();
		desc.points.stride = sizeof(physx::PxVec3);
		std::vector<physx::PxVec3> data;
		for(const CommonUtilities::Vector3f& vertex : *aVertexlist)
		{
			data.push_back(physx::PxVec3(vertex.x, vertex.y, vertex.z));
		}
		desc.points.data = data.data();
		desc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;
		desc.vertexLimit = (PxU16)aVertexLimit;
		desc.polygonLimit = (PxU16)aPolygonLimit;
		physx::PxDefaultMemoryOutputStream buf;
		PxCookConvexMesh(cook, desc, buf);
		physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
		physx::PxConvexMesh* mesh = Physics.createConvexMesh(input);
		physx::PxMeshScale Scale(physx::PxVec3(1,1,1));
		physx::PxConvexMeshGeometry geom(mesh, Scale);

		shape = Physics.createShape(geom, *Physics.createMaterial(aMaterial.x, aMaterial.y, aMaterial.z),true, 
			physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSCENE_QUERY_SHAPE | (aType == RigidBodyType::Trigger ? physx::PxShapeFlag::eTRIGGER_SHAPE : physx::PxShapeFlag::eSIMULATION_SHAPE));
		//shape->setContactOffset(10.f);
	}
	if(aShape == ShapeType::Mesh)
	{
		if(aIndexList == nullptr)
		{
			printf("Missing index list for tri mesh\n");
			return;
		}
		physx::PxTriangleMeshDesc desc;
		desc.points.count = aVertexLimit;
		desc.points.stride = sizeof(physx::PxVec3);
		std::vector<physx::PxVec3> data;
		for(const CommonUtilities::Vector3f& vertex : *aVertexlist)
		{
			data.push_back(physx::PxVec3(vertex.x, vertex.y, vertex.z));
		}
		desc.points.data = data.data();
		PxArray<PxU32> test;
		std::vector<PxU32> triData;
		for(const auto& vertex : *aIndexList)
		{
			test.pushBack(vertex);
			triData.push_back(vertex);
		}
		desc.triangles.count        = aPolygonLimit;
		desc.triangles.stride       = 3*sizeof(PxU32);
		desc.triangles.data         = test.begin();
		
		if(!desc.isValid())
		{
			printf("Invalid mesh collider\n");
			return;
		}
		physx::PxDefaultMemoryOutputStream buf;
		PxTriangleMeshCookingResult::Enum cond;
		if(!PxCookTriangleMesh(cook, desc, buf, &cond))
		{
			printf("Failed att cooking, get new chef\n");
			return;
		}
		physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
		physx::PxTriangleMesh* mesh = Physics.createTriangleMesh(input);
		physx::PxMeshScale Scale(physx::PxVec3(1,1,1));
		physx::PxTriangleMeshGeometry geom(mesh, Scale);
		
		shape = Physics.createShape(geom, *Physics.createMaterial(aMaterial.x, aMaterial.y, aMaterial.z),true, 
			physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSCENE_QUERY_SHAPE | (aType == RigidBodyType::Trigger ? physx::PxShapeFlag::eTRIGGER_SHAPE : physx::PxShapeFlag::eSIMULATION_SHAPE));
		//shape->setContactOffset(1.0f);
	}
	shape->setLocalPose(PxTransform(ToPhysxVect(aTransform.GetPosition()*aTransform.GetRotationMatrix()), ToPhysxQuat(aTransform.GetRotationQuaternion())));
	shape->setQueryFilterData(myFilterData);

	myRigidBody->attachShape(*shape);
}

void RigidBody::AddForce(CommonUtilities::Vector3f aForce, uint8_t aForceType)
{
	if(myType == Dynamic)
	{
		myRigidBody->is<PxRigidBody>()->addForce(PxVec3(aForce), static_cast<PxForceMode::Enum>(aForceType));
	}
}

CommonUtilities::Vector3f RigidBody::GetVelocity() const
{
	if(myType == Dynamic)
	{
		PxVec3 temp = myRigidBody->is<PxRigidBody>()->getLinearVelocity();
		return CommonUtilities::Vector3f(temp.x, temp.y, temp.z);
	}
	return CommonUtilities::Vector3f();
}

void RigidBody::AddRotationForce(CommonUtilities::Vector3f aForce, uint8_t aForceType)
{
	if(myType == Dynamic)
	{
		myRigidBody->is<PxRigidBody>()->addTorque(PxVec3(aForce), static_cast<PxForceMode::Enum>(aForceType));
	}
}

CommonUtilities::Vector3f RigidBody::GetRotationalVelocity() const
{
	if(myType == Dynamic)
	{
		PxVec3 temp = myRigidBody->is<PxRigidBody>()->getAngularVelocity();
		return CommonUtilities::Vector3f(temp.x, temp.y, temp.z);
	}
	return CommonUtilities::Vector3f();
}

void RigidBody::SetCollisionGroup(uint16_t aGroup)
{
	PxSetGroup(*myRigidBody, aGroup);
}

uint16_t RigidBody::GetCollisionGroup()
{
	return PxGetGroup(*myRigidBody);
}

void RigidBody::SetCollisionLayer(const physx::PxFilterData* aLayer)
{
	PxU32 shapeCount = myRigidBody->getNbShapes();
	PxShape** shapes = new PxShape*[shapeCount];
	myRigidBody->getShapes(shapes, shapeCount);
	for(PxU32 index= 0; index<shapeCount; index++)
	{
		shapes[index]->setSimulationFilterData(*aLayer);
	}
}

physx::PxFilterData RigidBody::SetAndDefineCollisionLayer(physx::PxU32 aOwn, physx::PxU32 aToFilterWith)
{
	PxFilterData filterData;
	filterData.word0 = aOwn;
	filterData.word1 = aToFilterWith;
	PxU32 shapeCount = myRigidBody->getNbShapes();
	PxShape** shapes = new PxShape*[shapeCount];
	myRigidBody->getShapes(shapes, shapeCount);
	for(PxU32 index= 0; index<shapeCount; index++)
	{
		shapes[index]->setSimulationFilterData(filterData);
	}
	return filterData;
}

void RigidBody::SetMaxSpeed(const float& aMaxSpeed)
{
	if(myType == Dynamic)
	{
		myRigidBody->is<PxRigidBody>()->setMaxLinearVelocity(aMaxSpeed);
		
	}
}

float RigidBody::GetMaxSpeed() const
{
	if(myType == Dynamic)
	{
		return myRigidBody->is<PxRigidBody>()->getMaxLinearVelocity();
	}
	return 0;
}

void RigidBody::SetMaxRotation(const float& aMaxRotation)
{
	if(myType == Dynamic)
	{
		myRigidBody->is<PxRigidBody>()->setMaxAngularVelocity(aMaxRotation);
	}
}

float RigidBody::GetMaxRotation() const
{
	if(myType == Dynamic)
	{
		return myRigidBody->is<PxRigidBody>()->getMaxAngularVelocity();
	}
	return 0;
}

void RigidBody::SetMass(const float& aMass)
{
	if(aMass>=0 && myType == Dynamic)
	{
		myRigidBody->is<PxRigidDynamic>()->setMass(aMass);
	}
}

float RigidBody::GetMass() const
{
	if(myType == Dynamic)
	{
		return myRigidBody->is<PxRigidDynamic>()->getMass();
	}
	return -1;
}

void RigidBody::SetLinearDamping(const float& aDampingCoefficient)
{
	if(myType == Dynamic)
	{
		myRigidBody->is<PxRigidDynamic>()->setLinearDamping(aDampingCoefficient);
	}
}

float RigidBody::GetLinearDamping() const
{
	if(myType == Dynamic)
	{
		return myRigidBody->is<PxRigidDynamic>()->getLinearDamping();
	}
	return 0;
}

void RigidBody::SetAngularDamping(const float aDampingCoefficient)
{
	if(myType == Dynamic)
	{
		myRigidBody->is<PxRigidDynamic>()->setAngularDamping(aDampingCoefficient);
		
	}
}

float RigidBody::GetAngularDamping() const
{
	if(myType == Dynamic)
	{
		return myRigidBody->is<PxRigidDynamic>()->getAngularDamping();
	}
	return 0;
}

void RigidBody::SetIsAffectedByGravity(bool aIsEffected)
{
	myRigidBody->is<PxActor>()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !aIsEffected);
}

bool Squish::RigidBody::GetIsAffectedByGravity()
{
	return (myRigidBody->is<PxActor>()->getActorFlags().isSet(PxActorFlag::eDISABLE_GRAVITY));
}

void RigidBody::SetSolverIteratiorCount(uint32_t aPositionIteratorCount, uint32_t aVelocityIteratorCount)
{
	if(myType == Dynamic)
	{
		return myRigidBody->is<PxRigidDynamic>()->setSolverIterationCounts(aPositionIteratorCount, aVelocityIteratorCount);
	}
}

void RigidBody::PrintFrictions()
{

	const PxU32 count = myRigidBody->getNbShapes();
	PxShape* shapes[1];
	myRigidBody->getShapes(shapes, count);
	for(int index = 0; index<1; index++)
	{
		//const PxU16 mcount = shapes[index]->getNbMaterials();
		PxMaterial* material[1];
		shapes[index]->getMaterials(material, 1);
		for(int mIndex = 0; mIndex<1; mIndex++)
		{
			printf("StaticFriction: %.6f\n",material[mIndex]->getStaticFriction());
			printf("DynamicFriction: %.6f\n",material[mIndex]->getDynamicFriction());
				
			
		}
		delete[] &material;
	}
	delete[] &shapes;
}

void RigidBody::SetKinematic(bool aIsKinematic)
{
	if(myType == Dynamic)
	{
		myRigidBody->is<PxRigidDynamic>()->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, aIsKinematic);
	}
}

void RigidBody::LockHorizontalMovement()
{
	if(myType == Dynamic)
	{
		myRigidBody->is<PxRigidDynamic>()->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, true);
		myRigidBody->is<PxRigidDynamic>()->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, true);
	}
}

void RigidBody::UnLockHorizontalMovement()
{
	if(myType == Dynamic)
	{
		myRigidBody->is<PxRigidDynamic>()->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, false);
		myRigidBody->is<PxRigidDynamic>()->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, false);
	}
}

void RigidBody::LockRotationAxis(PxRigidDynamicLockFlag::Enum aLock) const
{
	if(myType == Dynamic)
	{
		myRigidBody->is<PxRigidDynamic>()->setRigidDynamicLockFlag(aLock, true);
	}
}

void RigidBody::UnLockRotationAxis(physx::PxRigidDynamicLockFlag::Enum aLock) const
{
	if(myType == Dynamic)
	{
		myRigidBody->is<PxRigidDynamic>()->setRigidDynamicLockFlag(aLock, false);
	}
}

void RigidBody::SetTransform(TransformBase* aTransform)
{
	myUserData.myTransform = aTransform;
}

void RigidBody::UpdateTransform()
{
	PxTransform transform;
	if(myUserData.myTransform == nullptr) 
	{
		std::cout << "Rigid body doesn't have transform ptr\n";
		return;
	}
	
	transform.p = PxVec3(myUserData.myTransform->GetPosition().x, myUserData.myTransform->GetPosition().y, myUserData.myTransform->GetPosition().z);
	transform.q = PxQuat(myUserData.myTransform->GetRotationQuaternion().x, myUserData.myTransform->GetRotationQuaternion().y, myUserData.myTransform->GetRotationQuaternion().z, myUserData.myTransform->GetRotationQuaternion().w);
	myRigidBody->setGlobalPose(transform);
}

physx::PxRigidActor& RigidBody::GetPhysxActor()
{
	return *myRigidBody;
}

const ActorUserData* RigidBody::GetUserData() const
{
	return &myUserData;
}

void RigidBody::SetQueryFilterData(physx::PxFilterData aData)
{
	myFilterData = aData;
	size_t count = myRigidBody->getNbShapes();
	PxShape** shapes = new PxShape*[count];
	myRigidBody->getShapes(shapes, static_cast<PxU32>(count));
	for(int i = 0; i<count; i++)
	{
		shapes[i]->setQueryFilterData(myFilterData);
	}
	delete[] shapes;
}

physx::PxFilterData RigidBody::GetQueryFilterData() const
{
	return myFilterData;
}

const bool RigidBody::IsValid() const
{
	return myRigidBody != nullptr;
}
