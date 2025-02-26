#include "FeatherEngine.pch.h"
#include "SceneManager.h"

#include "GameObjectSystem/Components/Lights/LightComponent.h"
#include "GameObjectSystem/Components/Lights/LightManager.h"
#include "GraphicsEngine\LightBuffer.h"
//#include <Game/Particles/ParticleManager.h>
#include "GameObjectSystem/Components/MeshComponent.h"
#include "GameObjectSystem/Components/ControllerComponent.h"
#include "GraphicsEngine/Mesh.h"
#include "GameObjectSystem\Components\Animations\Animator.h"
#include <AssetManagement/AssetTypes/MeshAsset.h>
#include <GraphicsEngine/GfxCmd_RenderInterface.h>
#include <GraphicsEngine/GfxCmd_RenderSkybox.h>
#include "GraphicsEngine/GfxCmd_SetCustomTextures.h"
#include "GameObjectSystem\Components\AnimationTriggerComponent.h"

#include "GraphicsEngine/Mesh.h"
#include "GraphicsEngine/GraphicsCommands.h"
#include "GraphicsEngine/GraphicsEngine.h"
#include <Interface/InterfaceManager.h>
#include "GameObjectSystem\Components\ShadowCameraComponent.h"
#include "GameObjectSystem\Components\AudioSourceComponent.h"
#include "GameObjectSystem\Components\MusicPlayerComponent.h"

#include "MainSingleton.h"
#include "CollisionHandling/CollisionManager.h"
#include "GameObjectSystem/Components/CapsuleColliderComponent.h"
#include "GameObjectSystem/Components/LevelSwapComponent.h"
#include "GameObjectSystem/Components/MVCameraComponent.h"
#include "GameObjectSystem/Components/RigidBodyComponent.h"
#include "GameObjectSystem/Components/OBBColliderComponent.h"
#include "GameObjectSystem/Components/SphereColliderComponent.h"
#include "GameObjectSystem/Components/ShaderObjectComponent.h"
#include "GameObjectSystem/Components/SkyboxComponent.h"
#include "GraphicsEngine/GfxCmd_SetDebugBuffer.h"
#include "Graphics/GraphicsEngine/DebugBuffer.h"
#include "GameObjectSystem\PostMaster\GameObjectPostMaster.h"
#include <TextureAsset.h>

#include "AssetManagement/AssetTypes/AnimationAsset.h"
#include "Physcics/PhysicsEngine.h"

#include "AssetManagement/AssetTypes/ColliderAsset.h"
#include "GameObjectSystem/Components/ScriptComponent.h"
#include "GraphicsEngine/Shader.h"
#include <GameObjectSystem/Components/ParticleEmitterComponent.h>

//#include "GraphicsEngine/GfxCmd_TAPostProcess.h"

bool BackToFrontMeshSort(GameObjectSort aFirst, GameObjectSort aSecond)
{
	return (aFirst.DistanceToCamera > aSecond.DistanceToCamera);
}
bool FrontToBackMeshSort(GameObjectSort aFirst, GameObjectSort aSecond)
{
	return (aSecond.DistanceToCamera > aFirst.DistanceToCamera);
}

SceneManager::SceneManager()
{
	myAssetManagerRef = nullptr;
	myCameraFov = 90.f;
	myLatestID = 0;
	myIsLoadingScene = false;
	myIsDebugMode = false;
#ifndef _RETAIL
	myDebugBuffer = std::make_shared<DebugBuffer>();
	myDebugBuffer->Exposure = 1.f;
	myDebugBuffer->Tonemap = 0;
	myDebugBuffer->CubemapInfluence = 1;
#endif
	myLevels.emplace("L_Level1", L"Assets\\Exported Levels\\l_level1\\L_Level1_Level.json");
	myLevels.emplace("L_Level2", L"Assets\\Exported Levels\\l_level2\\L_Level2_Level.json");
	myLevels.emplace("L_Level3", L"Assets\\Exported Levels\\l_level3\\L_Level3_Level.json");
	myLevels.emplace("L_AssetGym", L"Assets\\Exported Levels\\l_assetgym\\L_AssetGym_Level.json");
	myLevels.emplace("L_PlayerGym", L"Assets\\Exported Levels\\l_playergym\\L_PlayerGym_Level.json");
}

SceneManager::~SceneManager()
{
	CollisionManager::Get()->CleanUp();
	ComponentSystem::Get()->CleanUp();
}

void SceneManager::Awake()
{
	GameObjectPostMaster::Get()->Register(eLetterRegister::LoadLights, this);
}

void SceneManager::Init()
{
	myAssetManagerRef = &AssetManager::Get();
	myGameObjectFactory = std::make_shared<GameObjectFactory>(ComponentSystem::Get());
	myGameObjectFactory->Init();
	//myMeshNavigatorRefrence = &MainSingleton::Get().GetMeshNavigator();
}

void SceneManager::LoadGlobalObjects()
{
	myGloballyLoadedObjects.LightManager = std::make_shared<LightManager>();
	myGloballyLoadedObjects.MusicPlayer = std::make_shared<GameObject>();
	myGloballyLoadedObjects.MusicPlayer->AddComponent(ComponentSystem::Get()->AddComponent<Transform, TRANSFORM>())->SetGameObject(myGloballyLoadedObjects.MusicPlayer);
	myGloballyLoadedObjects.MusicPlayer->AddComponent(ComponentSystem::Get()->AddComponent<MusicPlayerComponent, MUSIC_PLAYER_COMPONENT>());
	myGloballyLoadedObjects.MusicPlayer->GetComponent<MusicPlayerComponent>()->SetGameObject(myGloballyLoadedObjects.MusicPlayer);
	myGloballyLoadedObjects.MusicPlayer->GetComponent<MusicPlayerComponent>()->Init();

	std::shared_ptr<Mesh> skyboxMesh = std::make_shared<Mesh>();
	skyboxMesh->GenerateSphere(-95000, 10000);

	myGloballyLoadedObjects.Skybox = std::make_shared<GameObject>();
	myGloballyLoadedObjects.Skybox->SetName("Skybox");
	myGloballyLoadedObjects.Skybox->AddComponent(ComponentSystem::Get()->AddComponent<Transform, TRANSFORM>());
	myGloballyLoadedObjects.Skybox->GetComponent<Transform>()->SetGameObject(myGloballyLoadedObjects.Skybox);
	myGloballyLoadedObjects.Skybox->AddComponent(ComponentSystem::Get()->AddComponent<SkyboxComponent, SKYBOX_COMPONENT>());
	myGloballyLoadedObjects.Skybox->GetComponent<SkyboxComponent>()->SetGameObject(myGloballyLoadedObjects.Skybox);
	myGloballyLoadedObjects.Skybox->GetComponent<SkyboxComponent>()->SetMesh(skyboxMesh);
	myGloballyLoadedObjects.Skybox->SetEnabled(true);

	myGloballyLoadedObjects.Player = std::make_shared<GameObject>();
	myGloballyLoadedObjects.Player->SetName("Player");
	myGloballyLoadedObjects.Player->AddComponent(ComponentSystem::Get()->AddComponent<Transform, TRANSFORM>());
	myGloballyLoadedObjects.Player->GetComponent<Transform>()->SetGameObject(myGloballyLoadedObjects.Player);
	myGloballyLoadedObjects.Player->AddComponent(ComponentSystem::Get()->AddComponent<MeshComponent, MESH_COMPONENT>());
	myGloballyLoadedObjects.Player->GetComponent<MeshComponent>()->SetGameObject(myGloballyLoadedObjects.Player);
	myGloballyLoadedObjects.Player->GetComponent<MeshComponent>()->SetMesh(myAssetManagerRef->GetAsset<MeshAsset>("SK_PlayerCharacter")->myMesh);
	myGloballyLoadedObjects.Player->SetEnabled(true);
	myGloballyLoadedObjects.Player->AddComponent(ComponentSystem::Get()->AddComponent<Animator, ANIMATOR>());
	myGloballyLoadedObjects.Player->GetComponent<Animator>()->SetGameObject(myGloballyLoadedObjects.Player);
	myGloballyLoadedObjects.Player->GetComponent<Animator>()->Init();
	myGloballyLoadedObjects.Player->AddComponent(ComponentSystem::Get()->AddComponent<ScriptComponent, SCRIPT_COMPONENT>())->SetGameObject(myGloballyLoadedObjects.Player);
	myGloballyLoadedObjects.Player->GetComponent<ScriptComponent>()->AddScript(myAssetManagerRef->GetAsset<ScriptAsset>("SC_PlayerTest"));

	auto playerAnimator = myGloballyLoadedObjects.Player->GetComponent<Animator>();

	myGloballyLoadedObjects.ParticleEmiter = std::make_shared<GameObject>();
	myGloballyLoadedObjects.ParticleEmiter->AddComponent(ComponentSystem::Get()->AddComponent<Transform, TRANSFORM>());
	myGloballyLoadedObjects.ParticleEmiter->GetComponent<Transform>()->SetGameObject(myGloballyLoadedObjects.ParticleEmiter);
	myGloballyLoadedObjects.ParticleEmiter->AddComponent(ComponentSystem::Get()->AddComponent<ParticleEmitterComponent, PARTICLE_EMITTER_COMPONENT>());
	myGloballyLoadedObjects.ParticleEmiter->GetComponent<ParticleEmitterComponent>()->SetGameObject(myGloballyLoadedObjects.ParticleEmiter);
	myGloballyLoadedObjects.ParticleEmiter->GetTransform()->SetPosition({ 0, 750, 0 });
	myGloballyLoadedObjects.ParticleEmiter->GetComponent<ParticleEmitterComponent>()->LoadParticles("PS_SnowTest");



	std::vector<std::shared_ptr<Material>> playerMats;
	std::shared_ptr<Material> playerMaterial = std::make_shared<Material>();
	playerMaterial->SetAlbedoTexture(myAssetManagerRef->GetAsset<TextureAsset>("T_PlayerCharacter_C")->myTexture);
	playerMaterial->SetNormalTexture(myAssetManagerRef->GetAsset<TextureAsset>("T_PlayerCharacter_N")->myTexture);
	playerMaterial->SetMaterialTexture(myAssetManagerRef->GetAsset<TextureAsset>("T_PlayerCharacter_M")->myTexture);
	//std::vector < std::filesystem::path> playerMatPaths;
	//playerMatPaths.push_back("M_Player");
	playerMats.push_back(playerMaterial);


	Animator::SingleAnimationData animation = {}; // Add all player animations
	//Idle
	animation.Name = "Player Idle";
	animation.AnimationAssetName = "A_PlayerIdle";
	playerAnimator->AddSingleAnimation(animation, true);
	playerAnimator->PlayAnimation("Player Idle");
	//Idle Variant
	animation.Name = "Player Idle Variation";
	animation.AnimationAssetName = "A_PlayerIdleVariation";
	playerAnimator->AddSingleAnimation(animation, true);
	//Walk
	animation.Name = "Player Walk";
	animation.AnimationAssetName = "A_PlayerWalk";
	playerAnimator->AddSingleAnimation(animation, true);
	//Walk Up slope
	animation.Name = "Player Walk Up Slope";
	animation.AnimationAssetName = "A_PlayerSlopeUp";
	playerAnimator->AddSingleAnimation(animation, true);
	//Walking Against Wind
	animation.Name = "Player Walk Against Wind";
	animation.AnimationAssetName = "A_PlayerWalkAgainstWind";
	playerAnimator->AddSingleAnimation(animation, true);
	//Run
	animation.Name = "Player Run";
	animation.AnimationAssetName = "A_PlayerRun";
	playerAnimator->AddSingleAnimation(animation, true);
	//Walking up slope
	animation.Name = "Player Climb Slope";
	animation.AnimationAssetName = "A_PlayerSlopeUp";
	playerAnimator->AddSingleAnimation(animation, true);
	//Slide
	animation.Name = "Player Slide";
	animation.AnimationAssetName = "A_PlayerSlide";
	playerAnimator->AddSingleAnimation(animation, true);
	//Shout
	animation.Name = "Player Shout";
	animation.AnimationAssetName = "A_PlayerShout";
	playerAnimator->AddSingleAnimation(animation, false);
	//Big Shout
	animation.Name = "Player Shout Big";
	animation.AnimationAssetName = "A_PlayerShoutBig";
	playerAnimator->AddSingleAnimation(animation, false);
	//Jump
	animation.Name = "Player Jump";
	animation.AnimationAssetName = "A_PlayerJump";
	playerAnimator->AddSingleAnimation(animation, false);
	//SuperJump
	animation.Name = "Player Super Jump";
	animation.AnimationAssetName = "A_PlayerSuperJump";
	playerAnimator->AddSingleAnimation(animation, false);
	//Fall
	animation.Name = "Player Fall";
	animation.AnimationAssetName = "A_PlayerFall";
	playerAnimator->AddSingleAnimation(animation, true);
	//Float
	animation.Name = "Player Float";
	animation.AnimationAssetName = "A_PlayerFloat";
	playerAnimator->AddSingleAnimation(animation, true);
	//Jump Land
	animation.Name = "Player Jump Land";
	animation.AnimationAssetName = "A_PlayerFallLand";
	playerAnimator->AddSingleAnimation(animation, false);
	//Float Land
	animation.Name = "Player Float Land";
	animation.AnimationAssetName = "A_PlayerFloatLand";
	playerAnimator->AddSingleAnimation(animation, true);
	//GetTape
	animation.Name = "Player Get Tape";
	animation.AnimationAssetName = "A_PlayerGetTape";
	playerAnimator->AddSingleAnimation(animation, false);
	//coming to a halt
	animation.Name = "Player Walk Stop";
	animation.AnimationAssetName = "A_PlayerWalkStop";
	playerAnimator->AddSingleAnimation(animation, false);

	myGloballyLoadedObjects.Player->GetComponent<MeshComponent>()->SetMaterials(playerMats);
	{
		myGloballyLoadedObjects.Camera = std::make_shared<GameObject>();
		myGloballyLoadedObjects.Camera->AddComponent(ComponentSystem::Get()->AddComponent<MVCameraComponent, MV_CAMERA_COMPONENT>());
		myGloballyLoadedObjects.Camera->GetComponent<MVCameraComponent>()->SetGameObject(myGloballyLoadedObjects.Camera);
		myGloballyLoadedObjects.Camera->AddComponent(ComponentSystem::Get()->AddComponent<Transform, TRANSFORM>());
		myGloballyLoadedObjects.Camera->GetComponent<Transform>()->SetGameObject(myGloballyLoadedObjects.Camera);
		myGloballyLoadedObjects.Camera->GetComponent<MVCameraComponent>()->CreateProjection(myCameraFov, 16.f / 9.f, 1.0f, 100000.f);
		myGloballyLoadedObjects.Camera->GetTransform()->SetPosition({ 0, 100, -1000 });
		//myGloballyLoadedObjects.Camera->GetTransform()->SetPosition(myGloballyLoadedObjects.Player->GetComponent<MeshComponent>()->GetJoint(""));
		myGloballyLoadedObjects.Camera->AddComponent(ComponentSystem::Get()->AddComponent <ControllerComponent, CONTROLLER_COMPONENT>());
		myGloballyLoadedObjects.Camera->GetComponent<ControllerComponent>()->SetGameObject(myGloballyLoadedObjects.Camera);
		myGloballyLoadedObjects.Camera->GetComponent<ControllerComponent>()->Awake();
		myGloballyLoadedObjects.Camera->GetComponent<ControllerComponent>()->AddAction(ActionEventID::MoveCamForward);
		myGloballyLoadedObjects.Camera->GetComponent<ControllerComponent>()->AddAction(ActionEventID::MoveCamBackwards);
		myGloballyLoadedObjects.Camera->GetComponent<ControllerComponent>()->AddAction(ActionEventID::MoveCamLeft);
		myGloballyLoadedObjects.Camera->GetComponent<ControllerComponent>()->AddAction(ActionEventID::MoveCamRight);
		myGloballyLoadedObjects.Camera->GetComponent<ControllerComponent>()->AddAction(ActionEventID::FlyDown);
		myGloballyLoadedObjects.Camera->GetComponent<ControllerComponent>()->AddAction(ActionEventID::FlyUp);
		myGloballyLoadedObjects.Camera->GetComponent<ControllerComponent>()->AddAction(ActionEventID::Rotation);
		myGloballyLoadedObjects.Camera->SetShouldDebugRender(true);

		myGloballyLoadedObjects.Camera->AddComponent(ComponentSystem::Get()->AddComponent<RigidBodyComponent, RIGID_BODY_COMPONENT>())->SetGameObject(myGloballyLoadedObjects.Camera);
		myGloballyLoadedObjects.Camera->GetComponent<RigidBodyComponent>()->Initialize(Squish::Dynamic, Squish::Sphere, myGloballyLoadedObjects.Camera->GetTransform(), { 1, 1, 0 }, 3.f);
		myGloballyLoadedObjects.Camera->GetComponent<RigidBodyComponent>()->SetKinematic(true);
		CollisionManager::Get()->AddRigidBody(myGloballyLoadedObjects.Camera->GetComponent<RigidBodyComponent>());
		//Camera should be parented to anim cam, and change prog cam
		std::string jointName = "animCam";
		jointName = "progCam";
		auto meshComp = myGloballyLoadedObjects.Player->GetComponent<MeshComponent>();
		int jointIndex = meshComp->GetJoint(jointName);
		auto animator = myGloballyLoadedObjects.Player->GetComponent<Animator>();
		if (!meshComp->GetMesh()->GetSkeleton().Joints.empty())
		{
			myGloballyLoadedObjects.Camera->GetComponent<Transform>()->AddJointParent(&animator->GetAnimationBuffer()->JointTransforms[jointIndex], meshComp->GetMesh()->GetSkeleton().Joints[jointIndex].BindPoseInverse);
			myGloballyLoadedObjects.Camera->GetComponent<Transform>()->AddObjectParent(myGloballyLoadedObjects.Player->GetTransform());

			jointIndex = meshComp->GetJoint(jointName);
			myGloballyLoadedObjects.Camera->GetComponent<MVCameraComponent>()->SetCamJoint(&animator->GetAnimationBuffer()->JointTransforms[jointIndex]);
		}

		myGloballyLoadedObjects.Camera->GetComponent<Transform>()->SetPosition({ 0, 100, -1000 });
	}
	myGloballyLoadedObjects.Skybox->GetComponent<Transform>()->AddObjectParent(myGloballyLoadedObjects.Player->GetComponent<Transform>());
	LoadPlayerSettings();
	LoadShaderObjects();
}

void SceneManager::LoadScene(const std::filesystem::path& aPath)
{
	myIsLoadingScene.store(true, std::memory_order_release);
	mySceneToLoad = aPath;
	myFirst = false;
	Letter loadLevelLetter;
	loadLevelLetter.myType = eLetterRegister::LoadingLevel;
	loadLevelLetter.myData = (void*)mySceneToLoad.c_str();
	//GameObjectPostMaster::Get()->Send(loadLevelLetter);
	myHasLoadedEnemies = false;
	myHasLoadedScene = false;
	myCurrentLevelPath = aPath;
	loadThread = std::thread([this]() {LoadInternal(); });
	loadThread.detach();
}

void SceneManager::LoadSceneNextFrame(const std::filesystem::path& aPath)
{
	if (aPath.string().find("Assets") == std::string::npos)
	{
		if (myLevels.contains(aPath.string()))
		{
			mySceneToLoad = myLevels.at(aPath.string());
			myCurrentLevelPath = aPath;
			return;
		}
		else
		{
			mySceneToLoad = myLevels.at("L_PlayerGym");
		}
	}
	mySceneToLoad = aPath;

	std::wstring wideScenePath = mySceneToLoad.wstring();
	if (wideScenePath == std::wstring(L"Assets\\Exported Levels\\l_level1\\L_Level1_Level.json"))
	{
		Letter letter;
		letter.myType = eLetterRegister::SetLoadingScreenMask;
		unsigned index = 1;
		letter.myData = static_cast<void*>(&index);
		GameObjectPostMaster::Get()->Send(letter);
	}
	else if (wideScenePath == std::wstring(L"Assets\\Exported Levels\\l_level2\\L_Level2_Level.json"))
	{
		Letter letter;
		letter.myType = eLetterRegister::SetLoadingScreenMask;
		unsigned index = 2;
		letter.myData = static_cast<void*>(&index);
		GameObjectPostMaster::Get()->Send(letter);
	}
	else if (wideScenePath == std::wstring(L"Assets\\Exported Levels\\l_level3\\L_Level3_Level.json"))
	{
		Letter letter;
		letter.myType = eLetterRegister::SetLoadingScreenMask;
		unsigned index = 3;
		letter.myData = static_cast<void*>(&index);
		GameObjectPostMaster::Get()->Send(letter);
	}
	else if (wideScenePath == std::wstring(L"Assets\\Exported Levels\\l_level4\\L_Level4_Level.json"))
	{
		Letter letter;
		letter.myType = eLetterRegister::SetLoadingScreenMask;
		unsigned index = 4;
		letter.myData = static_cast<void*>(&index);
		GameObjectPostMaster::Get()->Send(letter);
	}

	myCurrentLevelPath = aPath;
}

void SceneManager::DestroyScene()
{
	for (int objectIndex = 0; objectIndex < myScene.DeferredObjects.size(); objectIndex++)
	{
		myScene.DeferredObjects[objectIndex]->Destroy();
	}
	for (int objectIndex = 0; objectIndex < myScene.ForwardObjects.size(); objectIndex++)
	{
		myScene.ForwardObjects[objectIndex]->Destroy();
	}
	myScene.IsGameScene = false;
	myScene.DeferredObjects.clear();
	myDeferredSortedObjects.clear();
	myScene.ForwardObjects.clear();
	myForwardSortedObjects.clear();
	for (int objectIndex = 0; objectIndex < myScene.TAObjects.size(); objectIndex++)
	{
		myScene.TAObjects[objectIndex]->Destroy();
	}
	myScene.TAObjects.clear();
	myTASortedObjects.clear();
	for (int objectIndex = 0; objectIndex < myScene.NavMeshSwitchers.size(); objectIndex++)
	{
		myScene.NavMeshSwitchers[objectIndex]->Destroy();
	}
	myScene.NavMeshSwitchers.clear();
	myScene.myEnemies.clear();
	myGloballyLoadedObjects.LightManager->Reset();

	//these two must be last or the whole thing will crash

	CollisionManager::Get()->CleanUp();
	ComponentSystem::Get()->CleanUp();
	myHasLoadedScene = false;
}

void SceneManager::UpdateScene(float aDeltaTime)
{
	if (myFirst)
	{
		myFirst = false;
		Letter levelLoadedLetter;
		levelLoadedLetter.myType = eLetterRegister::LoadLights;
		levelLoadedLetter.myData = nullptr;
		GameObjectPostMaster::Get()->Send(levelLoadedLetter);
		MainSingleton::Get().GetMiscBuffer()->CurrentLevel = myCurrentLevel;

		Letter loadLevelLetter;
		loadLevelLetter.myType = eLetterRegister::LoadingDone;
		loadLevelLetter.myData = (void*)&myCurrentLevel;
		GameObjectPostMaster::Get()->Send(loadLevelLetter);
		myHasLoadedEnemies = true;

		return;
	}
	/*if(!mySceneToLoad.empty() && myHasLoadedScene)
	{
		LoadScene(mySceneToLoad);
		return;
	}*/
	if (myIsLoadingScene.load(std::memory_order_acquire))
	{
		return;
	}
	if (!mySceneToLoad.empty())
	{
		DestroyScene();
		LoadScene(mySceneToLoad);
		return;
	}
	SortObjects();

	if (CU::Input::GetKeyDown(CU::Keys::N0) && CU::Input::GetKeyHeld(CU::Keys::CONTROL))
	{
		Letter abilityOne;
		abilityOne.myType = eLetterRegister::AbilityOne;
		GameObjectPostMaster::Get()->Send(abilityOne);
	}

	if (myGloballyLoadedObjects.Player != nullptr)
	{
		myGloballyLoadedObjects.LightManager->Update(myGloballyLoadedObjects.Player->GetTransform()->GetPosition());
	}
	if (myGloballyLoadedObjects.Camera != nullptr)
	{
		MainSingleton::Get().GetAudioEngine().Update3D(myGloballyLoadedObjects.Camera->GetTransform()->GetPosition(),
			myGloballyLoadedObjects.Camera->GetTransform()->GetForwardVector(),
			myGloballyLoadedObjects.Camera->GetTransform()->GetUpVector());
	}
	myGloballyLoadedObjects.MusicPlayer->Update(aDeltaTime);
	myGloballyLoadedObjects.ParticleEmiter->Update(aDeltaTime);
	if (CU::Input::GetKeyHeld(CU::Keys::U))
	{
		myGloballyLoadedObjects.MusicPlayer->GetTransform()->AddRotation(CommonUtilities::Vector3f(0, -1.f, 0));
	}
	if (CU::Input::GetKeyHeld(CU::Keys::I))
	{
		myGloballyLoadedObjects.MusicPlayer->GetTransform()->AddRotation(CommonUtilities::Vector3f(0, 1.f, 0));
	}
	ViewFrustumCulling();

	if (CU::Input::GetKeyHeld('U'))
	{
		myGloballyLoadedObjects.Camera->GetComponent<ControllerComponent>()->AddSpeed(min(myGloballyLoadedObjects.Camera->GetComponent<ControllerComponent>()->GetSpeed() * 0.02f, 50));
	}

	if (CU::Input::GetKeyHeld('I'))
	{
		myGloballyLoadedObjects.Camera->GetComponent<ControllerComponent>()->AddSpeed(max(myGloballyLoadedObjects.Camera->GetComponent<ControllerComponent>()->GetSpeed() * -0.02f, -50));
	}

	if (CU::Input::GetKeyDown(CU::Keys::RETURN))
	{
		GraphicsEngine::Get().CompileAllCustomShaders();
	}

	auto mousePoint = CU::Input::GetMouseAbsolutePosition();
	CU::Vector2f mousepos = { static_cast<float>(mousePoint.x), static_cast<float>(mousePoint.y) };

	if (CU::Input::GetKeyDown('X'))
	{
		myGloballyLoadedObjects.Camera->GetComponent<Transform>()->SetPosition(myGloballyLoadedObjects.Player->GetComponent<Transform>()->GetPosition());
	}
	Squish::Update(1 / 120.f);

#ifndef _RETAIL
	if (CU::Input::GetKeyHeld(CU::Keys::N0))
	{
		myDebugBuffer->Exposure += aDeltaTime;
	}
	if (CU::Input::GetKeyHeld(CU::Keys::N9))
	{
		myDebugBuffer->Exposure -= aDeltaTime;
	}
	if (CU::Input::GetKeyDown(CU::Keys::T))
	{
		myDebugBuffer->Tonemap = (1 + myDebugBuffer->Tonemap) % 4;
	}

#endif
}

bool SceneManager::GetIsLoading()
{
	return myIsLoadingScene.load();
}

void SceneManager::RenderScene()
{
	if (!myIsLoadingScene && myHasLoadedScene)
	{
		auto& renderer = MainSingleton::Get().GetRenderer();
#ifndef _RETAIL
		//DebugBufferData debugBufferData;
		//debugBufferData.type = GraphicsCommandType::SetDebugBuffer;
		//debugBufferData.isEmpty = false;
		//debugBufferData.debugBuffer = *myDebugBuffer;
		renderer.Enqueue<GfxCmd_SetDebugBuffer>(myDebugBuffer);
#endif
		//EventData eData;
		//eData.eventName = "DirlightShadowMapping";
		//eData.type = GraphicsCommandType::BeginEvent;
		renderer.Enqueue<GfxCmd_BeginEvent>("DirlightShadowMapping");

		//ShaderData sData;
		//sData.shaderName = "Default_VS";
		//sData.shaderType = ShaderType::VertexShader;
		//sData.type = GraphicsCommandType::SetShaders;
		renderer.Enqueue<GfxCmd_SetVertexShader>("Default_VS");

		//sData.shaderType = ShaderType::PixelShader;
		//sData.shaderName = "Empty";
		renderer.Enqueue<GfxCmd_SetPixelShader>("Empty");

		myGloballyLoadedObjects.LightManager->RenderDirlightShadows();

		//PSOData pData;
		//pData.PSO = GraphicsEngine::Get().GetShadowPSO();
		//pData.type = GraphicsCommandType::ChangePipelineStage;
		renderer.Enqueue<GfxCmd_ChangePipelineState>(GraphicsEngine::Get().GetShadowPSO());

		int index = 0;
		for (auto& object : myDeferredSortedObjects)
		{
			index++;
			if (object.GameObject != nullptr)
			{
				object.GameObject->Render();
			}
		}
		for (auto& object : myForwardSortedObjects)
		{
			if (object.GameObject)
			{
				object.GameObject->Render();
			}
		}

		//RenderData data;
		//data.type = GraphicsCommandType::EndEvent;
		renderer.Enqueue<GfxCmd_EndEvent>();

		//eData.eventName = "Deferred";
		renderer.Enqueue<GfxCmd_BeginEvent>("Deferred");

		//pData.PSO = GraphicsEngine::Get().GetDeferredPSO();
		renderer.Enqueue<GfxCmd_ChangePipelineState>(GraphicsEngine::Get().GetDeferredPSO());

		//sData.shaderType = ShaderType::PixelShader;
		//sData.shaderName = "GBuffer_PS";
		renderer.Enqueue<GfxCmd_SetPixelShader>("GBuffer_PS");

		myGloballyLoadedObjects.LightManager->Render();
		myGloballyLoadedObjects.Camera->Render();

		for (auto& object : myDeferredSortedObjects)
		{
			if (object.GameObject != nullptr)
			{
				object.GameObject->Render();
			}
		}

		//data.type = GraphicsCommandType::ClearBackBuffer;
		renderer.Enqueue<GfxCmd_ClearBackBuffer>();

		//MarkerData mData;
		//mData.type = GraphicsCommandType::SetMarker;
		//mData.markerName = "Light";
		renderer.Enqueue<GfxCmd_SetMarker>("Light");


		//sData.shaderType = ShaderType::VertexShader;
		//sData.shaderName = "Quad_VS";
		renderer.Enqueue<GfxCmd_SetVertexShader>("Quad_VS");

		//sData.shaderType = ShaderType::PixelShader;
		//sData.shaderName = "Directional_Light_PS";
		renderer.Enqueue<GfxCmd_SetPixelShader>("Directional_Light_PS");

		//pData.PSO = GraphicsEngine::Get().GetLightPSO();
		renderer.Enqueue<GfxCmd_ChangePipelineState>(GraphicsEngine::Get().GetLightPSO());

		//data.type = GraphicsCommandType::DrawQuad;
		renderer.Enqueue<GfxCmd_DrawQuad>();

		//sData.shaderType = ShaderType::PixelShader;
		//sData.shaderName = "PointLight_PS";
		renderer.Enqueue<GfxCmd_SetPixelShader>("PointLight_PS");

		//renderer.Enqueue(data);
		//data.type = GraphicsCommandType::EndEvent;
		renderer.Enqueue<GfxCmd_EndEvent>();

		//eData.eventName = "Forward";
		renderer.Enqueue<GfxCmd_BeginEvent>("Forward");

		//sData.shaderType = ShaderType::PixelShader;
		//sData.shaderName = "Default_PS";
		renderer.Enqueue<GfxCmd_SetPixelShader>("Default_PS");
		//sData.shaderType = ShaderType::VertexShader;
		//sData.shaderName = "Default_VS";
		renderer.Enqueue<GfxCmd_SetVertexShader>("Default_VS");
		//pData.PSO = GraphicsEngine::Get().GetForwardPSO();
		renderer.Enqueue<GfxCmd_ChangePipelineState>(GraphicsEngine::Get().GetForwardPSO());

		for (auto& object : myForwardSortedObjects)
		{
			if (object.GameObject)
			{
				object.GameObject->Render();
			}
		}

		

		myGloballyLoadedObjects.Skybox->Render();

		renderer.Enqueue<GfxCmd_ChangePipelineState>(GraphicsEngine::Get().GetParticlePSO());
		myGloballyLoadedObjects.ParticleEmiter->Render();
		renderer.Enqueue<GfxCmd_SetVertexShader>("Default_VS");

		//data.type = GraphicsCommandType::EndEvent;
		renderer.Enqueue<GfxCmd_EndEvent>();
		//eData.eventName = "Debug Rendering";
		renderer.Enqueue<GfxCmd_BeginEvent>("Debug Rendering");
		//DebugRendering();

		//data.type = GraphicsCommandType::EndEvent;
		renderer.Enqueue<GfxCmd_EndEvent>();

		//eData.eventName = "TA";
		renderer.Enqueue<GfxCmd_BeginEvent>("TA");

		//CustomTextureData ctData;
		//ctData.pipelineStage = PIPELINE_STAGE_PIXEL_SHADER;
		//renderer.Enqueue<GfxCmd_SetCustomTextures>(PIPELINE_STAGE_PIXEL_SHADER);

		//data.type = GraphicsCommandType::SetGBufferResource;
		renderer.Enqueue<GfxCmd_SetGBufferResource>();

		//pData.PSO = GraphicsEngine::Get().GetCustomPSO();
		renderer.Enqueue<GfxCmd_ChangePipelineState>(GraphicsEngine::Get().GetCustomPSO());

		//renderer.Enqueue<GfxCmd_SetVertexShader>("Default_VS");
		//renderer.Enqueue<GfxCmd_SetPixelShader>("Default_PS");
		for (auto& object : myTASortedObjects)
		{
			if (object.GameObject)
			{
				//sData.shaderType = ShaderType::VertexShader;
				//sData.shaderName = object.GameObject->GetComponent<MeshComponent>()->GetVertexShader();
				renderer.Enqueue<GfxCmd_SetVertexShader>(object.GameObject->GetComponent<MeshComponent>()->GetVertexShader());

				//sData.shaderType = ShaderType::PixelShader;
				//std::string name = object.GameObject->GetComponent<MeshComponent>()->GetPixelShader().data();
				renderer.Enqueue<GfxCmd_SetPixelShader>(object.GameObject->GetComponent<MeshComponent>()->GetPixelShader());

				object.GameObject->Render();
			}
		}
		for (auto& shaderObject : myGloballyLoadedObjects.ShaderObjects)
		{
			//sData.shaderType = ShaderType::PixelShader;
			//sData.shaderName = shaderObject->GetComponent<MeshComponent>()->GetPixelShader();
			renderer.Enqueue<GfxCmd_SetVertexShader>(shaderObject->GetComponent<MeshComponent>()->GetVertexShader());

			//sData.shaderType = ShaderType::VertexShader;
			//sData.shaderName = shaderObject->GetComponent<MeshComponent>()->GetVertexShader();
			std::string name = shaderObject->GetComponent<MeshComponent>()->GetPixelShader().data();
			renderer.Enqueue<GfxCmd_SetPixelShader>(shaderObject->GetComponent<MeshComponent>()->GetPixelShader());
			shaderObject->Render();
		}

		//data.type = GraphicsCommandType::EndEvent;
		renderer.Enqueue<GfxCmd_EndEvent>();
		auto camera = myGloballyLoadedObjects.Camera->GetComponent<MVCameraComponent>();
		if (camera != nullptr)
		{
			if (!camera->IsDebugRendering())
			{
				//data.type = GraphicsCommandType::TAPostProcess;
				renderer.Enqueue<GfxCmd_TAPostProcess>();
			}
		}
		//data.type = GraphicsCommandType::Tonemap;
		renderer.Enqueue<GfxCmd_Tonemap>();

		//data.type = GraphicsCommandType::Bloom;
		renderer.Enqueue<GfxCmd_Bloom>();

		//data.type = GraphicsCommandType::EndEvent;
		renderer.Enqueue<GfxCmd_EndEvent>();

		InterfaceManager::Get()->Draw();

		renderer.SwitchUpdateIntermediate();
	}
}

Scene& SceneManager::GetScene()
{
	return myScene;
}

int SceneManager::GetCurrentLevel()
{
	return myCurrentLevel;
}

void SceneManager::AddObject(std::shared_ptr<GameObject> aGameObject)
{
	myScene.DeferredObjects.push_back(aGameObject);
	InitObjectSort();
}

void SceneManager::AddTAObject(std::shared_ptr<GameObject> aGameObject)
{
	myScene.TAObjects.push_back(aGameObject);
	InitObjectSort();
}

void SceneManager::CreateModelViewerScene()
{
	myScene = {};
	myScene.IsGameScene = true;
	{
		std::shared_ptr<GameObject>	floor = std::make_shared<GameObject>();
		myGameObjectFactory->Fabricate(floor, "StaticMesh");
		auto meshComp = floor->GetComponent<MeshComponent>();
		std::filesystem::path path = "M_Dirt";
		meshComp->SetMaterials(path);
		auto mesh = std::make_shared<Mesh>();
		mesh->GenerateHorizontalPlane(1000);
		meshComp->SetMesh(mesh);
		myScene.DeferredObjects.push_back(floor);
	}
	{
		std::shared_ptr<GameObject>	mainMesh = std::make_shared<GameObject>();
		myGameObjectFactory->Fabricate(mainMesh, "StaticMesh");
		auto meshComp = mainMesh->GetComponent<MeshComponent>();
		auto mesh = std::make_shared<Mesh>();
		mesh->GenerateHorizontalPlane(1000);
		meshComp->SetMesh(mesh);
		myScene.DeferredObjects.push_back(mainMesh);
	}

	myGloballyLoadedObjects.LightManager->InitLights();

	std::shared_ptr<GameObject>	dirLight = std::make_shared<GameObject>();
	myGameObjectFactory->Fabricate(dirLight, "Light");
	dirLight->AddComponent(ComponentSystem::Get()->AddComponent<ShadowCameraComponent, SHADOW_CAMERA_COMPONENT>());
	dirLight->GetComponent<ShadowCameraComponent>()->SetGameObject(dirLight);
	dirLight->GetComponent<LightComponent>()->Initialize({ 1, 1, 1 }, 1, { 0, -1, 0 }, { 0, 2000, 0 }, 0, LightType::DirectionalLight, 1000);
	dirLight->GetComponent<LightComponent>()->InitDirLightMatrices(myScene.DeferredObjects[0]->GetTransform()->GetMatrixScale(), 5000.0f);
	dirLight->Awake();
	dirLight->GetComponent<ShadowCameraComponent>()->Init(
		dirLight->GetComponent<LightComponent>()->GetLightData()->LightViewInv,
		dirLight->GetComponent<LightComponent>()->GetLightData()->LightProj,
		dirLight->GetComponent<LightComponent>()->GetLightData()->LightPos);
	dirLight->GetComponent<ShadowCameraComponent>()->CreateOrthographic({ 2000, 2000 }, 0.1f, 2000);
	myGloballyLoadedObjects.LightManager->AddLight(LightType::DirectionalLight, dirLight);
}

bool& SceneManager::GetLoadedEnemies()
{
	return myHasLoadedEnemies;
}

void SceneManager::SetGameObjectFactory(std::shared_ptr<GameObjectFactory> aGameObjectFactory)
{
	myGameObjectFactory = aGameObjectFactory;
}

void SceneManager::ReloadScene()
{
	LoadSceneNextFrame(myCurrentLevelPath);
}

void SceneManager::Put(Letter aLetter)
{
	if (aLetter.myType == eLetterRegister::LoadLights)
	{
		myCurrentLevel = 0;
		if (myCurrentLevelPath.string().find('1') != std::string::npos)
		{
			GraphicsEngine::Get().SetCubemapLevel(1);
			myCurrentLevel = 1;
		}
		if (myCurrentLevelPath.string().find('2') != std::string::npos)
		{
			GraphicsEngine::Get().SetCubemapLevel(2);
			myCurrentLevel = 2;
		}
		if (myCurrentLevelPath.string().find('3') != std::string::npos)
		{
			GraphicsEngine::Get().SetCubemapLevel(3);
			myCurrentLevel = 3;
		}
		if (myCurrentLevelPath.string().find('4') != std::string::npos)
		{
			GraphicsEngine::Get().SetCubemapLevel(3);
			myCurrentLevel = 4;
		}
	}
}

std::shared_ptr<GameObjectFactory> SceneManager::GetGameObjectFactory() const
{
	return myGameObjectFactory;
}

void SceneManager::SortObjects()
{
	CU::Vector3f cameraPos = {};
	if (myGloballyLoadedObjects.Camera != nullptr)
	{
		cameraPos = myGloballyLoadedObjects.Camera->GetTransform()->GetPosition();
	}
	for (auto object : myDeferredSortedObjects)
	{
		object.DistanceToCamera = (object.GameObject->GetTransform()->GetPosition() - cameraPos).LengthSqr();
		object.Visible = object.GameObject->GetIsVisible();
	}

	std::sort(myDeferredSortedObjects.begin(), myDeferredSortedObjects.end(), BackToFrontMeshSort);

	for (auto object : myForwardSortedObjects)
	{
		object.DistanceToCamera = (object.GameObject->GetTransform()->GetPosition() - cameraPos).LengthSqr();
		object.Visible = object.GameObject->GetIsVisible();
	}

	std::sort(myForwardSortedObjects.begin(), myForwardSortedObjects.end(), FrontToBackMeshSort);

	for (auto object : myTASortedObjects)
	{
		object.DistanceToCamera = (object.GameObject->GetTransform()->GetPosition() - cameraPos).LengthSqr();
		object.Visible = object.GameObject->GetIsVisible();
	}

	std::sort(myTASortedObjects.begin(), myTASortedObjects.end(), FrontToBackMeshSort);
}

bool SceneManager::LoadPlayerSettings()
{
	std::string filePath = "settings\\PlayerSettings.json";
//	nlohmann::json jsonReader;
//	std::fstream file(filePath);
//	if (!file.good())
//	{
//#ifdef _DEBUG
//		std::cout << "The player settings file could not be loaded. Is it located in bin/settings?";
//#endif
//		return false;
//	}
//	file >> jsonReader;
//	CU::Vector3f lightColor = { jsonReader.at("LightColorR"), jsonReader.at("LightColorG"), jsonReader.at("LightColorB") };
//	CU::Vector3f lightOffset = { jsonReader.at("OffsetX"), jsonReader.at("OffsetY"), jsonReader.at("OffsetZ") };
//	float intensity = jsonReader.at("Intensity");
//	myGloballyLoadedObjects.Player->AddComponent(ComponentSystem::Get()->AddComponent<LightComponent, LIGHT_COMPONENT>());
//	myGloballyLoadedObjects.Player->GetComponent<LightComponent>()->SetGameObject(myGloballyLoadedObjects.Player);
//	myGloballyLoadedObjects.LightManager->AddLight(LightType::PointLight, myGloballyLoadedObjects.Player);


	/*myGloballyLoadedObjects.Player->GetComponent<PlayerComponent>()->LoadSettings(filePath);*/
	return true;
}

void SceneManager::LoadShaderObjects()
{
	std::string filePath = "settings\\VFXSettings.json";
	nlohmann::json jsonReader;
	std::fstream file(filePath);
	if (!file.good())
	{
#ifdef _DEBUG
		std::cout << "The vfx shaderobject settings file could not be loaded. Is it located in bin/settings?";
#endif
		return;
	}
	file >> jsonReader;
	if (jsonReader.contains("VFXObjects"))
	{
		for (auto& json : jsonReader["VFXObjects"])
		{
			myGloballyLoadedObjects.ShaderObjects.emplace_back(std::make_shared<GameObject>());
			std::shared_ptr<GameObject>& newObject = myGloballyLoadedObjects.ShaderObjects.back();
			myGameObjectFactory->Fabricate(newObject, "ShaderObject");
			newObject->GetComponent<ShaderObjectComponent>()->AddEventTrigger(eLetterRegister::AbilityOne);
			myGloballyLoadedObjects.ShaderObjects.back()->GetComponent<ShaderObjectComponent>()->InitFromJson(json);
		}
	}
}

void SceneManager::InitObjectSort()
{
	myDeferredSortedObjects.clear();
	CU::Vector3f cameraPos = myGloballyLoadedObjects.Camera->GetTransform()->GetPosition();

	for (int deferredObjectIndex = 0; deferredObjectIndex < myScene.DeferredObjects.size(); deferredObjectIndex++)
	{
		if (myScene.DeferredObjects[deferredObjectIndex]->GetTransform())
		{
			myDeferredSortedObjects.emplace_back(GameObjectSort{ (myScene.DeferredObjects[deferredObjectIndex]->GetTransform()->GetPosition() - cameraPos).LengthSqr(), myScene.DeferredObjects[deferredObjectIndex] });
		}
	}
	myDeferredSortedObjects.emplace_back(GameObjectSort{ (myGloballyLoadedObjects.Player->GetTransform()->GetPosition() - cameraPos).LengthSqr(), myGloballyLoadedObjects.Player });

	myForwardSortedObjects.clear();
	for (int forwardObjectIndex = 0; forwardObjectIndex < myScene.ForwardObjects.size(); forwardObjectIndex++)
	{
		if (myScene.ForwardObjects[forwardObjectIndex]->GetTransform())
		{
			myForwardSortedObjects.emplace_back(GameObjectSort{ (myScene.ForwardObjects[forwardObjectIndex]->GetTransform()->GetPosition() - cameraPos).LengthSqr(), myScene.ForwardObjects[forwardObjectIndex] });
		}
	}

	myTASortedObjects.clear();
	for (int taObjectIndex = 0; taObjectIndex < myScene.TAObjects.size(); taObjectIndex++)
	{
		if (myScene.TAObjects[taObjectIndex]->GetTransform())
		{
			myTASortedObjects.emplace_back(GameObjectSort{ (myScene.TAObjects[taObjectIndex]->GetTransform()->GetPosition() - cameraPos).LengthSqr(), myScene.TAObjects[taObjectIndex] });
			myScene.TAObjects[taObjectIndex]->SetEnabled(true);
		}
	}

}

void SceneManager::LoadInternal()
{
	myJsonParser.ParseFile(mySceneToLoad.string());
	json levelJson = myJsonParser.GetFileContents(mySceneToLoad.string());

	//ParticleManager::GetInstance().ClearParticles();

	int index = 0;

	for (auto& elem : levelJson["Actors"])
	{
		std::string archetype = elem["Archetype"];
		std::shared_ptr<GameObject> actor = std::make_shared<GameObject>();
		actor->SetName(elem["Name"]);
		for (auto& component : elem["Components"])
		{
			if (component["TypeName"] == "AudioComponent" && component.contains("File"))
			{
				AudioSourceComponent* audioComponent;
				if (!actor->GetComponent<AudioSourceComponent>())
				{
					audioComponent = static_cast<AudioSourceComponent*>(actor->AddComponent(ComponentSystem::Get()->AddComponent<AudioSourceComponent, AUDIO_SOURCE_COMPONENT>()));
					audioComponent->SetGameObject(actor);
				}
				else
				{
					audioComponent = actor->GetComponent<AudioSourceComponent>();
				}
				std::string audioFile = component["File"];
				if (audioFile.ends_with("SFX3D"))
				{
					audioComponent->AddSound(audioFile, SoundType::SFX3D, SoundPriority::Medium);
				}
				else if (audioFile.ends_with("SFX"))
				{
					audioComponent->AddSound(audioFile, SoundType::SFX, SoundPriority::Medium);
				}
				else if (audioFile.ends_with("VOICE"))
				{
					audioComponent->AddSound(audioFile, SoundType::Voice, SoundPriority::High);
				}
				for (auto tag : elem["Tags"])
				{
					audioComponent->TagSound(audioFile, std::string(tag).c_str());
				}

			}
		}

		CollisionManager& collisionMan = *CollisionManager::Get();
		//triggers better hard coded for now to manage everything
		if (archetype.find("Trigger") != std::string::npos)
		{
			std::vector<std::string> tags;
			for (auto tag : elem["Tags"])
			{
				tags.push_back(tag);
			}
			/*actor->AddComponent(ComponentSystem::Get()->AddComponent<Transform, TRANSFORM>())->SetGameObject(actor);
			actor->AddComponent(ComponentSystem::Get()->AddComponent<RigidBodyComponent, RIGID_BODY_COMPONENT>())->SetGameObject(actor);*/
		
			myGameObjectFactory->Fabricate(actor, "Trigger", tags);

			std::array<float, 16> transform;
			for (int i = 0; i < 16; i++)
			{
				transform[i] = elem["Transform"]["transformNoScale"][i];
			}

			CU::Matrix4x4<float> matrix = ExtractTransposedUnrealTransform(transform);
			//matrix = CU::Matrix4x4f::CreateRotationAroundY(3.14f) * matrix;

			CU::Quaternionf rotationQuat;
			rotationQuat.Set(matrix);
			auto rotation = rotationQuat.GetEuler();

			CU::Vector3f scale = { 1.f, 1.f, 1.f };
			if (elem["Transform"].contains("scale"))
			{
				scale = { elem["Transform"]["scale"][0], elem["Transform"]["scale"][2], elem["Transform"]["scale"][1] };
			}

			if (actor->GetComponent<Transform>())
			{
				auto actorTransform = actor->GetComponent<Transform>();
				actorTransform->SetPosition({ matrix[3][0], matrix[3][1], matrix[3][2] });
				actorTransform->SetRotation(rotationQuat);
				actorTransform->SetScale(scale);
			}

			if (elem["Components"][0].contains("Collider"))
			{
				std::cout << elem["Components"][0]["Collider"];
				if (std::string(elem["Components"][0]["Collider"]) == "Capsule")
				{
					actor->GetComponent<RigidBodyComponent>()->Initialize(Squish::Trigger, Squish::Capsule, actor->GetTransform(), { 0,0,0 }, (float)elem["Components"][0]["Radius"], static_cast<float>(elem["Components"][0]["HalfHeight"]));
					collisionMan.AddRigidBody(actor->GetComponent<RigidBodyComponent>());
				}
				else if (std::string(elem["Components"][0]["Collider"]) == "Box")
				{
					actor->GetComponent<RigidBodyComponent>()->Initialize(Squish::Trigger, Squish::Box, actor->GetTransform(), { 0,0,0 }, { static_cast<float>(elem["Components"][0]["Bounds"]["x"]), static_cast<float>(elem["Components"][0]["Bounds"]["y"]), static_cast<float>(elem["Components"][0]["Bounds"]["z"]) });
					collisionMan.AddRigidBody(actor->GetComponent<RigidBodyComponent>());
				}
				else if (std::string(elem["Components"][0]["Collider"]) == "Sphere")
				{
					actor->GetComponent<RigidBodyComponent>()->Initialize(Squish::Trigger, Squish::Sphere, actor->GetTransform(), { 0,0,0 }, (float)elem["Components"][0]["Radius"]);
					collisionMan.AddRigidBody(actor->GetComponent<RigidBodyComponent>());
				}
			}
			myScene.DeferredObjects.push_back(actor);
			myGameObjectMap.emplace(actor->GetName(), actor);
		}
		else if (archetype.find("Light") != std::string::npos) //lightsource
		{
			if (!elem["Components"][0].contains("LightType"))
			{
				index++;
				continue;
			}
			int lightType = elem["Components"][0]["LightType"];
			std::vector<std::string> tags;
			for (auto tag : elem["Tags"])
			{
				tags.push_back(tag);
			}

			myGameObjectFactory->Fabricate(actor, "Light", tags);

			CU::Vector3f color;
			for (int i = 0; i < 3; i++)
			{
				color[i] = elem["Components"][0]["Color"]["color"][i];
			}


			float intensity = elem["Components"][0]["Intensity"];
			if (static_cast<LightType>(lightType) == LightType::DirectionalLight)
			{
				actor->AddComponent(ComponentSystem::Get()->AddComponent<ShadowCameraComponent, SHADOW_CAMERA_COMPONENT>());
				actor->GetComponent<ShadowCameraComponent>()->SetGameObject(actor);
				actor->GetComponent<ShadowCameraComponent>()->Awake();

				intensity = 1;
			}
			float outerConeAngle = 0.0f;
			float attenuationRadius = 0.0f;
			if (static_cast<LightType>(lightType) == LightType::SpotLight)
			{
				//actor->GetComponent<ShadowCameraComponent>()->CreatePerspective(1.0f, 10000000.0f);
				outerConeAngle = elem["Components"][0]["OuterConeAngle"];
				attenuationRadius = elem["Components"][0]["AttenuationRadius"];
				intensity *= 10000.0f;

			}
			else if (static_cast<LightType>(lightType) == LightType::PointLight)
			{
				//actor->GetComponent<ShadowCameraComponent>()->CreatePerspective(1.0f, 10000000.0f);
				attenuationRadius = elem["Components"][0]["AttenuationRadius"];
				intensity *= 10000.0f;
			}

			std::array<float, 16> transform;
			for (int i = 0; i < 16; i++)
			{
				transform[i] = elem["Transform"]["transformNoScale"][i];
			}
			CU::Matrix4x4<float> matrix = ExtractTransposedUnrealTransform(transform);
			//matrix = CU::Matrix4x4f::CreateRotationAroundY(3.14f) * matrix;

			CU::Quaternionf rotationQuat;
			rotationQuat.Set(matrix);
			auto rotation = rotationQuat.GetEuler();

			CU::Vector3f scale = { 1.f, 1.f, 1.f };
			if (elem["Transform"].contains("scale"))
			{
				scale = { elem["Transform"]["scale"][0], elem["Transform"]["scale"][2], elem["Transform"]["scale"][1] };
			}

			if (actor->GetComponent<Transform>())
			{
				auto actorTransform = actor->GetComponent<Transform>();
				actorTransform->SetPosition({ matrix[3][0], matrix[3][1], matrix[3][2] });
				actorTransform->SetRotation(rotationQuat);
				actorTransform->SetScale(scale);
			}
			CommonUtilities::Vector3f translation = matrix.GetTranslation();

			actor->GetComponent<LightComponent>()->Initialize(
				color,
				intensity,
				rotation,
				translation,
				outerConeAngle,
				static_cast<LightType>(lightType),
				attenuationRadius);
			myGloballyLoadedObjects.LightManager->AddLight(static_cast<LightType>(lightType), actor);
			myGameObjectMap.emplace(actor->GetName(), actor);
		}
		else if (archetype.find("StaticMesh") != std::string::npos)
		{

			if (!elem["Components"][0].contains("Mesh"))
			{
				std::cout << "Static Mesh actor without any Mesh components found at slot: " << index << " belonging to " << elem["Name"] << std::endl;
				index++;
				continue;
			}
			std::string meshName = elem["Components"][0]["Mesh"];
			std::vector<std::string> tags;
			for (auto tag : elem["Tags"])
			{
				tags.push_back(tag);
			}
			myGameObjectFactory->Fabricate(actor, "StaticMeshActor", tags);
			//actor->GetComponent<RigidBodyComponent>()->Initialize(Squish::Trigger, Squish::Box, actor->GetTransform(), {1,1,0}, {200,200,200});
			//CollisionManager::Get()->AddRigidBody(actor->GetComponent<RigidBodyComponent>());
			auto mesh = myAssetManagerRef->GetAsset<MeshAsset>(meshName)->myMesh;
			actor->GetComponent<MeshComponent>()->SetMesh(mesh);
			std::vector<std::shared_ptr<Material>> materials;
			for (int matIndex = 0; matIndex < elem["Components"][0]["Materials"].size(); matIndex++)
			{
				std::string materialName = elem["Components"][0]["Materials"][matIndex]["Name"];

				if (materialName == "WorldGridMaterial")
				{
					continue;
				}
				Material current;
				if (elem["Components"][0]["Materials"][matIndex].contains("Textures"))
				{
					for (size_t textureIndex = 0; textureIndex < elem["Components"][0]["Materials"][matIndex]["Textures"].size(); textureIndex++)
					{
						std::string textureName = elem["Components"][0]["Materials"][matIndex]["Textures"][textureIndex]["Name"];

						if (textureName.ends_with("C"))
						{
							current.SetAlbedoTexture(myAssetManagerRef->GetAsset<TextureAsset>(textureName)->myTexture);
							continue;
						}
						if (textureName.ends_with("N"))
						{
							current.SetNormalTexture(myAssetManagerRef->GetAsset<TextureAsset>(textureName)->myTexture);
							continue;
						}
						if (textureName.ends_with("M"))
						{
							current.SetMaterialTexture(myAssetManagerRef->GetAsset<TextureAsset>(textureName)->myTexture);
							continue;
						}
						if (textureName.ends_with("FX"))
						{
							current.SetFXTexture(myAssetManagerRef->GetAsset<TextureAsset>(textureName)->myTexture);
						}
					}
				}

				if (current.GetAlbedoTexture()->myName.empty())
				{
					current.SetAlbedoTexture(myAssetManagerRef->GetAsset<TextureAsset>("t_default_c")->myTexture);
				}
				if (current.GetNormalTexture()->myName.empty())
				{
					current.SetNormalTexture(myAssetManagerRef->GetAsset<TextureAsset>("t_default_n")->myTexture);
				}
				if (current.GetMaterialTexture()->myName.empty())
				{
					current.SetMaterialTexture(myAssetManagerRef->GetAsset<TextureAsset>("t_default_m")->myTexture);
				}
				//if (current.GetFXTexture()->myName.empty())
				//{
				//	current.SetFXTexture(myAssetManagerRef->GetAsset<TextureAsset>("t_default_fx")->myTexture);
				//}
				materials.push_back(std::make_shared<Material>(current));
			}
			actor->GetComponent<MeshComponent>()->SetMaterials(materials);
			actor->SetName(meshName);

			std::array<float, 16> transform;
			for (int i = 0; i < 16; i++)
			{
				transform[i] = elem["Transform"]["transformNoScale"][i];
			}
			CU::Matrix4x4<float> matrix = ExtractTransposedUnrealTransform(transform);
			//matrix = CU::Matrix4x4f::CreateRotationAroundY(3.14159265359f) * matrix;

			CU::Quaternionf rotationQuat;
			rotationQuat.Set(matrix);

			CU::Vector3f scale = { 1.f, 1.f, 1.f };
			if (elem["Transform"].contains("scale"))
			{
				scale = { elem["Transform"]["scale"][0], elem["Transform"]["scale"][2], elem["Transform"]["scale"][1] };
			}

			if (actor->GetComponent<Transform>())
			{
				auto actorTransform = actor->GetComponent<Transform>();
				actorTransform->SetPosition({ matrix[3][0], matrix[3][1], matrix[3][2] });
				actorTransform->SetRotation(rotationQuat);
				actorTransform->SetScale(scale);
			}
			if (actor->GetComponent<RigidBodyComponent>())
			{
				if (meshName.starts_with("SM") || meshName.starts_with("SK"))
				{
					meshName.replace(meshName.begin(), meshName.begin() + meshName.find_first_of("_"), "U");
				}
				auto debug = myAssetManagerRef->GetAsset<ColliderAsset>(meshName);
				actor->GetComponent<RigidBodyComponent>()->CreateFromAsset(debug);
				collisionMan.AddRigidBody(actor->GetComponent<RigidBodyComponent>());
			}

			bool hasTag = false;
			for (auto& tag : elem["Tags"])
			{
				std::string tagg = tag;
				if (tagg == "Forward")
				{
					myScene.ForwardObjects.push_back(actor);
					hasTag = true;
				}
				if (tagg.ends_with("PS"))
				{
					actor->GetComponent<MeshComponent>()->SetPSShader(tagg);
					myScene.TAObjects.push_back(actor);
					hasTag = true;
				}
				else if (tagg.ends_with("VS"))
				{
					actor->GetComponent<MeshComponent>()->SetVSShader(tagg);
					if (!hasTag)
					{
						myScene.TAObjects.push_back(actor);
					}
					hasTag = true;
				}
			}
			if (hasTag)
			{
				continue;
			}
			myScene.DeferredObjects.push_back(actor);
			myGameObjectMap.emplace(actor->GetName(), actor);
		}
		else if (archetype.find("SkeletalMesh") != std::string::npos)
		{
			if (!elem["Components"][0].contains("Mesh"))
			{
				std::cout << "Static Mesh actor without any Mesh components found at slot: " << index << " belonging to " << elem["Name"] << std::endl;
				index++;
				continue;
			}
			std::string meshName = elem["Components"][0]["Mesh"];
			std::vector<std::string> tags;
			for (auto tag : elem["Tags"])
			{
				tags.push_back(tag);
			}

			myGameObjectFactory->Fabricate(actor, "SkeletalMeshActor", tags);

			auto mesh = myAssetManagerRef->GetAsset<MeshAsset>(meshName)->myMesh;
			actor->GetComponent<MeshComponent>()->SetMesh(mesh);
			actor->GetComponent<Animator>()->Init();
			std::vector<std::filesystem::path> matPaths;
			for (int matIndex = 0; matIndex < elem["Components"][0]["Materials"].size(); matIndex++)
			{
				std::string materialName = elem["Components"][0]["Materials"][matIndex]["Name"];
				std::filesystem::path matPath = materialName.c_str();
				matPaths.push_back(matPath);
				matPaths.push_back(matPath);
			}
			if (matPaths.empty())
			{
				matPaths.push_back("M_DeadBushes");
				matPaths.push_back("M_DeadBushes");
			}
			actor->GetComponent<MeshComponent>()->SetMaterials(matPaths);
			actor->SetName(meshName);

			std::array<float, 16> transform;
			for (int i = 0; i < 16; i++)
			{
				transform[i] = elem["Transform"]["transformNoScale"][i];
			}
			CU::Matrix4x4<float> matrix = ExtractTransposedUnrealTransform(transform);
			//matrix = CU::Matrix4x4f::CreateRotationAroundY(3.14f) * matrix;

			CU::Quaternionf rotationQuat;
			rotationQuat.Set(matrix);

			CU::Vector3f scale = { 1.f, 1.f, 1.f };
			if (elem["Transform"].contains("scale"))
			{
				scale = { elem["Transform"]["scale"][0], elem["Transform"]["scale"][2], elem["Transform"]["scale"][1] };
			}

			if (actor->GetComponent<Transform>())
			{
				auto actorTransform = actor->GetComponent<Transform>();
				actorTransform->SetPosition({ matrix[3][0], matrix[3][1], matrix[3][2] });
				actorTransform->SetRotation(rotationQuat);
				actorTransform->SetScale(scale);
			}

			bool hasTag = false;
			for (auto& tag : elem["Tags"])
			{
				std::string tagg = tag;
				if (tagg == "Forward")
				{
					myScene.ForwardObjects.push_back(actor);
					hasTag = true;
				}
				if (tagg.ends_with("PS"))
				{
					actor->GetComponent<MeshComponent>()->SetPSShader(tagg);
					myScene.TAObjects.push_back(actor);
					hasTag = true;
				}
				else if (tagg.ends_with("VS"))
				{
					actor->GetComponent<MeshComponent>()->SetVSShader(tagg);
					myScene.TAObjects.push_back(actor);
					hasTag = true;
				}
			}
			if (hasTag)
			{
				continue;
			}
			myScene.DeferredObjects.push_back(actor);
			myGameObjectMap.emplace(actor->GetName(), actor);
		}
		else if (archetype.find("PlayerStart") != std::string::npos)
		{
			std::array<float, 16> transform;
			for (int i = 0; i < 16; i++)
			{
				transform[i] = elem["Transform"]["transformNoScale"][i];
			}

			CU::Matrix4x4<float> matrix = ExtractTransposedUnrealTransform(transform);

			CU::Quaternionf rotationQuat;
			rotationQuat.Set(matrix);

			myGloballyLoadedObjects.Player->GetComponent<Transform>()->SetPosition({ matrix[3][0], matrix[3][1], matrix[3][2]});
			myGloballyLoadedObjects.Player->GetComponent<Transform>()->SetRotation(rotationQuat);
			myGloballyLoadedObjects.Player->Awake();

		}
		else if (archetype.find("Landscape") != std::string::npos)
		{
			std::vector<std::string> tags;
			for (auto tag : elem["Tags"])
			{
				tags.push_back(tag);
			}

			myGameObjectFactory->Fabricate(actor, "Landscape", tags);
			for (std::string tag : tags)
			{
				if (tag.find("SM_Landscape") != std::string::npos)
				{
					auto mesh = myAssetManagerRef->GetAsset<MeshAsset>(tag)->myMesh;
					actor->GetComponent<MeshComponent>()->SetMesh(mesh);
					/*std::array<float, 16> transform;
					for (int i = 0; i < 16; i++)
					{
						transform[i] = elem["Transform"]["transformNoScale"][i];
					}
					actor->GetComponent<Transform>()->SetPosition({ -transform[12], transform[14], transform[13] });*/
					//actor->GetTransform()->SetScale(CommonUtilities::Vector3f(elem["Transform"]["scale"][0], elem["Transform"]["scale"][2], elem["Transform"]["scale"][1]));
					std::string path = ("UME") + tag.substr(tag.find_last_of("_"));
					actor->GetComponent<RigidBodyComponent>()->CreateFromAsset(myAssetManagerRef->GetAsset<ColliderAsset>(path));
					PxFilterData filter;
					filter.word0 = 1|1<<1;
					actor->GetComponent<RigidBodyComponent>()->SetQueryFilterData(filter);
					CollisionManager::Get()->AddRigidBody(actor->GetComponent<RigidBodyComponent>());
				}
				if (tag.find("M_Ground") != std::string::npos)
				{
					Material current;

					current.SetAlbedoTexture(myAssetManagerRef->GetAsset<TextureAsset>("t_snow_c")->myTexture);
					current.SetNormalTexture(myAssetManagerRef->GetAsset<TextureAsset>("t_snow_n")->myTexture);
					current.SetMaterialTexture(myAssetManagerRef->GetAsset<TextureAsset>("t_snow_m")->myTexture);

					std::vector<std::shared_ptr<Material>> mats;
					mats.push_back(std::make_shared<Material>(current));
					actor->GetComponent<MeshComponent>()->SetMaterials(mats);
				}
			}
			myScene.DeferredObjects.push_back(actor);
			myGameObjectMap.emplace(actor->GetName(), actor);
		}
		else if (archetype.find("BP_") != std::string::npos)
		{
			std::array<float, 16> bluePrintTransform;
			for (int i = 0; i < 16; i++)
			{
				bluePrintTransform[i] = elem["Transform"]["transformNoScale"][i];
			}
			CU::Matrix4x4<float> bluePrintMatrix = ExtractUnrealTransform(bluePrintTransform);

			CU::Vector3f bluePrintScale = { 1.f, 1.f, 1.f };
			if (elem["Transform"].contains("scale"))
			{
				bluePrintScale = { elem["Transform"]["scale"][0], elem["Transform"]["scale"][2], elem["Transform"]["scale"][1] };
			}

			std::vector<std::string> bPtags;
			for (auto& tag : elem["Tags"])
			{
				bPtags.push_back(tag);
			}
			for (auto& component : elem["Components"])
			{
				std::string name = component["Name"];
				if (name.find("DefaultSceneRoot") != std::string::npos)
				{
					continue;
				}
				std::shared_ptr<GameObject> componentActor = std::make_shared<GameObject>();
				std::string typeName = component["TypeName"];

				std::vector<std::string> compTags;

				for (auto& tag : component["Tags"])
				{
					compTags.push_back(tag);
				}
				if (!bPtags.empty())
				{
					for (int i = 0; i < bPtags.size(); i++)
					{
						compTags.push_back(bPtags[i]);
					}
				}

				myGameObjectFactory->Fabricate(componentActor, typeName, compTags);

				std::array<float, 16> componentTransform;
				for (int i = 0; i < 16; i++)
				{
					componentTransform[i] = static_cast<float>(component["Transform"]["transformNoScale"][i]);
				}

				CU::Matrix4x4<float> componentMatrix = ExtractUnrealTransform(componentTransform);

				//Needed to corret (at least) 0 degrees and 180 degrees rotations because of reasons related to different spaces.
				componentMatrix = CU::Matrix4x4f::CreateRotationAroundY(1.5707963f) * componentMatrix;

				CU::Vector3f componentScale = { 1.f, 1.f, 1.f };
				if (component["Transform"].contains("scale"))
				{
					componentScale = { component["Transform"]["scale"][0], component["Transform"]["scale"][2], component["Transform"]["scale"][1] };
				}

				if (componentActor->GetComponent<Transform>())
				{
					
					auto netMatrix =  componentMatrix * bluePrintMatrix;
					auto netPos = netMatrix.GetRow(4);
					CU::Quaternionf rotationQuat;
					rotationQuat.Set(netMatrix);
					auto actorTransform = componentActor->GetComponent<Transform>();
					actorTransform->SetPosition({ netMatrix[3][0], netMatrix[3][1], netMatrix[3][2] });
					actorTransform->SetRotation(rotationQuat);
					auto netScale = (componentScale + bluePrintScale) / 2.f;
					if(abs(netScale.x - 1.f ) > 0.0001f)
					{
						std::cout << "danger";
					}
					actorTransform->SetScale(netScale);
					if(abs(actorTransform->GetMatrix()(1,1) - netMatrix(1, 1)) > 0.0001f || abs(actorTransform->GetMatrix()(2, 2) - netMatrix(2, 2)) > 0.0001f || abs(actorTransform->GetMatrix()(3, 3) - netMatrix(3, 3)) > 0.0001f)
					{
 						std::cout << "danger";
						actorTransform->RecalculateCache();
						rotationQuat.Set(netMatrix);

					}
					printf("throw");

				}

				if (componentActor->GetComponent<MeshComponent>())
				{
					std::string meshName;
					if (component.contains("Mesh"))
					{
						meshName = component["Mesh"];
					}
					auto mesh = myAssetManagerRef->GetAsset<MeshAsset>(meshName)->myMesh;
					componentActor->GetComponent<MeshComponent>()->SetMesh(mesh);
					std::vector<std::shared_ptr<Material>> materials;
					for (int matIndex = 0; matIndex < component["Materials"].size(); matIndex++)
					{
						std::string materialName = component["Materials"][matIndex]["Name"];

						if (materialName == "WorldGridMaterial")
						{
							continue;
						}
						Material current;
						if (component["Materials"][matIndex].contains("Textures"))
						{
							for (size_t textureIndex = 0; textureIndex < component["Materials"][matIndex]["Textures"].size(); textureIndex++)
							{
								std::string textureName = component["Materials"][matIndex]["Textures"][textureIndex]["Name"];

								if (textureName.ends_with("C"))
								{
									current.SetAlbedoTexture(myAssetManagerRef->GetAsset<TextureAsset>(textureName)->myTexture);
									continue;
								}
								if (textureName.ends_with("N"))
								{
									current.SetNormalTexture(myAssetManagerRef->GetAsset<TextureAsset>(textureName)->myTexture);
									continue;
								}
								if (textureName.ends_with("M"))
								{
									current.SetMaterialTexture(myAssetManagerRef->GetAsset<TextureAsset>(textureName)->myTexture);
									continue;
								}
								if (textureName.ends_with("FX"))
								{
									current.SetFXTexture(myAssetManagerRef->GetAsset<TextureAsset>(textureName)->myTexture);
								}
							}
						}
						if (current.GetAlbedoTexture()->myName.empty())
						{
							current.SetAlbedoTexture(myAssetManagerRef->GetAsset<TextureAsset>("t_default_c")->myTexture);
						}
						if (current.GetNormalTexture()->myName.empty())
						{
							current.SetNormalTexture(myAssetManagerRef->GetAsset<TextureAsset>("t_default_n")->myTexture);
						}
						if (current.GetMaterialTexture()->myName.empty())
						{
							current.SetMaterialTexture(myAssetManagerRef->GetAsset<TextureAsset>("t_default_m")->myTexture);
						}
						//if (current.GetFXTexture()->myName.empty())
						//{
						//	current.SetFXTexture(myAssetManagerRef->GetAsset<TextureAsset>("t_default_fx")->myTexture);
						//}

						materials.push_back(std::make_shared<Material>(current));
					}
					componentActor->GetComponent<MeshComponent>()->SetMaterials(materials);
					componentActor->SetName(meshName+"_"+actor->GetName());
					myScene.DeferredObjects.push_back(componentActor);
					myGameObjectMap.emplace(componentActor->GetName(), componentActor);
				}

			}
		}
		index++;
	}

	std::string sceneName = mySceneToLoad.filename().string();
	myScene.IsGameScene = true;
	std::cout << "loaded scene " << sceneName << std::endl;


	/*std::shared_ptr<GameObject> ground = std::make_shared<GameObject>();
	ground->AddComponent(ComponentSystem::Get()->AddComponent<MeshComponent, MESH_COMPONENT>())->SetGameObject(ground);
	ground->GetComponent<MeshComponent>()->SetMesh(myAssetManagerRef->GetAsset<MeshAsset>("SM_Collision_PlayerGymPolyreduced")->myMesh);
	ground->AddComponent(ComponentSystem::Get()->AddComponent<RigidBodyComponent, RIGID_BODY_COMPONENT>());
	ground->GetComponent<RigidBodyComponent>()->CreateFromAsset(myAssetManagerRef->GetAsset<ColliderAsset>("UCX_Collision_PlayerGym"));
	CollisionManager::Get()->AddRigidBody(ground->GetComponent<RigidBodyComponent>());
	AddObject(ground);*/
	InitObjectSort();
	myGloballyLoadedObjects.LightManager->InitLights();
	myGloballyLoadedObjects.LightManager->GetDirLight()->GetComponent<LightComponent>()->InitDirLightMatrices(myGloballyLoadedObjects.Player->GetTransform()->GetMatrixScale(), 50000.0f);
	myIsLoadingScene.store(false, std::memory_order_release);
	mySceneToLoad.clear();

	//SendEvent that the loaded scene was done! -> Is done
	myHasLoadedScene = true;

	myFirst = true;

}

void SceneManager::ViewFrustumCulling()
{
	for (auto& object : myScene.DeferredObjects)
	{
		if (!object->GetComponent<MeshComponent>())
		{
			continue;
		}
		CU::AABB3Df localBox = object->GetComponent<MeshComponent>()->GetBoundingBox();
		CU::Vector4f worldMin = localBox.GetMin().ToVector4One() * object->GetTransform()->GetMatrixScale();
		CU::Vector4f worldMax = localBox.GetMax().ToVector4One() * object->GetTransform()->GetMatrixScale();
		//object->SetIsVisible(myGloballyLoadedObjects.Camera->GetComponent<MVCameraComponent>()->IsInsideFrustum(worldMin.ToVector3(), worldMax.ToVector3()));
	}
}

CU::Matrix4x4f SceneManager::ExtractUnrealTransform(std::array<float, 16> unrealMatrix)
{
	CU::Matrix4x4f outMatrix = CU::Matrix4x4f(unrealMatrix);
	CU::Vector4f r1 = outMatrix.GetRow(1);
	CU::Vector4f r2 = outMatrix.GetRow(2);
	CU::Vector4f r3 = outMatrix.GetRow(3);
	CU::Vector4f r4 = outMatrix.GetRow(4);
	r1 = CU::Vector4f(r1.y, r1.z, r1.x, r1.w);
	r2 = CU::Vector4f(r2.y, r2.z, r2.x, r2.w);
	r3 = CU::Vector4f(r3.y, r3.z, r3.x, r3.w);
	r4 = CU::Vector4f(r4.y, r4.z, r4.x, r4.w);
	outMatrix.SetRow(r2, 1);
	outMatrix.SetRow(r3, 2);
	outMatrix.SetRow(r1, 3);
	outMatrix.SetRow(r4, 4);
	outMatrix(1, 4) = 0;
	outMatrix(2, 4) = 0;
	outMatrix(3, 4) = 0;
	return outMatrix;


	////Viktors edition
	//CU::Matrix4x4f outMatrix = CU::Matrix4x4f(unrealMatrix);
	////CU::Vector4f r1 = outMatrix.GetRow(1);
	////CU::Vector4f r2 = outMatrix.GetRow(2);
	////CU::Vector4f r3 = outMatrix.GetRow(3);
	////CU::Vector4f r4 = outMatrix.GetRow(4);
	////r1 = CU::Vector4f(r1.x, r1.z, r1.y, r1.w);
	////r2 = CU::Vector4f(r2.x, r2.z, r2.y, r2.w);
	////r3 = CU::Vector4f(r3.x, r3.z, r3.y, r3.w);
	////r4 = CU::Vector4f(r4.x, r4.z, r4.y, r4.w);
	////outMatrix.SetRow(r2, 1);
	////outMatrix.SetRow(r3, 2);
	////outMatrix.SetRow(r1, 3);
	////outMatrix.SetRow(r4, 4);
	////outMatrix(1, 4) = 0;
	////outMatrix(2, 4) = 0;
	////outMatrix(3, 4) = 0;
	//return outMatrix;	
}

CU::Matrix4x4f SceneManager::ExtractTransposedUnrealTransform(std::array<float, 16> unrealMatrix)
{
	CU::Matrix4x4f outMatrix = CU::Matrix4x4f(unrealMatrix);
	CU::Vector4f r1 = outMatrix.GetRow(1);
	CU::Vector4f r2 = outMatrix.GetRow(2);
	CU::Vector4f r3 = outMatrix.GetRow(3);
	CU::Vector4f r4 = outMatrix.GetRow(4);
	r1 = CU::Vector4f(r1.y, r1.z, r1.x, r1.w);
	r2 = CU::Vector4f(r2.y, r2.z, r2.x, r2.w);
	r3 = CU::Vector4f(r3.y, r3.z, r3.x, r3.w);
	r4 = CU::Vector4f(r4.y, r4.z, r4.x, r4.w);
	outMatrix.SetRow(r2, 1);
	outMatrix.SetRow(r3, 2);
	outMatrix.SetRow(r1, 3);
	outMatrix = CU::Matrix4x4f::CreateRotationAroundY(1.57f) * outMatrix;
	//outMatrix = outMatrix.GetTranspose();
	outMatrix.SetRow(r4, 4);
	outMatrix(1, 4) = 0;
	outMatrix(2, 4) = 0;
	outMatrix(3, 4) = 0;
	return outMatrix;
}

//void SceneManager::RadiusCulling(const float aRadius)
//{
//	const CU::Vector3f& cullingCenter = myGloballyLoadedObjects.Player->GetTransform()->GetPosition();
//
//	for (auto& object : myScene.DeferredObjects)
//	{
//		const float distanceSqr = (object->GetTransform()->GetMatrixScale().GetTranslation() - cullingCenter).LengthSqr();
//		distanceSqr > (aRadius * aRadius) ? object->SetIsVisible(false) : object->SetIsVisible(true);
//	}
//}

void SceneManager::DebugRendering()
{
	for (auto& object : myDeferredSortedObjects)
	{
		object.GameObject->DebugRender();
	}
	myGloballyLoadedObjects.Camera->DebugRender();
	//myGloballyLoadedObjects.LightManager->GetDirLight()->GetComponent<ShadowCameraComponent>()->DebugRender();
	for (auto& forwardObject : myForwardSortedObjects)
	{
		forwardObject.GameObject->DebugRender();
	}
	for (auto& taObject : myTASortedObjects)
	{
		taObject.GameObject->DebugRender();
	}
	for (auto& shaderObject : myGloballyLoadedObjects.ShaderObjects)
	{
		shaderObject->DebugRender();
	}
}

void SceneManager::LoadCameraSettings()
{
	char path[] = "settings\\PlayerSettings.json";
	GaiJsonParser parse;
	json mydata = parse.GetFileContents(path);

	CU::Vector3f position = { 0, 100, -1000 }; // default values if json fails

	if (mydata.contains("CameraWidthOffset"))
	{
		position.x = mydata.at("CameraWidthOffset");
	}
	if (mydata.contains("CameraHeight"))
	{
		position.y = mydata.at("CameraHeight");
	}
	if (mydata.contains("CameraDistanceToPlayer"))
	{
		position.z = mydata.at("CameraDistanceToPlayer");
	}

	myGloballyLoadedObjects.Camera->GetTransform()->SetPosition(position);
}

void SceneManager::AddObjectToMap(const char* aName, std::shared_ptr<GameObject> aObject)
{
	if(myGameObjectMap.contains(aName))
	{
		throw("This object already exists %s", aName);
	}
}

GameObject* SceneManager::GetGameoObject(const char* aObjectName)
{
	if(myGameObjectMap.contains(aObjectName))
	{
		return myGameObjectMap.at(aObjectName).get();
	}
	throw("Error gameobject with this id %s does not exist", aObjectName);
}
