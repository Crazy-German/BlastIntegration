#pragma once
#include <windef.h>
#include <wrl.h>
#include "CommonUtilities/Vector2.hpp"

struct ID3D11Device;
struct ID3D11Context;
struct ID3D11DeviceContext;
class SceneManager;

class MyGui
{
public:
	MyGui(SceneManager& aSceneManager);
	~MyGui() = default;
	void Init(HWND& aWindowHandle, Microsoft::WRL::ComPtr<ID3D11Device> aDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext> aContext);
	void Update(float aDeltaTime);
	void Render();
	void SetActive(bool aIsActive);
	bool GetActive();
private:
	void ReadCustomShaders();
private:
	bool myIsActive;
	SceneManager& mySceneManager;
	std::vector<std::filesystem::path> myShaders;
	CU::Vector2f myLightRotation = { 1.2f, 1.3f };
	float myLightIntensity = 4.0f;
	float myExposure = 0.2f;
	float myDirLightColor[3];

	float myCubemapInfluence = 1;
};