#include "ModelViewerGui.h"
#include "d3d11.h"

#include "DearImGui/imgui_impl_win32.h"
#include "DearImGui/imgui_impl_dx11.h"
#include "GraphicsEngine\GraphicsCommandList.h"
#include "GfxCmd_RenderGui.h"
#include "MainSingleton.h"

#include "ModelViewer.h"

#include "AssetManagement/AssetManager.h"
#include "AssetManagement/AssetTypes/MaterialAsset.h"

#include "GameObjectSystem/GameObject.h"
#include "GameObjectSystem/Components/MeshComponent.h"
#include "GameObjectSystem/Components/Animations/Animator.h"
#include "GameObjectSystem/Components/Lights/LightComponent.h"
#include "GameObjectSystem/Scenes/SceneManager.h"

#include "GraphicsEngine/Material.h"
#include "GraphicsEngine/Texture.h"
#include "GraphicsEngine/Mesh.h"

enum class GameObjects
{
	Floor,
	Model
};

ModelViewerGui::ModelViewerGui()
{
	myModelViewer = nullptr;

	myPosition = { 0.f, 0.f, 0.f };
	myRotation = { 0.f, 0.f, 0.f };
	myScale = { 1.f, 1.f, 1.f };

	myLightColor = { 1.f, 1.f, 1.f };
	myLightIntensity = 10.f;
	myLightRotation = { 0.f, 0.f, 0.f };

	myAlbedoTexturePath = "Default";
	myMaterialTexturePath = "Default";
	myNormalTexturePath = "Default";

	myLoadAnimations = false;
	myCubeMesh = nullptr;
	myVerticalPlaneMesh = nullptr;
	mySphere = nullptr;
}

ModelViewerGui::~ModelViewerGui()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ModelViewerGui::Initialize(HWND& aHWND, ID3D11Device* aDevice, ID3D11DeviceContext* aContext, ModelViewer* aModelViewer)
{
	myModelViewer = aModelViewer;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui_ImplWin32_Init(aHWND);
	ImGui_ImplDX11_Init(aDevice, aContext);

	myCubeMesh = std::make_shared<Mesh>();
	myCubeMesh->GenerateCube(100.f);
	myVerticalPlaneMesh = std::make_shared<Mesh>();
	myVerticalPlaneMesh->GenerateVerticalPlane(100.f);
	mySphere = std::make_shared<Mesh>();
	mySphere->GenerateSphere(50, 360);
}

void ModelViewerGui::Update()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

#pragma region getting started
	ImVec2 menuSize = { 710, 70 };
	if (!myActiveMesh && !myActiveAnimation && !myActiveTexture)
	{
		ImGui::SetNextWindowSize(menuSize);
		if (ImGui::Begin("Getting Started"))
		{
			ImGui::Text("Drag and drop files to load meshes, animations or textures");
			ImGui::Text("Press F6 to cycle through the debug shaders or use the menu to instantly watch your desired shaders");
		}
		ImGui::End();
	}
#pragma endregion


#pragma region log
	if (ImGui::Begin("Log"))
	{
		for (auto& message : myLogMessages)
		{
			ImGui::Text(message.c_str());
		}
	}
	ImGui::End();

#pragma endregion

#pragma region Scene
	menuSize = { 550, 600 };
	ImGui::SetNextWindowSize(menuSize);
	if (ImGui::Begin("Scene"))
	{
		ImVec2 buttonSize = { 120, 20.f };

		if (ImGui::Button("Toggle floor", buttonSize))
		{
			myActiveFloor = !myActiveFloor;
			MainSingleton::Get().GetSceneManager().GetScene().Objects[static_cast<unsigned>(GameObjects::Floor)]->SetEnabled(myActiveFloor);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset Scene", buttonSize))
		{
			ResetScene();
		}
		ImGui::SameLine();
		if(ImGui::Button("Unload Assets", buttonSize))
		{
			UnloadAssets();
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear Log", buttonSize))
		{
			myLogMessages.clear();
		}
		if (ImGui::CollapsingHeader("Light"))
		{
			float color[3] = { myLightColor.x, myLightColor.y, myLightColor.z };
			if (ImGui::ColorEdit3("Color##LightColor", color))
			{
				myLightColor = { color[0], color[1], color[2] };
				MainSingleton::Get().GetSceneManager().GetScene().ShadowCamera->GetComponent<LightComponent>()->SetColor(myLightColor);
			}
			if (ImGui::SliderFloat("Intensity", &myLightIntensity, 0.f, 100.f))
			{
				MainSingleton::Get().GetSceneManager().GetScene().ShadowCamera->GetComponent<LightComponent>()->SetIntensity(myLightIntensity);
			}
			if (ImGui::SliderFloat("Vertical Rotation", &myLightRotation.x, 0.f, 90.f))
			{
				myLightRotation.x = std::clamp(myLightRotation.x, 10.f, 90.f);
				auto lightTransform = MainSingleton::Get().GetSceneManager().GetScene().ShadowCamera->GetTransform();
				auto meshTransform = MainSingleton::Get().GetSceneManager().GetScene().Objects[static_cast<unsigned>(GameObjects::Model)]->GetTransform();

				lightTransform->SetPosition(meshTransform->GetPosition());
				lightTransform->SetRotation(CU::Vector3f(myLightRotation.x, myLightRotation.y, 0));
				lightTransform->AddPosition(CU::Vector3f(0, 0, -1000 * meshTransform->GetScale().x) * lightTransform->GetMatrix());
				MainSingleton::Get().GetSceneManager().GetScene().ShadowCamera->GetComponent<LightComponent>()->SetLightDirection(lightTransform->GetMatrix().GetRow(3).ToVector3());
			}
			if (ImGui::SliderFloat("Horizontal Rotation", &myLightRotation.y, 0.f, 360.f))
			{
				auto lightTransform = MainSingleton::Get().GetSceneManager().GetScene().ShadowCamera->GetTransform();
				auto meshTransform = MainSingleton::Get().GetSceneManager().GetScene().Objects[static_cast<unsigned>(GameObjects::Model)]->GetTransform();

				lightTransform->SetPosition(meshTransform->GetPosition());
				lightTransform->SetRotation(CU::Vector3f(myLightRotation.x, myLightRotation.y, 0));
				lightTransform->AddPosition(CU::Vector3f(0, 0, -1000 * meshTransform->GetScale().x) * lightTransform->GetMatrix());
				MainSingleton::Get().GetSceneManager().GetScene().ShadowCamera->GetComponent<LightComponent>()->SetLightDirection(lightTransform->GetMatrix().GetRow(3).ToVector3());
			}
		}

		if (ImGui::CollapsingHeader("Mesh Generation"))
		{
			auto mesh = MainSingleton::Get().GetSceneManager().GetScene().Objects[static_cast<unsigned>(GameObjects::Model)]->GetComponent<MeshComponent>();
			if (ImGui::Button("Create Cube"))
			{
				myAnimationNames.clear();
				myLoadAnimations = false;
				mesh->SetMesh(myCubeMesh);
				SnapToFloor();
				myActiveMesh = true;
				myActiveAnimation = false;
				myActiveName = "Cube";
			}
			ImGui::SameLine();
			if (ImGui::Button("Create Plane"))
			{
				myAnimationNames.clear();
				myLoadAnimations = false;
				mesh->SetMesh(myVerticalPlaneMesh);
				SnapToFloor();
				myActiveMesh = true;
				myActiveAnimation = false;
				myActiveName = "Plane";
			}
			ImGui::SameLine();
			if (ImGui::Button("Create Sphere"))
			{
				myAnimationNames.clear();
				myLoadAnimations = false;
				mesh->SetMesh(mySphere);
				SnapToFloor();
				myActiveName = "Sphere";
				myActiveMesh = true;
				myActiveAnimation = false;
			}
		}

		if (myActiveAnimation)
		{
			if (ImGui::CollapsingHeader("Animations"))
			{
				auto animator = MainSingleton::Get().GetSceneManager().GetScene().Objects[static_cast<unsigned>(GameObjects::Model)]->GetComponent<Animator>();
				for (size_t i = 0; i < myAnimationNames.size(); i++)
				{
					if (ImGui::Button(myAnimationNames[i].c_str()))
					{
						animator->PlayAnimation(myAnimationNames[i]);
					}
					ImGui::SameLine();
					std::string label = "Delete";
					if (ImGui::Button(label.c_str()))
					{
						animator->RemoveAnimation(myAnimationNames[i]);
						myAnimationNames.erase(myAnimationNames.begin() + i);
						if (myAnimationNames.size() == 0)
						{
							myActiveAnimation = false;
						}
					}
				}
			}
		}

		if(myActiveTexture)
		{
			if (ImGui::CollapsingHeader("Textures"))
			{
				if (!myActiveMesh)
				{
					ImVec4 yellowColor = { 1, 1, 0, 1 };
					ImGui::TextColored(yellowColor, "Drag in a mesh to view the texture");
				}
				auto defaultMaterial = AssetManager::Get().GetAsset<MaterialAsset>("M_Default")->myMaterial;
				auto meshMaterials = MainSingleton::Get().GetSceneManager().GetScene().Objects[static_cast<unsigned>(GameObjects::Model)]->GetComponent<MeshComponent>()->GetMaterials();
				auto albedoTexture = meshMaterials[0]->GetAlbedoTexture()->GetSRV().Get();
				auto materialTexture = meshMaterials[0]->GetMaterialTexture()->GetSRV().Get();
				auto normalTexture = meshMaterials[0]->GetNormalTexture()->GetSRV().Get();

				ImVec2 imageButtonSize = { 130, 130 };

				if (ImGui::ImageButton("Albedo", albedoTexture, imageButtonSize))
				{
					for (auto& material : meshMaterials)
					{
						material->SetAlbedoTexture(defaultMaterial->GetAlbedoTexture());
					}
					myAlbedoTexturePath = "Default";
				}
				ImGui::SameLine();
				if (ImGui::ImageButton("Material", materialTexture, imageButtonSize))
				{
					for (auto& material : meshMaterials)
					{
						material->SetMaterialTexture(defaultMaterial->GetMaterialTexture());
					}
					myMaterialTexturePath = "Default";
				}
				ImGui::SameLine();
				if (ImGui::ImageButton("Normal", (void*)normalTexture, imageButtonSize))
				{
					for (auto& material : meshMaterials)
					{
						material->SetNormalTexture(defaultMaterial->GetNormalTexture());
					}
					myNormalTexturePath = "Default";
				}

				ImGui::Text("Albedo Texture");
				ImGui::Text(myAlbedoTexturePath.c_str());
				ImGui::Text("Material Texture");
				ImGui::Text(myMaterialTexturePath.c_str());
				ImGui::Text("Normal Texture");
				ImGui::Text(myNormalTexturePath.c_str());
			}
		}

		if (myActiveMesh)
		{
			if (ImGui::CollapsingHeader("Mesh"))
			{
				std::string nameText = "Active mesh: " + myActiveName;
				ImGui::Text(nameText.c_str());

				bool updateScale = false;

				ImGui::Text("Scale");
				if (ImGui::SliderFloat("X##Scale", &myScale.x, 0.f, 5.f)) { updateScale = true; }
				if (ImGui::SliderFloat("Y##Scale", &myScale.y, 0.f, 5.f)) { updateScale = true; }
				if (ImGui::SliderFloat("Z##Scale", &myScale.z, 0.f, 5.f)) { updateScale = true; }
				if (updateScale)
				{
					MainSingleton::Get().GetSceneManager().GetScene().Objects[static_cast<unsigned>(GameObjects::Model)]->GetTransform()->SetScale(myScale);
				}
				buttonSize = { 100, 50 };
				if (ImGui::Button("Reset##Scale", buttonSize))
				{
					myScale = { 1.f, 1.f, 1.f };
					MainSingleton::Get().GetSceneManager().GetScene().Objects[static_cast<unsigned>(GameObjects::Model)]->GetTransform()->SetScale(myScale);
				}
				ImGui::SameLine();
				if (ImGui::Button("Snap To Floor", buttonSize))
				{
					auto mesh = MainSingleton::Get().GetSceneManager().GetScene().Objects[static_cast<unsigned>(GameObjects::Model)]->GetComponent<MeshComponent>();
					if (mesh->GetMesh() != nullptr)
					{
						mesh->GetMesh()->GenerateBoundingBox();
						SnapToFloor();
					}
				}
			}

			if (ImGui::CollapsingHeader("Shaders"))
			{
			/*	buttonSize = { 140, 20.f };
				auto mesh = MainSingleton::Get().GetSceneManager().GetScene().Objects[static_cast<unsigned>(GameObjects::Model)]->GetComponent<MeshComponent>();
				if (ImGui::Button("Default", buttonSize))
				{
					mesh->SetDebugShader(DebugShaders::Default);
				}
				ImGui::SameLine();
				if (ImGui::Button("World Normal", buttonSize))
				{
					mesh->SetDebugShader(DebugShaders::WorldNormal);
				}
				if (ImGui::Button("Albedo", buttonSize))
				{
					mesh->SetDebugShader(DebugShaders::Albedo);
				}
				ImGui::SameLine();
				if (ImGui::Button("Normal Map", buttonSize))
				{
					mesh->SetDebugShader(DebugShaders::NormalMap);
				}
				ImGui::SameLine();
				if (ImGui::Button("Ambient Occlusion", buttonSize))
				{
					mesh->SetDebugShader(DebugShaders::AmbientOcclusion);
				}
				if (ImGui::Button("Roughness", buttonSize))
				{
					mesh->SetDebugShader(DebugShaders::Roughness);
				}
				ImGui::SameLine();
				if (ImGui::Button("Metalness", buttonSize))
				{
					mesh->SetDebugShader(DebugShaders::Metalness);
				}
				if (ImGui::Button("Vertex Normal", buttonSize))
				{
					mesh->SetDebugShader(DebugShaders::VertexNormal);
				}
				ImGui::SameLine();
				if (ImGui::Button("Vertex Tangent", buttonSize))
				{
					mesh->SetDebugShader(DebugShaders::VertexTangent);
				}
				ImGui::SameLine();
				if (ImGui::Button("Vertex BiNormal", buttonSize))
				{
					mesh->SetDebugShader(DebugShaders::VertexBiNormal);
				}
				if (ImGui::Button("Vertex Color", buttonSize))
				{
					mesh->SetDebugShader(DebugShaders::VertexColor);
				}*/
			}
		}
	}
	ImGui::End();
#pragma endregion
}

void ModelViewerGui::Render()
{
	GraphicsCommandList::Get().Enqueue<GfxCmd_RenderGui>();
}

void ModelViewerGui::LoadAsset(const std::filesystem::path& aPath)
{
	AssetManager* assetManager = &AssetManager::Get();
	assetManager->RegisterAsset(aPath);

	const std::string ext = aPath.extension().string();
	std::string pathStem = aPath.stem().string();
	auto filename = aPath.filename().u8string();
	if (ext.ends_with("fbx"))
	{
		if (filename.rfind(u8"SM_", 0) == 0)
		{
			myAnimationNames.clear();
			myLoadAnimations = false;
			std::filesystem::path assetName = std::filesystem::path(filename);
			assetName = assetName.stem();
			MainSingleton::Get().GetSceneManager().GetScene().Objects[static_cast<unsigned>(GameObjects::Model)]->GetComponent<MeshComponent>()->SetMesh(assetName);
			SnapToFloor();

			myLogMessages.emplace_back("Loaded Mesh: " + aPath.string());
			myActiveMesh = true;
			myActiveAnimation = false;
			myActiveName = assetName.string();
		}
		else if (filename.rfind(u8"SK_", 0) == 0)
		{
			myAnimationNames.clear();
			myLoadAnimations = true;
			std::filesystem::path assetName = std::filesystem::path(filename);
			assetName = assetName.stem();
			MainSingleton::Get().GetSceneManager().GetScene().Objects[static_cast<unsigned>(GameObjects::Model)]->GetComponent<MeshComponent>()->SetMesh(assetName);
			MainSingleton::Get().GetSceneManager().GetScene().Objects[static_cast<unsigned>(GameObjects::Model)]->GetComponent<Animator>()->Awake();
			SnapToFloor();

			myLogMessages.emplace_back("Loaded Mesh with skeleton: " + aPath.string());
			myActiveMesh = true;
			myActiveAnimation = true;
			myActiveName = assetName.string();
		}
		else if (filename.rfind(u8"A_", 0) == 0)
		{
			if (!myLoadAnimations)
			{
				myLogMessages.emplace_back("Error! Animations can only be applied to meshes with skeletons");
				return;
			}
			auto animator = MainSingleton::Get().GetSceneManager().GetScene().Objects[static_cast<unsigned>(GameObjects::Model)]->GetComponent<Animator>();
			if (animator != nullptr)
			{
				Animator::SingleAnimationData animationData;
				animationData.Name = pathStem;
				animationData.AnimationAssetName = pathStem;
				animator->AddSingleAnimation(animationData);
				animator->PlayAnimation(pathStem);

				for (auto& animation : myAnimationNames)
				{
					if (animation == pathStem)
					{
						return;
					}
				}
				myAnimationNames.push_back(pathStem);
			}
			myActiveAnimation = true;
			myLogMessages.emplace_back("Loaded Animation: " + aPath.string());
		}
		else
		{
			myLogMessages.emplace_back("Error! Asset does not follow name standard. FBX files should begin with SM, A or SK");
		}
	}
	else if (ext.ends_with("dds"))
	{
		auto materials = MainSingleton::Get().GetSceneManager().GetScene().Objects[static_cast<unsigned>(GameObjects::Model)]->GetComponent<MeshComponent>()->GetMaterials();
		if (pathStem.ends_with("_C"))
		{
			for (auto& material : materials)
			{
				material->SetAlbedoTexture(aPath);
				myAlbedoTexturePath = aPath.string();
			}
			myLogMessages.emplace_back("Loaded Albedo Texture: " + aPath.string());
			if (!IsPowerOfTwo(materials[0]->GetAlbedoTexture()))
			{
				myLogMessages.emplace_back("Texture is not in power of two!");
			}
			myActiveTexture = true;
		}
		else if (pathStem.ends_with("_N"))
		{
			for (auto& material : materials)
			{
				material->SetNormalTexture(aPath);
				myNormalTexturePath = aPath.string();
			}
			myLogMessages.emplace_back("Loaded Normal Texture: " + aPath.string());
			if (!IsPowerOfTwo(materials[0]->GetNormalTexture()))
			{
				myLogMessages.emplace_back("Warning! Texture is not in power of two!");
			}
			myActiveTexture = true;
		}
		else if (pathStem.ends_with("_M"))
		{
			for (auto& material :	materials)
			{
				material->SetMaterialTexture(aPath);
				myMaterialTexturePath = aPath.string();
			}
			myLogMessages.emplace_back("Loaded Material Texture: " + aPath.string());
			if (!IsPowerOfTwo(materials[0]->GetMaterialTexture()))
			{
				myLogMessages.emplace_back("Warning! Texture is not in power of two");
			}
			myActiveTexture = true;
		}
		else if (pathStem.ends_with("_FX"))
		{
			myLogMessages.emplace_back("Modelviewer does not support effects yet");
		}
		else
		{
			myLogMessages.emplace_back("Error! Asset does not follow name standard. DDS files should have _M, _N or _C as suffix");
		}
	}
	else
	{
		myLogMessages.emplace_back(ext + " files are not supported");
	}
}

void ModelViewerGui::ResetScene()
{
	//delete myModelViewer->myScene;
	//myModelViewer->myScene = new MVScene();
	//myModelViewer->myScene->Init();
	MainSingleton::Get().GetSceneManager().CreateModelViewerScene();

	myLogMessages.clear();
	UnloadAssets();
	myLogMessages.emplace_back("Reset Scene");
}

void ModelViewerGui::SnapToFloor()
{
	auto gameObject = MainSingleton::Get().GetSceneManager().GetScene().Objects[static_cast<unsigned>(GameObjects::Model)];
	if (gameObject->GetComponent<MeshComponent>()->GetMesh() == nullptr)
	{
		return;
	}
	float offset = gameObject->GetComponent<MeshComponent>()->GetMesh()->GetBoundingBox().GetMin().y;
	myPosition.y = -offset * gameObject->GetTransform()->GetScale().y;
	gameObject->GetTransform()->SetPosition(myPosition);
}

bool ModelViewerGui::IsPowerOfTwo(std::shared_ptr<Texture> aTexture)
{
	ID3D11Resource* resource;
	aTexture->GetSRV()->GetResource(&resource);
	ID3D11Texture2D* texture = static_cast<ID3D11Texture2D*>(resource);
	D3D11_TEXTURE2D_DESC textureDesc;
	texture->GetDesc(&textureDesc);

	if (textureDesc.Width % 2 == 0 && textureDesc.Width == textureDesc.Height)
	{
		return true;
	}
	return false;
}

void ModelViewerGui::UnloadAssets()
{
	myAlbedoTexturePath = "Default";
	myMaterialTexturePath = "Default";
	myNormalTexturePath = "Default";
	myAnimationNames.clear();

	MainSingleton::Get().GetSceneManager().GetScene().Objects[static_cast<unsigned>(GameObjects::Model)]->GetComponent<MeshComponent>()->Awake();

	myActiveTexture = false;
	myActiveAnimation = false;
	myActiveMesh = false;
	AssetManager::Get().Reset();
	myLogMessages.emplace_back("Unloaded all assets");
}
