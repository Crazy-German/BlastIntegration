#include "FeatherEngine.pch.h"
#include "LightManager.h"


#include "GraphicsEngine/GfxCmd_SetLightBuffer.h"
#include "GraphicsEngine/GfxCmd_SetFrameBuffer.h"
#include "MainSingleton.h"
#include "GameObjectSystem\Components\ShadowCameraComponent.h"


#include <algorithm>


struct LightSorting
{
	LightSorting() = delete;
	LightSorting(std::shared_ptr<GameObject> aLightSource, const float aDistance)
	{
		Light = aLightSource;
		DistanceSqr = aDistance;
	}
	float DistanceSqr;
	std::shared_ptr<GameObject> Light;
};

bool FrontToBackLightSort(LightSorting aFirst, LightSorting aSecond)
{
	return (aSecond.DistanceSqr > aFirst.DistanceSqr);
}


LightManager::LightManager()
{
	MainSingleton::Get().GetInputMapper().Subscribe(ActionEventID::Toggle_DirLight, this);
	MainSingleton::Get().GetInputMapper().Subscribe(ActionEventID::Toggle_PointLight, this);
	MainSingleton::Get().GetInputMapper().Subscribe(ActionEventID::Toggle_SpotLight, this);
}

LightManager::~LightManager()
{
	MainSingleton::Get().GetInputMapper().UnRegisterFromAll(this);
}


void LightManager::AddLight(LightType aLightType, std::shared_ptr<GameObject> aLightSource)
{
	assert(aLightSource != nullptr && "Trying to add nullptr to lightcomponent in LightManager!");
	switch (aLightType)
	{
	case LightType::DirectionalLight:
		myDirLight = aLightSource;
		myLightBuffer.DirLight = *myDirLight->GetComponent<LightComponent>()->GetLightData();
		return;
	case LightType::PointLight:
		myPointLights.emplace_back(std::move(aLightSource));
		return;
	//case LightType::SpotLight:
	//	mySpotLights.emplace_back(std::move(aLightSource));
	//	return;
	default:
		break;
	}
	return;
}

void LightManager::InitLights()
{
	if (!myDirLight)
	{
		std::cout << "No directional light in scene! Adding a default one.\n";
		myDirLight = std::make_shared<GameObject>();myDirLight->AddComponent(ComponentSystem::Get()->AddComponent<LightComponent, LIGHT_COMPONENT>());
		myDirLight->GetComponent<LightComponent>()->GetLightData()->Active = true;
		myDirLight->GetComponent<LightComponent>()->InitDirLightMatrices(CU::Matrix4x4f());
	}
	myPointSortingList.reserve(myPointLights.size());
	for (auto& pointLight : myPointLights)
	{
		myPointSortingList.emplace_back(pointLight, 0.0f);
	}
	//mySpotSortingList.reserve(mySpotLights.size());
	//for (auto& spotLight : mySpotLights)
	//{
	//	mySpotSortingList.emplace_back(spotLight, 0.0f);
	//}

	for (int index = 0; index < myPointLights.size(); index++)
	{
		if (!myPointLights[index])
		{
			continue;
		}
		myLightBuffer.PointLights[index] = *myPointLights[index]->GetComponent<LightComponent>()->GetLightData();
		myLightBuffer.PointLightCount++;
		if (myLightBuffer.PointLightCount >= 8)
		{
			break;
		}
	}

	//for (int index = 0; index < mySpotLights.size(); index++)
	//{
	//	if (!mySpotLights[index])
	//	{
	//		continue;
	//	}
	//	myLightBuffer.SpotLights[index] = *mySpotLights[index]->GetComponent<LightComponent>()->GetLightData();
	//	myLightBuffer.SpotLightCount++;
	//	if (myLightBuffer.SpotLightCount >= 4)
	//	{
	//		break;
	//	}
	//}

	//for (size_t index = 0; index < myPointLights.size(); index++)
	//{
	//	if (!myPointLights[index])
	//	{
	//		continue;
	//	}
	//	switch (static_cast<LightType>(myPointLights[index]->GetComponent<LightComponent>()->GetLightType()))
	//	{
	//	case LightType::PointLight:
	//		if (myLightBuffer.PointLightCount <= 3)
	//		{
	//			myLightBuffer.PointLights[myLightBuffer.PointLightCount] = *myPointLights[index]->GetComponent<LightComponent>()->GetLightData();
	//			myLightBuffer.PointLightCount++;
	//		}
	//		break;
	//		//case LightType::SpotLight:
	//		//	if (myLightBuffer.SpotLightCount <= 3)
	//		//	{
	//		//		myLightBuffer.SpotLights[myLightBuffer.SpotLightCount] = *mySpotLights[index]->GetComponent<LightComponent>()->GetLightData();
	//		//		myLightBuffer.SpotLightCount++;
	//		//	}
	//		//	break;
	//
	//	default:
	//		break;
	//	}
	//}
}

void LightManager::Reset()
{
	if (myDirLight)
	{
		myDirLight->Destroy();
	}
	myDirLight = nullptr;
	//TEMP
	for (int index = 0; index < myPointLights.size(); index++)
	{
		if (myPointLights[index])
		{
			myPointLights[index]->Destroy();
		}
		myPointLights[index] = nullptr;
	}
	for (int spotLightIndex = 0; spotLightIndex < mySpotLights.size(); spotLightIndex++)
	{
		mySpotLights[spotLightIndex]->Destroy();
	}
	myLightBuffer.PointLightCount = 0;
	//myLightBuffer.SpotLightCount = 0;
	myPointLights.resize(1);
	mySpotLights.clear();
	myPointSortingList.clear();
	mySpotSortingList.clear();
}

void LightManager::Update(const CU::Vector3f& aPlayerPosition)
{
	CommonUtilities::Vector3f pos = aPlayerPosition-myDirLight->GetComponent<LightComponent>()->GetLightData()->LightDir*3000.f;
 	myDirLight->GetComponent<LightComponent>()->GetLightData()->LightViewInv = myDirLight->GetComponent<LightComponent>()->GetLightData()->LightViewInv.CreateLookAt(pos, pos+myDirLight->GetComponent<LightComponent>()->GetLightData()->LightDir*1000.f, {0,1,0});
	
	myDirLight->GetComponent<LightComponent>()->GetLightData()->LightViewInv = myDirLight->GetComponent<LightComponent>()->GetLightData()->LightViewInv.GetFastInverse();
	
	myLightBuffer.DirLight = *myDirLight->GetComponent<LightComponent>()->GetLightData();

	LightCulling(aPlayerPosition);
	for (int pointIndex = 0; pointIndex < myLightBuffer.PointLightCount; pointIndex++)
	{
		myLightBuffer.PointLights[pointIndex] = *myPointSortingList[pointIndex].Light->GetComponent<LightComponent>()->GetLightData();
	}
}

void LightManager::Render()
{
	auto& renderer = RENDERER;
	renderer.Enqueue<GfxCmd_SetLightBuffer>(myLightBuffer);

}

std::shared_ptr<GameObject> LightManager::GetDirLight()
{
	return myDirLight;
}

void LightManager::RenderDirlightShadows()
{
	if (myDirLight->GetComponent<ShadowCameraComponent>())
	{
		myDirLight->GetComponent<ShadowCameraComponent>()->Render();
	}
}


void LightManager::RecieveEvent(const ActionEvent& anEvent)
{
	switch (anEvent.Id)
	{
	case ActionEventID::Toggle_DirLight:
		if (myLightBuffer.DirLight.Active)
		{
			myDirLight->GetComponent<LightComponent>()->GetLightData()->Active = false;
			return;
		}
		myDirLight->GetComponent<LightComponent>()->GetLightData()->Active = true;
		return;
	case ActionEventID::Toggle_PointLight:
		for (size_t index = 0; index < myLightBuffer.PointLightCount; index++)
		{
			if (myLightBuffer.PointLights[index].Active)
			{
				myLightBuffer.PointLights[index].Active = false;
				continue;
			}
			myLightBuffer.PointLights[index].Active = true;
			continue;
		}
		return;
	//case ActionEventID::Toggle_SpotLight:
	//	for (size_t index = 0; index < myLightBuffer.SpotLightCount; index++)
	//	{
	//		if (myLightBuffer.SpotLights[index].Active)
	//		{
	//			myLightBuffer.SpotLights[index].Active = false;
	//			continue;
	//		}
	//		myLightBuffer.SpotLights[index].Active = true;
	//		continue;
	//	}
	//	return;
	default:
		break;
	}
}

LightBuffer* LightManager::GetLightBuffer()
{
	return &myLightBuffer;
}

void LightManager::LightCulling(const CU::Vector3f& aPlayerPosition)
{
	if (myPointLights.size() > 8) // we're using spotlight-slots to pointlights!
	{
		for (auto& pointSort : myPointSortingList)
		{
			pointSort.DistanceSqr = (aPlayerPosition - pointSort.Light->GetTransform()->GetMatrixScale().GetTranslation()).LengthSqr();
		}
		std::sort(myPointSortingList.begin(), myPointSortingList.end(), FrontToBackLightSort);
	}

	if (mySpotLights.size() > 4)
	{
		for (auto& spotSort : myPointSortingList)
		{
			spotSort.DistanceSqr = (aPlayerPosition - spotSort.Light->GetTransform()->GetMatrixScale().GetTranslation()).LengthSqr();
		}
		std::sort(mySpotSortingList.begin(), mySpotSortingList.end(), FrontToBackLightSort);
	}
}

