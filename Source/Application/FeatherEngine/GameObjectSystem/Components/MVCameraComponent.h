#pragma once
#include "Component.h"
#include "CommonUtilities/InputObserver.h"
#include "GraphicsEngine/FrameBuffer.h"
#include "CommonUtilities/Matrix.hpp"
#include "CommonUtilities/Vector.hpp"
#include "CommonUtilities\PlaneVolume.hpp"


enum class DebugPass
{
	Game,
	Albedo,
	Occlusion,
	Roughness,
	Metalness,
	Normal,
	VertexColor,
	VertexNormal,
	UV,
	Emission,
	Count
};

struct FrameBuffer;
class Texture;
class DebugLineObject;

#define MV_CAMERA_COMPONENT 5
class MVCameraComponent : public Component, public InputObserver
{
public:
	MVCameraComponent();
	~MVCameraComponent() override;

	void Awake() override;

	void CreateProjection(const float aHorizontalFOV, const float aAspectRatio, const float aNearPlane, const float aFarPlane);

	void InitFrustum();

	const CU::Matrix4x4f& GetProjection() { return myProjection; }

	void Update(const float& aDeltaTime) override;

	void Render() override;

	void DebugRender() override;

	void UpdateTransform(CU::Vector2f aDeltaRotation);

	//void GoToCutscene(float aTimeToBlend);
	//void GoToGameplay(float aTimeToBlend);

	void SetSpeed(float aSpeedValue);
	void AddSpeed(float aSpeedValue);

	const CU::Matrix4x4f& GetViewMatrix();

	std::shared_ptr<FrameBuffer> GetFrameBuffer();

	CU::Vector3f ToWorldSpace(const CU::Vector3f& aCameraSpacePosition) const;

	bool IsInsideFrustum(const CU::Vector3f& aWorldMin, const CU::Vector3f& aWorldMax);

	void RecieveEvent(const ActionEvent& anEvent) override;  // Used for cycling debug rendering

	bool IsDebugRendering() const;

	void SetCamJoint(CU::Matrix4x4f* aCamJoint);

	void AddYRotation(float aRotation);

	void AddMist(float aDeltaMistModifier);
	void SetMist(float aMistModifier);

private:
	void UpdateFrustum();


private:
	CU::Matrix4x4f myProjection;
	std::shared_ptr<FrameBuffer> myFrameBuffer;
	std::shared_ptr<CU::PlaneVolume<float>> myFrustum;
	float myAspectRatio = 0.0f;
	float myFOVRadians = 0.0f;
	float myNearPlaneDistance = 0.0f;
	float myFarPlaneDistance = 0.0f;

	float myMoveSpeed = 0.0f;
	float myRotationSpeed = 10.f;

	DebugPass myPass;

	//temp stuff?
	float myDeltaTime = 0.0f;
	float myTotalTime = 0.0f;

	std::shared_ptr<DebugLineObject> myDebugLines;
	CU::Matrix4x4f myStartTransform;
	bool myFrustumInitalized = false;
	CU::Vector3f myRotation;

	bool myCanRotateCam;

	//For cutscene & gameplaycam and blending between them
	CU::Matrix4x4f* myGameJoint;
	CU::Vector2f myGameJointRot;

	CU::Matrix4x4f* myAnimJoint;

	float myBlendProgress;
};

