#pragma once

#include <memory>
#include <vector>
#include "LightComponent.h"
#include "GraphicsEngine\LightBuffer.h"
#include "CommonUtilities\InputObserver.h"

class GameObject;
struct LightSorting;

class LightManager : public InputObserver
{
public:
	LightManager();
	~LightManager();

	void AddLight(LightType aLightType, std::shared_ptr<GameObject> aLightSource);

	void InitLights();

	void Reset();
	void Update(const CU::Vector3f& aPlayerPosition);

	void Render();

	std::shared_ptr<GameObject> GetDirLight();

	void RenderDirlightShadows();

	void RecieveEvent(const ActionEvent& anEvent) override;

	LightBuffer* GetLightBuffer();

private:
	void LightCulling(const CU::Vector3f& aPlayerPosition);

private:
	LightBuffer myLightBuffer;
	std::shared_ptr<GameObject> myDirLight = nullptr;
	std::vector<std::shared_ptr<GameObject>> myPointLights;
	std::vector<std::shared_ptr<GameObject>> mySpotLights;
	std::vector<LightSorting> myPointSortingList;
	std::vector<LightSorting> mySpotSortingList;
	float myCullingRadius = 1000.0f;
};
