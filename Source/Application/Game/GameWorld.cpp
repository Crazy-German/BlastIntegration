#include "GameWorld.h"

#include <filesystem>
#include <future>

#include "MainSingleton.h"
#include "wrl.h"
#include "CollisionHandling/CollisionManager.h"
#include "DearImGui/imgui_impl_dx11.h"
#include "DearImGui/imgui_impl_win32.h"
#include <GraphicsEngine/FrameBuffer.h>

#include "AssetManagement/AssetTypes/MeshAsset.h"
#include "extensions/PxSimpleFactory.h"
#include "GameObjectSystem/Components/CharacterControllerComponent.h"
#include "GameObjectSystem/Components/LevelSwapComponent.h"
#include "MuninGraph/TimerManager.h"
#include "Physcics/BlastAsset.h"
#include "Physcics/BlastManager.h"
#include "Physcics/PhysXScene.h"

using namespace Microsoft::WRL;

#include "Application.h"
#include "myGui.h"

#include "AssetManagement/AssetManager.h"
#include "GameObjectSystem/Components/Animations/Animator.h"
#include "GameObjectSystem/Components/MeshComponent.h"

#include "CommonUtilities/Input.h"
#include "CommonUtilities/Timer.h"
#include "CommonUtilities/CommonMath.hpp"
#include "GraphicsEngine/GraphicsCommands.h"
#include "MainSingleton.h"
#include <Interface/InterfaceManager.h>
#include "External\nlohmann\json.hpp"
#include "../FeatherEngine/LevelManagement/JsonParser.h"
#include "GameObjectSystem\Components\ControllerComponent.h"
#include "GameObjectSystem\Components\AnimationTriggerComponent.h"
#include "Components/PlayerComponent.h"

#include <Squish\Physcics\PhysicsEngine.h>
#include "GameObjectSystem/ComponentSystem.h"
#include "CommonUtilities\ScopedTimer.h"

#include "KebabenekaiseFactory.h"
#include "Components/FootStepsComponent.h"



GameWorld::GameWorld()
{
	myCurrentAnimation = 0;
	myIsRunning = false;
	myMainWindowHandle = nullptr;
	myTimer = &CommonUtilities::Timer::Get();
}
GameWorld::~GameWorld() = default;

bool GameWorld::Initialize(SIZE aWindowSize, WNDPROC aWindowProcess, LPCWSTR aWindowTitle)
{
	constexpr LPCWSTR windowClassName = L"ModelViewerMainWindow";

	// First we create our Window Class
	WNDCLASS windowClass = {};
	windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = aWindowProcess;
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.lpszClassName = windowClassName;
	RegisterClass(&windowClass);

	LONG posX = (GetSystemMetrics(SM_CXSCREEN) - aWindowSize.cx) / 2;
	if (posX < 0)
		posX = 0;

	LONG posY = (GetSystemMetrics(SM_CYSCREEN) - aWindowSize.cy) / 2;
	if (posY < 0)
		posY = 0;

	// Then we use the class to create our window
	myMainWindowHandle = CreateWindow(
		windowClassName,                                // Classname
		aWindowTitle,                                   // Window Title
		WS_POPUP | WS_VISIBLE,                 // Flags
		posX,
		posY,
		aWindowSize.cx,
		aWindowSize.cy,
		nullptr, nullptr, nullptr,
		nullptr
	);

	if (true)
	{
		//SetWindowLongPtr(myMainWindowHandle, GWL_STYLE, WS_VISIBLE | WS_POPUP);
		//SetWindowPos(myMainWindowHandle, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_NOOWNERZORDER | SWP_NOZORDER);
	}

	//Load engine settings
	MainSingleton::Get().InitMiscBuffer(new MiscBuffer());
	MainSingleton::Get().GetAudioEngine().InitAndLoadAllFiles();
	EngineSettings::Get().Initialize(myMainWindowHandle);
	Squish::InitializeSquish();
	Squish::SetCallback(CollisionManager::Get());

	{   // Graphics CreateModelViewerScene
		MVLOG(Log, "Initializing Graphics Engine...");

		if (!GraphicsEngine::Get().Initialize(myMainWindowHandle, EngineSettings::Get().GetResolution().width, EngineSettings::Get().GetResolution().height))
			return false;
	}

	ComponentSystem::Get()->RegisterComponent<PlayerComponent, PLAYER_COMPONENT>();

	MapInputs();

	auto startDir = std::filesystem::current_path().u8string();

	startDir.append(u8"\\Assets");
	const std::filesystem::path contentDir = startDir;
	std::cout << contentDir << std::endl;
	AssetManager::Get().Initialize(contentDir);

	MainSingleton::Get().GetSceneManager().Init();
	MainSingleton::Get().GetSceneManager().SetGameObjectFactory(std::make_shared<KebabenekaiseFactory>(ComponentSystem::Get()));
	MainSingleton::Get().GetSceneManager().Awake();
	MainSingleton::Get().GetSceneManager().LoadGlobalObjects();
	MainSingleton::Get().SetWindowHandle(myMainWindowHandle);
	InitPlayer();

	//CU::Input::LockMouseToCenter(true, myMainWindowHandle);

	InterfaceManager::Get()->Initialize(true);
#ifndef _RETAIL
	myGui = std::make_unique<MyGui>(MainSingleton::Get().GetSceneManager());
	myGui->Init(myMainWindowHandle, GraphicsEngine::Get().GetDevice(), GraphicsEngine::Get().GetContext());
	myGui->SetActive(true);
#endif 


	FrameBuffer buffer;
	buffer.Resolution = CU::Vector2f(static_cast<float>(GraphicsEngine::Get().GetViewPort().Width), static_cast<float>(GraphicsEngine::Get().GetViewPort().Height));
	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::FrameBuffer, buffer);

	ShowWindow(myMainWindowHandle, SW_SHOW);
	SetForegroundWindow(myMainWindowHandle);
	float halfSize = 50;
	//AssetManager::Get().GetAsset<MeshAsset>("SM_Container")->myMesh->GetVertices();
	Mesh mesh;
	mesh.GenerateCube(50);
	//mesh = *AssetManager::Get().GetAsset<MeshAsset>("SM_FlatCart")->myMesh;
	std::vector<CommonUtilities::Vector3f> pos;
	std::vector<CommonUtilities::Vector3f> norm;
	std::vector<CommonUtilities::Vector2f> uv;
	
	for(const auto& vertex : mesh.GetVertices())
	{
		pos.push_back(vertex.Position.ToVector3());
		norm.push_back(vertex.Normal);
		uv.push_back(vertex.UV);
	}
	myPlane = new Squish::RigidBody();
	myPlaneTransform.SetPosition({ -5000, -200, -5000 });
	
	myPlane->Initialize(Squish::RigidBodyType::Static, Squish::ShapeType::Box, &myPlaneTransform,{ 0,0,0 }, { 5000, 100, 5000 });
	Squish::PhysicsEngine::Get()->GetScene()->AddActor(myPlane);

	myBlastAsset = BlastManager::Get()->CreateNewAsset(pos, norm, uv,  mesh.GetIndices(), 4);
	myBlastAsset->SetPosition({0,200,0});
	myBlastAsset->Hit();

	return true;
}

int GameWorld::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	//CreateModelViewerScene();

	myIsRunning = true;
	ComponentSystem::Get()->Awake();
	//std::filesystem::path level = "Assets/Exported Levels/l_assetgym/L_AssetGym_Level.json";
	//std::filesystem::path level = "Assets\\Exported Levels\\l_level3\\L_Level3_Level.json";
	//std::filesystem::path level = "Assets/Exported Levels/l_playergym/L_PlayerGym_Level.json";
	//std::filesystem::path level = "Assets/Exported Levels/l_assetgym/TestExportMap_Level.json"; 
	//std::filesystem::path level = "Assets/Exported Levels/l_assetgym/L_AssetGym_Level.json";//TODO change file path to other gyms
	//MainSingleton::Get().GetSceneManager().LoadScene(level);
	while (myIsRunning)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				myIsRunning = false;
#ifndef _RETAIL
				ImGui_ImplDX11_Shutdown();
				ImGui_ImplWin32_Shutdown();
				ImGui::DestroyContext();
#endif
				return 0;
			}
		}

		//if (!InterfaceManager::Get()->GetLockMouse())
		//{
		//	MainSingleton::Get().SetMouseLock(true);
		//	ShowCursor(false);
		//}
		//else
		//{
		//	if()
		//	{
		//		MainSingleton::Get().SetMouseLock(false);
		//		ShowCursor(true);
		//	}
		//}

		if(CU::Input::GetKeyDown(CU::Keys::M))
		{
			myLockMouse = !myLockMouse;
		}

		/*if (GetActiveWindow() == myMainWindowHandle)
		{
			if (InterfaceManager::Get()->GetLockMouse())
			{
				if(!myLockMouse)
				{
					MainSingleton::Get().SetMouseLock(false);
					ShowCursor(true);
				}
				else
				{
					MainSingleton::Get().SetMouseLock(true);
					SetCursor(NULL);
				}
			}
			else
			{
				MainSingleton::Get().SetMouseLock(false);
				ShowCursor(true);
			}
		}
		else
		{*/
			MainSingleton::Get().SetMouseLock(false);
			ShowCursor(true);
		//}


#ifndef YIELD_IMPLEMENTATION
		CheckLoadingDone(); //TODO: not great to do this every frame
#endif // !YIELD_IMPLEMENTATION


		myGui->Update(myTimer->GetDeltaTime());

		MainSingleton::Get().GetRenderer().RenderFrame();
		if (myGui)
		{
			myGui->Render();
		}
		GraphicsEngine::Get().Present();
	}

	return 0;
}

int GameWorld::RunFromUnreal(const std::string& aLevel)
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	//CreateModelViewerScene();

	myIsRunning = true;
	ComponentSystem::Get()->Awake();
	std::filesystem::path level = aLevel;
	MainSingleton::Get().GetSceneManager().LoadScene(level);
	while (MainSingleton::Get().GetSceneManager().GetIsLoading())
	{

	}
	myHasLoadedScene = true;
	while (myIsRunning)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				myIsRunning = false;
#ifndef _RETAIL
				ImGui_ImplDX11_Shutdown();
				ImGui_ImplWin32_Shutdown();
				ImGui::DestroyContext();
#endif
				return 0;
			}
		}


		myGui->Update(myTimer->GetDeltaTime());
		MainSingleton::Get().GetRenderer().RenderFrame();
		if (myGui)
		{
			myGui->Render();
		}
		GraphicsEngine::Get().Present();
	}

	return 0;
}

void GameWorld::RegisterAssets()
{
}

void GameWorld::CreateScene()
{

}

void GameWorld::UpdateScene(float aTimeDelta)
{
	float cappedDelta = aTimeDelta;
	if (cappedDelta > 0.0333333f)
	{
		cappedDelta = 0.03333f;
	}
	Squish::Update(cappedDelta);
	BlastManager::Get()->Update();
	//MainSingleton::Get().GetSceneManager().UpdateScene(cappedDelta);
	//ComponentSystem::Get()->Update(cappedDelta);
	//MainSingleton::Get().GetAudioEngine().Update(aTimeDelta);
	//InterfaceManager::Get()->Update(cappedDelta);
	//if (!InterfaceManager::Get()->IsInMenu())
	{
		//MainSingleton::Get().GetSceneManager().UpdateScene(cappedDelta);

		//CollisionManager::Get()->Update(cappedDelta);
	}
}

void GameWorld::RenderScene()
{
	MainSingleton::Get().GetSceneManager().RenderScene();

}

HWND GameWorld::GetWindowHandle()
{
	return myMainWindowHandle;
}

void GameWorld::GameLoop()
{
#ifdef YIELD_IMPLEMENTATION
	do
	{
		CheckLoadingDone();
		std::this_thread::yield();
	} while (!myHasLoadedScene);

#else
	std::unique_lock<std::mutex> conditionLock(myLoadLock);
	myLoadCondition.wait(conditionLock, [&] { return myHasLoadedScene = true; });
#endif

	//For fixed update timing
	constexpr float fps = 120.f;
	constexpr float fixedDelta = (1000.0f / fps);

	auto startTime = std::chrono::high_resolution_clock::now();
	double fractionalTime = 0;
	unsigned warmupFrames = 5; // just to not update the first few frames where delta is very very low


	while (myIsRunning)
	{
		const auto currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> frametime = currentTime - startTime;

		const auto passedMS = frametime.count() + fractionalTime;
		if (static_cast<float>(passedMS) <= fixedDelta)
		{
			continue;
		}
		startTime = currentTime;
		fractionalTime = CU::Max(static_cast<float>(passedMS) - fixedDelta, 0.0f);

		myTimer->Update();
		auto deltaTime = myTimer->GetDeltaTime();

		if (warmupFrames > 0)
		{
			warmupFrames--;
			continue;
		}
		CommonUtilities::Input::Update();

		if (GetKeyState(static_cast<int>(CU::Keys::B)) & 0x8000)
		{
			myBlastAsset->Hit();
			//InterfaceManager::Get()->StopSplashScreen();
		}

		CU::Vector2f mousePos(static_cast<float>(CU::Input::GetMousePosition().x), static_cast<float>(CU::Input::GetMousePosition().y));
		if (InterfaceManager::Get()->Input(mousePos))
		{

		}
		if (!MainSingleton::Get().GetSceneManager().GetIsLoading())
		{
			MainSingleton::Get().GetInputMapper().Refresh();
		}

		UpdateScene(deltaTime);
		RenderScene();

	}
}

void GameWorld::RecieveEvent(const ActionEvent& anEvent)
{
	anEvent;
#ifndef RETAIL_

#endif
}

void GameWorld::MapInputs()
{
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::FlyDown, ActionEventID::FlyDown);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::FlyUp, ActionEventID::FlyUp);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::MoveBackwards, ActionEventID::MoveBackwards);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::MoveForward, ActionEventID::MoveForward);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::MoveLeft, ActionEventID::MoveLeft);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::MoveRight, ActionEventID::MoveRight);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::Jump, ActionEventID::Jump);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::Scream, ActionEventID::Scream);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::Charge, ActionEventID::Charge);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::Rotation, ActionEventID::Rotation);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::Toggle_DirLight, ActionEventID::Toggle_DirLight);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::Toggle_PointLight, ActionEventID::Toggle_PointLight);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::Toggle_SpotLight, ActionEventID::Toggle_SpotLight);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::Middle_Mouse, ActionEventID::Walk);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::Left_Click, ActionEventID::Left_Click);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::Shift_Plus_Left, ActionEventID::Attack);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::Change_DebugPass, ActionEventID::Change_DebugPass);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::RotationX, ActionEventID::CameraMovedX);
	MainSingleton::Get().GetInputMapper().BindEvent(GameInput::RotationY, ActionEventID::CameraMovedY);
	MainSingleton::Get().GetInputMapper().BindKey(CommonUtilities::Keys::F8, ActionEventID::EnableColliderDebug);
	MainSingleton::Get().GetInputMapper().BindKey(CommonUtilities::Keys::F9, ActionEventID::RenderNavMesh);
	/*MainSingleton::Get().GetInputMapper().BindKey(CommonUtilities::Keys::MOUSELBUTTON, ActionEventID::Left_Click);
	MainSingleton::Get().GetInputMapper().BindKey(CommonUtilities::Keys::MOUSERBUTTON, ActionEventID::HeavyAttack);
	MainSingleton::Get().GetInputMapper().BindKey(CommonUtilities::Keys::N1, ActionEventID::AbilityOne);
	MainSingleton::Get().GetInputMapper().BindKey(CommonUtilities::Keys::N2, ActionEventID::AbilityTwo);
	MainSingleton::Get().GetInputMapper().BindKey(CommonUtilities::Keys::N3, ActionEventID::AbilityThree);
	MainSingleton::Get().GetInputMapper().BindKey(CommonUtilities::Keys::A, ActionEventID::AbilityOne);
	MainSingleton::Get().GetInputMapper().BindKey(CommonUtilities::Keys::S, ActionEventID::AbilityTwo);
	MainSingleton::Get().GetInputMapper().BindKey(CommonUtilities::Keys::D, ActionEventID::AbilityThree);
	MainSingleton::Get().GetInputMapper().BindKey(CommonUtilities::Keys::MBUTTON, ActionEventID::Walk);*/
	MainSingleton::Get().GetInputMapper().BindKey(CU::Keys::LEFT, ActionEventID::MoveCamLeft);
	MainSingleton::Get().GetInputMapper().BindKey(CU::Keys::RIGHT, ActionEventID::MoveCamRight);
	MainSingleton::Get().GetInputMapper().BindKey(CU::Keys::UP, ActionEventID::MoveCamForward);
	MainSingleton::Get().GetInputMapper().BindKey(CU::Keys::DOWN, ActionEventID::MoveCamBackwards);
}

void GameWorld::InitPlayer()
{
	std::shared_ptr<GameObject> player = MainSingleton::Get().GetSceneManager().GetGlobalObjects().Player;
	player->AddComponent(ComponentSystem::Get()->AddComponent<PlayerComponent, PLAYER_COMPONENT>())->SetGameObject(player);
	player->Awake();
	player->GetComponent<PlayerComponent>()->Init();
	player->GetComponent<PlayerComponent>()->LoadSettings("settings\\PlayerSettings.json");
	player->AddComponent(ComponentSystem::Get()->AddComponent<CharacterControllerComponent, CHARACTER_CONTROLLER_COMPONENT>())->SetGameObject(player);
	player->GetComponent<CharacterControllerComponent>()->Init(player->GetTransform(), 25, 50);

	player->GetComponent<PlayerComponent>()->SetCamera(*MainSingleton::Get().GetSceneManager().GetGlobalObjects().Camera);
	//CollisionManager::Get()->AddRigidBody(player->GetComponent<CharacterControllerComponent>());

	//player->GetComponent<RigidBodyComponent>()->Initialize(Squish::RigidBodyType::Dynamic, Squish::ShapeType::Capsule, player->GetTransform(), { PX_MAX_F32*0.5, PX_MAX_F32*0.5, 0.0f }, 25.f,50.f);
	//player->GetComponent<RigidBodyComponent>()->SetLinearDamping(0.05f);
	//player->GetComponent<RigidBodyComponent>()->SetAngularDamping(0.05f);
	//player->GetComponent<RigidBodyComponent>()->GetPhysxActor().is<PxRigidBody>()->setMinCCDAdvanceCoefficient(0.05f);
	//player->GetComponent<RigidBodyComponent>()->SetSolverIteratiorCount(40,10);
	////player->GetComponent<RigidBodyComponent>()->SetKinematic(true);
	//player->GetComponent<RigidBodyComponent>()->SetMass(100);
	//player->GetComponent<RigidBodyComponent>()->PrintFrictions();

	//player->GetComponent<RigidBodyComponent>()->SetIsAffectedByGravity(false);

	if (player->GetComponent<Animator>())
	{
		player->AddComponent(ComponentSystem::Get()->AddComponent<FootStepsComponent, FOOTSTEPS_COMPONENT>())->SetGameObject(player);
		player->GetComponent<FootStepsComponent>()->Init(player->GetComponent<Animator>());
		{
			std::string leftFootName = "L_heel";
			std::string rightFootName = "R_heel";
			if (!player->GetComponent<MeshComponent>())
			{
				return;
			}
			auto meshComp = player->GetComponent<MeshComponent>();
			if (!meshComp->GetMesh())
			{
				return;
			}
			if (!meshComp->GetMesh()->GetSkeleton().Joints.empty())
			{
				int leftJointIndex = meshComp->GetJoint(leftFootName);
				int rightJointIndex = meshComp->GetJoint(rightFootName);
				auto animator = player->GetComponent<Animator>();
				player->GetComponent<FootStepsComponent>()->SetLeftBone(&animator->GetAnimationBuffer()->JointTransforms[leftJointIndex], meshComp->GetMesh()->GetSkeleton().Joints[leftJointIndex].BindPoseInverse);
				player->GetComponent<FootStepsComponent>()->SetRightBone(&animator->GetAnimationBuffer()->JointTransforms[rightJointIndex], meshComp->GetMesh()->GetSkeleton().Joints[rightJointIndex].BindPoseInverse);
			}
		}
	}


}


void GameWorld::CheckLoadingDone()
{
	if (!MainSingleton::Get().GetSceneManager().GetIsLoading())
	{
		myHasLoadedScene = true;
		myLoadCondition.notify_all();
	}
}



