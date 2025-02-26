#pragma once
#include "Component.h"
#include "CommonUtilities/Matrix4x4.hpp"
#include "CommonUtilities/Vector2.hpp"

struct FrameBuffer;

#define SHADOW_CAMERA_COMPONENT 10
class ShadowCameraComponent : public Component
{
public:
	ShadowCameraComponent() = default;
	~ShadowCameraComponent() override = default;

	void Awake() override;
	void Render() override;

	void Init(const CU::Matrix4x4<float>& aViewInverse, const CU::Matrix4x4<float>& aProjection, const CU::Vector3<float>& aPosition);
	void CreateOrthographic(const CU::Vector2f aResolution, const float aNearPlane, const float aFarPlane);
	void CreatePerspective(const float aNearPlane, const float aFarPlane);
	void CreateShadowCube();

	CommonUtilities::Matrix4x4f GetProjection();
private:
	std::shared_ptr<FrameBuffer> GetFrameBuffer();

public:
	void DebugRender() override;

private:
	CU::Matrix4x4f myProjection;
	std::shared_ptr<FrameBuffer> myFrameBuffer;
};

