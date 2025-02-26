#pragma once
#include "../Component.h"
#include "CommonUtilities/Vector.hpp"
#include "CommonUtilities/Matrix4x4.hpp"
struct LightBuffer;
struct DirLightData;
struct PointLightData;
struct SpotLightData;
struct LightData;

#define DEFAULT_DIRLIGHT_XROTATION 1.2f
#define DEFAULT_DIRLIGHT_YROTATION 1.3f
#define DEFAULT_DIRLIGHT_BOUNDS 10000.0f
#define LIGHT_COMPONENT 100

struct LightBounds
{
	float Radius = 0.f;
	CU::Vector3<float> Origin;
};

enum class LightType
{
	DirectionalLight,
	PointLight,
	SpotLight
};
class LightComponent : public Component
{
public:
	LightComponent() = default;
	LightComponent(const LightComponent& aLightComponent) = default;
	~LightComponent() override = default;

	void Awake() override;
	void Update(const float& aDeltaTime) override;

	void Initialize(const CU::Vector3f& aColor, float aIntensity, const CU::Vector3f& aLightDirection, 
		const CU::Vector3f& aLightPosition, float aAngleDeg, LightType aLightMode, float anAttenuationRadius);
	void InitDirLightMatrices(const CU::Matrix4x4f& aPlayerTransform, float aLightBoundsRadius = DEFAULT_DIRLIGHT_BOUNDS);
	void SetColor(const CU::Vector3f& aColor);
	void SetIntensity(float aIntensity);
	void SetLightDirection(const CU::Vector3f& aLightDirection);
	void SetLightPosition(const CU::Vector3f& aLightPosition);
	void SetConeAngle(float aAngleDeg);
	void SetLightType(LightType aLightMode);
	void SetLightProj(CU::Matrix4x4f aLightProj);
	void SetLightViewInverse(CU::Matrix4x4f aLightViewInverse);

	std::shared_ptr<LightData> GetLightData() const;

	int GetLightType() const;

	template<class T>
	T GetLightData() const;

	void Render();

private:
	std::shared_ptr<LightData> myLightData;
	LightType myLightType = LightType::DirectionalLight;
};

