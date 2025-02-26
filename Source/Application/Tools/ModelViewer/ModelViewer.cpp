#include "ModelViewer.h"
#include <filesystem>
#include <future>
#include <memory>
#include "wrl.h"
#include "GraphicsEngine/GraphicsCommands.h"
#include "AssetManagement/AssetManager.h"
#include "CommonUtilities/Input.h"
#include "CommonUtilities/Timer.h"
#include "TGAFbx/include/TGAFbx.h"
#include <Windows.h>
#include "shellapi.h"
#include "WinUser.h"
#include "ModelViewerGui.h"
#include "MainSingleton.h"
#include "GameObjectSystem/Scenes/SceneManager.h"

#if _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(LogModelViewer, ModelViewer, Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(LogModelViewer, GameWorld, Warning);
#endif

#define MVLOG(Verbosity, Message, ...) LOG(LogModelViewer, Verbosity, Message, ##__VA_ARGS__)


ModelViewer::ModelViewer()
{
	myGui = nullptr;
	myIsRunning = false;
	myMainWindowHandle = nullptr;
}
ModelViewer::~ModelViewer() = default;

bool ModelViewer::Initialize(SIZE aWindowSize, WNDPROC aWindowProcess, LPCWSTR aWindowTitle)
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
		WS_OVERLAPPEDWINDOW | WS_POPUP,                 // Flags
		posX,
		posY,
		aWindowSize.cx,
		aWindowSize.cy,
		nullptr, nullptr, nullptr,
		nullptr
	);

	{   
		MVLOG(Log, "Initializing Graphics Engine...");

		if (!GraphicsEngine::Get().Initialize(myMainWindowHandle, aWindowSize.cx, aWindowSize.cy))
		{
			return false;
		}
	}
#if RUN_MODEL_VIEWER
	{
		GraphicsEngine& graphicsEngine = GraphicsEngine::Get();
		myGui = new ModelViewerGui();
		myGui->Initialize(myMainWindowHandle, graphicsEngine.GetDevice().Get(), graphicsEngine.GetContext().Get(), this);
		DragAcceptFiles(myMainWindowHandle, true);
	}
#endif
	MVLOG(Log, "Ready!");


	auto startDir = std::filesystem::current_path().u8string();
	startDir = startDir.substr(0, startDir.find_last_of(u8"Bin\\"));
	startDir.append(u8"\\Assets");
	const std::filesystem::path contentDir = startDir;
	AssetManager::Get().Initialize(contentDir);

	auto& sceneManager = MainSingleton::Get().GetSceneManager();
	sceneManager.Awake();
	sceneManager.Init();
	sceneManager.CreateModelViewerScene();

	ShowWindow(myMainWindowHandle, SW_SHOW);
	SetForegroundWindow(myMainWindowHandle);
	return true;
}

int ModelViewer::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	{
		auto startDir = std::filesystem::current_path().u8string();
		startDir = startDir.substr(0, startDir.find_last_of(u8"Bin\\"));
		startDir.append(u8"\\Assets");
		const std::filesystem::path contentDir = startDir;
		AssetManager::Get().Initialize(contentDir);
		TGA::FBX::Importer::InitImporter();
	}
	AssetManager::Get().RegisterAsset("Materials/M_Default.json");

	CU::Timer timer;

	//Somehow create new scene

	MainSingleton::Get().GetSceneManager().Init();
	myIsRunning = true;

	while (myIsRunning)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				myIsRunning = false;
			}
#if RUN_MODEL_VIEWER
			if (msg.message == WM_DROPFILES)
			{
				char fileName[MAX_PATH];

				for (unsigned i = 0; i < DragQueryFileA(reinterpret_cast<HDROP>(msg.wParam), i, fileName, MAX_PATH); ++i)
				{
					myGui->LoadAsset(fileName);
				}
			}
#endif
			//Don't know what to do here, so I'm freestyling
			//MainSingleton::Get().GetInputMapper().UpdateEvents(msg.message, msg.wParam, msg.lParam);
			MainSingleton::Get().GetInputMapper().Refresh();
		}
		//Same here
		//CU::InputHandler::Get().UpdateInput();
		CU::Input::Update();
		timer.Update();

		GraphicsCommandList* commandList = &GraphicsCommandList::Get();
		commandList->Reset();
		Update(timer.GetDeltaTime());
		Render();
	}
	
	return 0;
}

void ModelViewer::Update(float aTimeDelta)
{
#if RUN_MODEL_VIEWER
	myGui->Update();
#endif
	MainSingleton::Get().Update(aTimeDelta);
}

void ModelViewer::Render()
{
	GraphicsCommandList* commandList = &GraphicsCommandList::Get();

	MainSingleton::Get().GetSceneManager().RenderScene();
#if RUN_MODEL_VIEWER
	myGui->Render();
#endif
	
	//commandList->Enqueue<GfxCmd_Present>();
	if (commandList->HasCommands() && !commandList->IsFinished())
	{
		commandList->Execute();
	}
	
}


