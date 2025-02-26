#pragma once
#include "Vector3.hpp"
#include "Matrix4x4.hpp"
#include "Quaternion.h"


class TransformBase
{
public:
	TransformBase();
	TransformBase(CommonUtilities::Vector3f aPosition);
	TransformBase(CommonUtilities::Vector3f aPosition, CommonUtilities::Quaternionf aRotation);
	~TransformBase();

	void SetPosition(const CU::Vector3f& aPosition);
	void Update(const float& aDeltaTime);

	void SetScale(const CU::Vector3f& aScale);
	void SetRotation(const CU::Quaternionf& aQuaternion);
	void SetRotation(const CU::Vector3f& aRotationXYZDeg);
	void SetRotationRadian(const CU::Vector3f& aRotationXYZRad);
	void SetTransform(const CU::Matrix4x4f& aTransform);

	void AddPosition(const CU::Vector3f& aPosition);
	void AddScale(const CU::Vector3f& aScale);
	void AddRotation(const CU::Quaternionf& aQuaternion);
	void AddRotation(const CU::Vector3f& aRotationXYZDeg);
	void AddRotationRadian(const CU::Vector3f& aRotationXYZRad);

	void AddObjectParent(TransformBase* aParent);
	void AddJointParent(CU::Matrix4x4f* aJoint, CU::Matrix4x4f aBindPose);

	TransformBase* GetParent();

	CU::Vector3f GetPosition() const;
	CU::Vector3f GetScale() const;
	CU::Quaternionf GetRotationQuaternion() const;
	//Returns in radians
	CU::Vector3f GetRotationEuler() const;
	CU::Matrix3x3f GetRotationMatrix() const;

	CU::Vector3f GetForwardVector();
	CU::Vector3f GetUpVector();
	CU::Vector3f GetRightVector();

	CU::Matrix4x4f GetMatrix();
	CU::Matrix4x4f& GetMatrixScale();

	static CommonUtilities::Matrix4x4f Compose(const CommonUtilities::Vector3f& aTranslation, const CommonUtilities::Quaternionf& aRotation, const CommonUtilities::Vector3f& aScale);

	void RecalculateCache();
protected:


	CU::Vector3f myPosition;
	CU::Vector3f myScale;
	CU::Quaternionf myRotation;

	TransformBase* myParent;
	CU::Matrix4x4f* myJointParent;
	CU::Matrix4x4f myJointParentBindPoseInverse;

	bool myIsDirty = false;

	//Todo find better solution for this
	// bool checking if have created parent self

	bool myHaveCreatedParent;

	CU::Matrix4x4f myCache;
	CU::Matrix4x4f myCacheScale;
};

