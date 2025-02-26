#include "FeatherEngine.pch.h"
#include "Transform.h"

float constexpr DegToRad = 3.141516f / 180.0f;

//Transform::Transform()
//{
//	myIsDirty = true;
//	myScale = { 1.f, 1.f, 1.f };
//	myPosition = { 0.f, 0.f, 0.f };
//	myRotation.MakeIdentity();
//}
//
//void Transform::SetPosition(const CU::Vector3f& aPosition, const bool& aIsUsingRadians)
//{
//	myIsDirty = true;
//	myPosition = aPosition;
//	myIsUsingRadians = aIsUsingRadians;
//}
//
void Transform::Update(const float& aDeltaTime)
{
	aDeltaTime;
	if(myIsDirty || myParent)
	{
		RecalculateCache();
	}
}
//
//void Transform::SetScale(const CU::Vector3f& aScale)
//{
//	myIsDirty = true;
//	myScale = aScale;
//}
//
//void Transform::SetRotation(const CU::Quaternionf& aQuaternion)
//{
//	myIsDirty = true;
//	myRotation = aQuaternion;
//}
//
//void Transform::SetRotation(const CU::Vector3f& aRotationXYZDeg, bool aIsUsingRadians)
//{
//	myIsDirty = true;
//	myIsUsingRadians = aIsUsingRadians;
//	//myRotation.Set(aRotationXYZDeg * DegToRad);
//	myRotationXYZ = aRotationXYZDeg;
//}
//
//void Transform::SetTransForm(const CU::Matrix4x4f& aTransform)
//{
//	myCacheScale = aTransform;
//	myCache = aTransform;
//	myPosition = aTransform.GetRow(4).ToVector3();
//}
//
//
//void Transform::AddPosition(const CU::Vector3f& aPosition, const bool& aIsUsingRadians)
//{
//	myIsUsingRadians = aIsUsingRadians;
//	myIsDirty = true;
//	myPosition += aPosition;
//}
//
//void Transform::AddScale(const CU::Vector3f& aScale)
//{
//	myIsDirty = true;
//	myScale += aScale;
//}
//
//void Transform::AddRotation(const CU::Quaternionf& aQuaternion)
//{
//	myIsDirty = true;
//	myRotation = aQuaternion.GetInverse() * myRotation * aQuaternion;
//}
//
//void Transform::AddRotation(const CU::Vector3f& aRotationXYZDeg)
//{
//	myIsDirty = true;
//	//myRotation *= CU::Quaternionf(aRotationXYZDeg * DegToRad);
//	myRotationXYZ += aRotationXYZDeg;
//}
//
//void Transform::AddObjectParent(Transform* aParent)
//{
//	myParent = aParent;
//}
//
//void Transform::AddJointParent(CU::Matrix4x4f* aJoint, CU::Matrix4x4f aBindPose)
//{
//	myJointParent = aJoint;
//	myJointParentBindPoseInverse = aBindPose;
//}
//
//CU::Vector3f Transform::GetPosition() const
//{
//	return myPosition;
//}
//
//CU::Vector3f Transform::GetScale() const
//{
//	return myScale;
//}
//
//CU::Quaternionf Transform::GetRotationQuaternion() const
//{
//	return myRotation;
//}
//
//CU::Vector3f Transform::GetRotationEuler() const
//{
//	return myRotationXYZ;
//}
//
//CU::Vector3f Transform::GetForwardVector()
//{
//	if (myIsDirty)
//	{
//		RecalculateCache();
//	}
//	return myCacheScale.GetRow(3).ToVector3();
//}
//
//CU::Vector3f Transform::GetUpVector()
//{
//	if (myIsDirty)
//	{
//		RecalculateCache();
//	}
//	return myCacheScale.GetRow(2).ToVector3();
//}
//
//CU::Vector3f Transform::GetRightVector()
//{
//	if (myIsDirty)
//	{
//		RecalculateCache();
//	}
//	return myCacheScale.GetRow(1).ToVector3();
//}
//
//CU::Matrix4x4f Transform::GetMatrix()
//{
//	if (myIsDirty)
//	{
//		RecalculateCache();
//	}
//	return myCache;
//}
//
//CU::Matrix4x4f& Transform::GetMatrixScale()
//{
//	if (myIsDirty)
//	{
//		RecalculateCache();
//	}
//	return myCacheScale;
//}
//
//CommonUtilities::Matrix4x4f Transform::Compose(const CommonUtilities::Vector3f& aTranslation,
//	const CommonUtilities::Quaternionf& aRotation, const CommonUtilities::Vector3f& aScale)
//{
//		CommonUtilities::Matrix4x4f retval;
//        const float x = aRotation.x, y = aRotation.y, z = aRotation.z, w = aRotation.w;
//        const float x2 = x + x, y2 = y + y, z2 = z + z;
//        const float xx = x * x2, xy = x * y2, xz = x * z2;
//        const float yy = y * y2, yz = y * z2, zz = z * z2;
//        const float wx = w * x2, wy = w * y2, wz = w * z2;
//
//        const float sx = aScale.x, sy = aScale.y, sz = aScale.z;
//
//        retval(1,1) = (1 - (yy + zz)) * sx;
//        retval(1,2) = (xy + wz) * sx;
//        retval(1,3) = (xz - wy) * sx;
//        retval(1,4) = 0;
//				
//        retval(2,1) = (xy - wz) * sy;
//        retval(2,2) = (1 - (xx + zz)) * sy;
//        retval(2,3) = (yz + wx) * sy;
//        retval(2,4) = 0;
//				
//        retval(3,1) = (xz + wy) * sz;
//        retval(3,2) = (yz - wx) * sz;
//        retval(3,3) = (1 - (xx + yy)) * sz;
//        retval(3,4) = 0;
//				
//        retval(4,1) = aTranslation.x;
//        retval(4,2) = aTranslation.y;
//        retval(4,3) = aTranslation.z;
//        retval(4,4) = 1;
//	return retval;
//}
//
//
//void Transform::RecalculateCache()
//{
//	//Todo: Temporary
//	if (myIsUsingRadians)
//	{
//		myRotation.Set(myRotationXYZ);
//		myIsUsingRadians = false;
//	}
//	else
//	{
//		myRotation.Set(myRotationXYZ * DegToRad);
//	}
//
//	myCacheScale = {};
//
//	CU::Matrix4x4f rotation = myRotation.GetMatrix();
//	myCacheScale *= rotation;
//	myCache = rotation;
//
//	myCache = Compose(myPosition, myRotation, {1,1,1});
//	if(myParent)
//	{
//		//myRotation = myRotation*myParent->GetRotationQuaternion(); DID Not work
//		CommonUtilities::Matrix4x4f jointworld = CommonUtilities::Matrix4x4f();;
//		if(myJointParent)
//		{
//			jointworld= myJointParentBindPoseInverse.GetInverse()*(*myJointParent);
//		}
//		jointworld *= (myParent->GetMatrix());
//		//myCache = (myParent->GetMatrix())*myCache;
//		myCache = myCache*jointworld;
//		
//		myCacheScale = myCache;//(myJointParent->GetInverse())*(myParent->GetMatrixScale())*myCacheScale;
//	}
//	myCacheScale.SetScale(myScale);
//	myIsDirty = false;
//}
