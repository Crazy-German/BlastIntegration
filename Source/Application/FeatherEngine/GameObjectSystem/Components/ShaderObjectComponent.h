#pragma once

#include "Component.h"
#include "External\nlohmann\json.hpp"

class GameObject;
struct MiscBuffer;

struct ShaderObjectSettings
{
	float StartDelay = 0.0f;
	float LifeTime = 0.0f;
	bool HasBoneParent = false;
	bool HasParent = false;
	bool IsTriggered = false;
	bool Retrigger = false;
};

#define SHADER_OBJECT_COMPONENT 350
class ShaderObjectComponent : public Component
{
public:
	ShaderObjectComponent() = default;
	~ShaderObjectComponent() = default;

	void Awake() override;
	void Init(const float aDelay, const float aLifeTime, const char* aMesh, std::string_view aPixelShader, std::string_view aVertexShader = "");
	void InitFromJson(nlohmann::json& aJsonObject);
	void AddEventTrigger(eLetterRegister aLetter);
	void Destroy() override;

	void Update(const float& aDeltaTime);
	void Render() override;

	void Start();

	bool GetIsActive();
	void SetIdentifierIndex(int aIndex);
	void Put(Letter aLetter) override;
private:
	void SetPositionOffset(const CU::Vector3f& aPositionOffset);
	void SetTransformOffset(const CU::Matrix4x4f& aTransformOffset);

private:
	ShaderObjectSettings mySettings;
	std::shared_ptr<MiscBuffer> myMiscBuffer;
	std::string myName = "";
	int myIdentifierIndex = -1;
	eLetterRegister myEventTrigger = eLetterRegister::_Last;
	float myTimer = 0.0f;
	bool myIsActive = false;
	bool myHasTriggered = false;

};

