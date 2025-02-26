#include "TransformBase.h"


float constexpr DegToRad = 3.141516f / 180.0f;

TransformBase::TransformBase():myParent(nullptr), myJointParent(nullptr)
{
	myIsDirty = true;
	myScale = { 1.f, 1.f, 1.f };
	myPosition = { 0.f, 0.f, 0.f };
	myRotation.MakeIdentity();
	myHaveCreatedParent = false;
}

TransformBase::TransformBase(CommonUtilities::Vector3f aPosition):myParent(nullptr), myJointParent(nullptr)
{
	myIsDirty = true;
	myPosition = aPosition;
	myRotation.MakeIdentity();
	myScale = { 1.f, 1.f, 1.f };
	RecalculateCache();
}

TransformBase::TransformBase(CommonUtilities::Vector3f aPosition, CommonUtilities::Quaternionf aRotation):myParent(nullptr), myJointParent(nullptr)
{
	myIsDirty = true;
	myPosition = aPosition;
	myRotation = aRotation;
	myScale = { 1.f, 1.f, 1.f };
	myCache = Compose(myPosition, myRotation, myScale);
	myCacheScale = myCache;
	myIsDirty = false;
}

TransformBase::~TransformBase()
{
	if(myHaveCreatedParent && myParent)
	{
		delete myParent;
		myHaveCreatedParent = false;
	}
}

void TransformBase::SetPosition(const CU::Vector3f& aPosition)
{
	myIsDirty = true;
	myPosition = aPosition;
}

void TransformBase::Update(const float& aDeltaTime)
{
	aDeltaTime;
	if(myIsDirty || myParent)
	{
		RecalculateCache();
	}
}

void TransformBase::SetScale(const CU::Vector3f& aScale)
{
	myIsDirty = true;
	myScale = aScale;
}

void TransformBase::SetRotation(const CU::Quaternionf& aQuaternion)
{
	myIsDirty = true;
	myRotation = aQuaternion;
}

void TransformBase::SetRotation(const CU::Vector3f& aRotationXYZDeg)
{
	myIsDirty = true;
	//myRotation.Set(aRotationXYZDeg * DegToRad);
	myRotation.Set(aRotationXYZDeg * DegToRad);
}

void TransformBase::SetRotationRadian(const CU::Vector3f& aRotationXYZRad)
{
	myIsDirty = true;
	myRotation.Set(aRotationXYZRad);
}

void TransformBase::SetTransform(const CU::Matrix4x4f& aTransform)
{
	myCacheScale = aTransform;
	myCache = aTransform;
	myPosition = aTransform.GetRow(4).ToVector3();
}

void TransformBase::AddPosition(const CU::Vector3f& aPosition)
{
	myIsDirty = true;
	myPosition += aPosition;
}

void TransformBase::AddScale(const CU::Vector3f& aScale)
{
	myIsDirty = true;
	myScale += aScale;
}

void TransformBase::AddRotation(const CU::Quaternionf& aQuaternion)
{
	myIsDirty = true;
	myRotation = aQuaternion.GetInverse() * myRotation * aQuaternion;
}

void TransformBase::AddRotation(const CU::Vector3f& aRotationXYZDeg)
{
	myIsDirty = true;
	CU::Vector3f euler = myRotation.GetEuler();
	euler += aRotationXYZDeg * DegToRad;
	myRotation.Set(euler);
}

void TransformBase::AddRotationRadian(const CU::Vector3f& aRotationXYZRad)
{
	myIsDirty = true;
	CU::Vector3f euler = myRotation.GetEuler();
	euler += aRotationXYZRad;
	myRotation.Set(euler);
}

void TransformBase::AddObjectParent(TransformBase* aParent)
{
	if(myHaveCreatedParent && myParent)
	{
		delete myParent;
	}
	myHaveCreatedParent = false;
	myParent = aParent;
}

void TransformBase::AddJointParent(CU::Matrix4x4f* aJoint, CU::Matrix4x4f aBindPose)
{
	myParent = new TransformBase();
	myHaveCreatedParent = true;
	myJointParent = aJoint;
	myJointParentBindPoseInverse = aBindPose;
}

TransformBase* TransformBase::GetParent()
{
	return myParent;
}

CU::Vector3f TransformBase::GetPosition() const
{
	return myPosition;
}

CU::Vector3f TransformBase::GetScale() const
{
	return myScale;
}

CU::Quaternionf TransformBase::GetRotationQuaternion() const
{
	return myRotation;
}

CU::Vector3f TransformBase::GetRotationEuler() const
{
	return myRotation.GetEuler();
}

CU::Matrix3x3f TransformBase::GetRotationMatrix() const
{
	return myCache;
}

CU::Vector3f TransformBase::GetForwardVector()
{
	if (myIsDirty)
	{
		RecalculateCache();
	}
	return myCacheScale.GetRow(3).ToVector3();
}

CU::Vector3f TransformBase::GetUpVector()
{
	if (myIsDirty)
	{
		RecalculateCache();
	}
	return myCacheScale.GetRow(2).ToVector3();
}

CU::Vector3f TransformBase::GetRightVector()
{
	if (myIsDirty)
	{
		RecalculateCache();
	}
	return myCacheScale.GetRow(1).ToVector3();
}

CU::Matrix4x4f TransformBase::GetMatrix()
{
	if (myIsDirty)
	{
		RecalculateCache();
	}
	return myCache;
}

CU::Matrix4x4f& TransformBase::GetMatrixScale()
{
	if (myIsDirty)
	{
		RecalculateCache();
	}
	return myCacheScale;
}

CommonUtilities::Matrix4x4f TransformBase::Compose(const CommonUtilities::Vector3f& aTranslation,
	const CommonUtilities::Quaternionf& aRotation, const CommonUtilities::Vector3f& aScale)
{
		CommonUtilities::Matrix4x4f retval;
        const float x = aRotation.x, y = aRotation.y, z = aRotation.z, w = aRotation.w;
        const float x2 = x + x, y2 = y + y, z2 = z + z;
        const float xx = x * x2, xy = x * y2, xz = x * z2;
        const float yy = y * y2, yz = y * z2, zz = z * z2;
        const float wx = w * x2, wy = w * y2, wz = w * z2;

        const float sx = aScale.x, sy = aScale.y, sz = aScale.z;

        retval(1,1) = (1 - (yy + zz)) * sx;
		retval(2, 1) = (xy + wz);// * sx;
		retval(3, 1) = (xz - wy);// * sx;
        retval(1,4) = 0;
				
		retval(1, 2) = (xy - wz);// * sy;
        retval(2,2) = (1 - (xx + zz)) * sy;
		retval(3, 2) = (yz + wx);// *sy;
        retval(2,4) = 0;
				
		retval(1, 3) = (xz + wy);// *sz;
		retval(2, 3) = (yz - wx);// *sz;
        retval(3,3) = (1 - (xx + yy)) * sz;
        retval(3,4) = 0;
				
        retval(4,1) = aTranslation.x;
        retval(4,2) = aTranslation.y;
        retval(4,3) = aTranslation.z;
        retval(4,4) = 1;
	return retval;
}


void TransformBase::RecalculateCache()
{

	myCache = Compose(myPosition, myRotation, {1,1,1});
	if(myParent)
	{
		//myRotation = myRotation*myParent->GetRotationQuaternion(); DID Not work
		CommonUtilities::Matrix4x4f jointworld = CommonUtilities::Matrix4x4f();
		if(myJointParent)
		{
			jointworld= myJointParentBindPoseInverse.GetInverse()*(*myJointParent);
		}
		jointworld *= (myParent->GetMatrix());
		//myCache = (myParent->GetMatrix())*myCache;
		myCache = myCache*jointworld;
		
		//myCacheScale = myCache;//(myJointParent->GetInverse())*(myParent->GetMatrixScale())*myCacheScale;
	}
	myCacheScale = myCache;
	myCacheScale.SetScale(myScale);
	myIsDirty = false;
}
