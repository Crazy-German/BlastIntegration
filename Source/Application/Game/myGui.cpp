#include "../FeatherEngine/GameObjectSystem/Scenes/SceneManager.h"

#include "myGui.h"
#include "DearImGui\imgui.h"
#include <DearImGui/imgui_impl_win32.h>
#include "DearImGui/imgui_impl_dx11.h"

#include "GraphicsEngine\GraphicsEngine.h"
#include "d3d11.h"
#include <Windows.h>

#include "GameObjectSystem/Components/Lights/LightManager.h"
#include "GraphicsEngine/DebugBuffer.h"
#include "CommonUtilities/Input.h"

//MyGui::MyGui()
//{
//	myIsActive = false;
//}

MyGui::MyGui(SceneManager& aSceneManager)
	:mySceneManager(aSceneManager)
{
	myIsActive = false;
}

void MyGui::Init(HWND& aWindowHandle, Microsoft::WRL::ComPtr<ID3D11Device> aDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext> aContext)
{
	ImGui::CreateContext();


	ImGui_ImplWin32_Init(aWindowHandle);

	ImGui_ImplDX11_Init(*aDevice.GetAddressOf(), *aContext.GetAddressOf());

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	ReadCustomShaders();


}

void MyGui::Update(float aDeltaTime)
{
	ImGuiIO& io = ImGui::GetIO();
	io.WantCaptureMouse;
	if (myIsActive)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (ImGui::Begin("TA-GUI"))
		{
			if (ImGui::CollapsingHeader("Compile Shaders"))
			{
				for (auto& shader : myShaders)
				{
					if (ImGui::Button(shader.stem().string().c_str()))
					{
						GraphicsEngine::Get().CompileShader(shader);
					}
				}
			}
		}
		ImGui::End();

		if (ImGui::Begin("Debugging"))
		{
			if (ImGui::CollapsingHeader("Active Objects"))
			{
				if (!mySceneManager.GetIsLoading())
				{

				}

			}
		}
		ImGui::End();


		if (ImGui::Begin("GUI"))
		{
			int fps = static_cast<int>(1 / aDeltaTime);
			std::string fpsText = "FPS " + std::to_string(fps);

			ImGui::Text(fpsText.c_str());


			if(ImGui::ColorPicker3("Directional Light", myDirLightColor))
			{
				mySceneManager.GetGlobalObjects().LightManager->GetDirLight()->GetComponent<LightComponent>()->GetLightData()->Color.x = myDirLightColor[0];
				mySceneManager.GetGlobalObjects().LightManager->GetDirLight()->GetComponent<LightComponent>()->GetLightData()->Color.y = myDirLightColor[1];
				mySceneManager.GetGlobalObjects().LightManager->GetDirLight()->GetComponent<LightComponent>()->GetLightData()->Color.z = myDirLightColor[2];
			}

			if (ImGui::SliderFloat("LightIntensity", &myLightIntensity, 0.f, 30.f))
			{
				mySceneManager.GetGlobalObjects().LightManager->GetDirLight()->GetComponent<LightComponent>()->GetLightData()->Intensity = myLightIntensity;
			}
			if (ImGui::SliderFloat("Exposure", &myExposure, 0.f, 1.5f))
			{
#ifndef _RETAIL
				mySceneManager.myDebugBuffer->Exposure = myExposure;
#endif
			}

			if (ImGui::SliderFloat("Vertical Rotation", &myLightRotation.x, 0.f, 1.57079633f))
			{
				myLightRotation.x = std::clamp(myLightRotation.x, 0.f, 1.57079633f);
				CU::Matrix4x4f lightTransformMatrix = {};
				auto meshTransform = mySceneManager.GetGlobalObjects().Player->GetTransform();

				lightTransformMatrix.SetTranslation(meshTransform->GetPosition());
				lightTransformMatrix = lightTransformMatrix * CU::Matrix4x4f::CreateRotationAroundX(myLightRotation.x);
				lightTransformMatrix = lightTransformMatrix * CU::Matrix4x4f::CreateRotationAroundY(myLightRotation.y);
				lightTransformMatrix.SetTranslation((CU::Vector4f(0, 0, -1000 * meshTransform->GetScale().x, 0) * lightTransformMatrix).ToVector3());

				auto light = mySceneManager.GetGlobalObjects().LightManager->GetDirLight();
				auto lightComp = light->GetComponent<LightComponent>();
				if(lightComp != nullptr)
				{
					lightComp->SetLightDirection(lightTransformMatrix.GetRow(3).ToVector3());
					lightComp->SetLightPosition(lightTransformMatrix.GetRow(4).ToVector3());
				}
			}
			if (ImGui::SliderFloat("Horizontal Rotation", &myLightRotation.y, 0.f, 3.14159265359f))
			{
				CU::Matrix4x4f lightTransformMatrix = {};
				auto meshTransform = mySceneManager.GetGlobalObjects().Player->GetTransform();

				lightTransformMatrix.SetTranslation(meshTransform->GetPosition());
				lightTransformMatrix = lightTransformMatrix * CU::Matrix4x4f::CreateRotationAroundX(myLightRotation.x);
				lightTransformMatrix = lightTransformMatrix * CU::Matrix4x4f::CreateRotationAroundY(myLightRotation.y);
				lightTransformMatrix.SetTranslation((CU::Vector4f(0, 0, -1000 * meshTransform->GetScale().x, 0) * lightTransformMatrix).ToVector3());
				auto lightComp = mySceneManager.GetGlobalObjects().LightManager->GetDirLight();
				if (lightComp != nullptr)
				{
					lightComp->GetComponent<LightComponent>()->SetLightDirection(lightTransformMatrix.GetRow(3).ToVector3());
					lightComp->GetComponent<LightComponent>()->SetLightPosition(lightTransformMatrix.GetRow(4).ToVector3());
				}
			}

			if (ImGui::SliderFloat("Cubemap influence", &myCubemapInfluence, 0.f, 2.5f))
			{
#ifndef _RETAIL
				mySceneManager.myDebugBuffer->CubemapInfluence = myCubemapInfluence;
#endif
			}
			
		}
		ImGui::End();
	}
}

void MyGui::Render()
{
	if (myIsActive)
	{
		auto cuMousePos = CU::Input::GetMousePosition();
		ImVec2 mousePos;
		mousePos.x = static_cast<float>(cuMousePos.x);
		mousePos.y = static_cast<float>(cuMousePos.y);
		ImGui::SetCursorPos(mousePos);
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
}

void MyGui::SetActive(bool aIsActive)
{
	myIsActive = aIsActive;
}

bool MyGui::GetActive()
{
	return myIsActive;
}

void MyGui::ReadCustomShaders()
{
	std::filesystem::path shadersPath = "Assets\\VFX\\Shaders";
	for (auto& file : std::filesystem::recursive_directory_iterator(shadersPath))
	{
		if (file.path().has_filename() && file.path().has_extension())
		{
			if (file.path().extension() == ".hlsli")
			{
				continue;
			}
			if (file.path().extension() != ".hlsl")
			{
				continue;
			}
			if (file.path().filename().stem().string().ends_with("_VS"))
			{
				myShaders.emplace_back(file.path());
			}
			else if (file.path().filename().stem().string().ends_with("PS"))
			{
				myShaders.emplace_back(file.path());
			}
			else if (file.path().filename().stem().string().ends_with("GS"))
			{
				myShaders.emplace_back(file.path());
			}

		}
	}
}

