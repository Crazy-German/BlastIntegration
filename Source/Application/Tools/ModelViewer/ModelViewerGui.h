#pragma once
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "DearImGui/imgui.h"
#include "Windows.h"
#include "CommonUtilities/Vector.hpp"

class Scene;
class Mesh;
class Texture;
class ModelViewer;
struct ID3D11Device;
struct ID3D11DeviceContext;

class ModelViewerGui
{
public:
	friend ModelViewer;

	ModelViewerGui();
	~ModelViewerGui();
	 
	void Initialize(HWND& aHWND, ID3D11Device* aDevice, ID3D11DeviceContext* aContext, ModelViewer* aModelViewer);
	void Update();
	void Render();

	void LoadAsset(const std::filesystem::path& aPath);
	void ResetScene();
	void SnapToFloor();
	bool IsPowerOfTwo(std::shared_ptr<Texture> aTexture);
	void UnloadAssets();

private:
	ImFont* myNormalFont = nullptr;
	std::string myAlbedoTexturePath;
	std::string myNormalTexturePath;
	std::string myMaterialTexturePath;
	std::string myActiveName;

	CU::Vector3f myLightColor;
	float myLightIntensity;
	CU::Vector3f myLightRotation;

	CU::Vector3f myPosition;
	CU::Vector3f myRotation;
	CU::Vector3f myScale;

	std::vector<std::string> myLogMessages;

	bool myLoadAnimations;
	std::shared_ptr<Mesh> myCubeMesh;
	std::shared_ptr<Mesh> myVerticalPlaneMesh;
	std::shared_ptr<Mesh> mySphere;
	std::vector<std::string> myAnimationNames;

	ModelViewer* myModelViewer;

	bool myActiveAnimation = false;
	bool myActiveMesh = false;
	bool myActiveTexture = false;
	bool myActiveFloor = true;
};

