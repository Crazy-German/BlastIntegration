#define WIN32_LEAN_AND_MEAN
#define IMGUI_DEFINE_MATH_OPERATORS
#include <cstdio>
#include <exception>

#include "Application.h"

#include "Windows.h"

#include "StringHelpers.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"


#include <shellapi.h>
#include <algorithm>
#include "../../FeatherEngine/EngineSettings.h"
#include "CommonUtilities/Input.h"
#include "CommonUtilities/Timer.h"
#include "DebugStuff/MemoryTracker.h"
#include "GraphicsEngine/FrameBuffer.h"
#include "GraphicsEngine/GraphicsEngine.h"
#include "Logger/Logger.h"

#include "MuninGraph.h"
#include "TimerManager.h"
//#include "GameObjectSystem/Components/ScriptComponent.h"
#include "ScriptGraphNodes/ScriptGraphTypes.h"
#include "ScriptGraphNodes/ScriptGraphNodes.h"
#include "MuninGraph/ScriptGraph/ScriptGraph.h"
#include "MuninGraph/ScriptGraph/ScriptGraphSchema.h"
#include "MuninGraphEditor/ScriptGraphEditor/ScriptGraphEditor.h"
#include "ScriptGraphNodes/Events/SGNode_BeginPlay.h"

#include "GraphicsEngine/GfxCmd_Bloom.h"
#include "GraphicsEngine/GfxCmd_ChangePipelineState.h"
#include "GraphicsEngine/GfxCmd_ClearBackBuffer.h"
#include "GraphicsEngine/GfxCmd_DrawQuad.h"
#include "GraphicsEngine/GfxCmd_Present.h"
#include "GraphicsEngine/GraphicsCommandList.h"

int GuardedMain();

int main(const int /*argc*/, const char* /*argc*/[])
{
	return GuardedMain();
}
LRESULT CALLBACK WinProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
#ifndef _RETAIL
    extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;
#endif
    if (uMsg == WM_SETCURSOR)
    {
        if (LOWORD(lParam) == HTCLIENT)
        {
            return true;
        }
    }
    
    CommonUtilities::Input::HandleEvents(uMsg, wParam, lParam);

    if (uMsg == WM_DESTROY || uMsg == WM_CLOSE)
    {
        PostQuitMessage(0);
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int GuardedMain()
{
    // Redirect stdout and stderr to the console.


#ifndef _RETAIL
    FILE* consoleOut;
	FILE* consoleErr;
    AllocConsole();

    freopen_s(&consoleOut, "CONOUT$", "w", stdout);  // NOLINT(cert-err33-c)
    if (consoleOut != nullptr)
    {
        setvbuf(consoleOut, nullptr, _IONBF, 1024);  // NOLINT(cert-err33-c)
    }

	freopen_s(&consoleErr, "CONOUT$", "w", stderr);  // NOLINT(cert-err33-c)
    if (consoleErr != nullptr)
    {
        setvbuf(consoleErr, nullptr, _IONBF, 1024);  // NOLINT(cert-err33-c)
    }

    SetConsoleOutputCP(CP_UTF8);
    MVLOG(Log, "ModelViewer starting...");

	const HWND consoleWindow = GetConsoleWindow();
    RECT consoleSize;
    GetWindowRect(consoleWindow, &consoleSize);
    MoveWindow(consoleWindow, consoleSize.left, consoleSize.top, 1280, 720, true);
#endif

    RECT clientSize;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &clientSize);

    const LONG width = clientSize.right;
    const LONG height = clientSize.bottom;

    SIZE windowSize = { width, height};
    constexpr LPCWSTR windowTitle = L"The Last Hope";

    WNDCLASS windowClass = {};
	windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = WinProc;
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.lpszClassName = windowTitle;
	RegisterClass(&windowClass);

	LONG posX = (GetSystemMetrics(SM_CXSCREEN) - windowSize.cx) / 2;
	if (posX < 0)
		posX = 0;

	LONG posY = (GetSystemMetrics(SM_CYSCREEN) - windowSize.cy) / 2;
	if (posY < 0)
		posY = 0;

	// Then we use the class to create our window
	auto myMainWindowHandle = CreateWindow(
		windowTitle,                                // Classname
		windowTitle,                                   // Window Title
		WS_POPUP | WS_VISIBLE,                 // Flags
		posX,
		posY,
		windowSize.cx,
		windowSize.cy,
		nullptr, nullptr, nullptr,
		nullptr
	);
    EngineSettings::Get().Initialize(myMainWindowHandle);
    GraphicsEngine::Get().Initialize(myMainWindowHandle, EngineSettings::Get().GetResolution().width, EngineSettings::Get().GetResolution().height);

    FrameBuffer buffer;
	buffer.Resolution = CU::Vector2f(static_cast<float>(GraphicsEngine::Get().GetViewPort().Width), static_cast<float>(GraphicsEngine::Get().GetViewPort().Height));
	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::FrameBuffer, buffer);

    ImGui::CreateContext();
    ImGui_ImplWin32_Init(myMainWindowHandle);
    ImGui_ImplDX11_Init(GraphicsEngine::Get().GetDevice().Get(), GraphicsEngine::Get().GetContext().Get());

    assert(MuninGraph::Get().Initialize());
    GameObject* object = new GameObject();
    object->AddComponent(ComponentSystem::Get()->AddComponent<Transform, TRANSFORM>());
    //object->AddComponent(ComponentSystem::Get()->AddComponent<ScriptComponent, SCRIPT_COMPONENT>);
    ScriptGraph* myGraph = new ScriptGraph(object);
    ScriptGraphSchema mySchema(myGraph);
    ScriptGraphEditorSettings* myEditorSettings = new ScriptGraphEditorSettings(GraphicsEngine::Get().GetDevice());
    ScriptGraphEditorState myState;
	ScriptGraphEditor* myEditor = new ScriptGraphEditor(myEditorSettings, &myState, myGraph);
    //object->GetComponent<ScriptComponent>()->SetScript(myGraph);


	ShowWindow(myMainWindowHandle, SW_SHOW);
	SetForegroundWindow(myMainWindowHandle);
    //these have to be here, windows is too stupid to set the program icon otherwise
    /*SendMessage(gameWorld.GetWindowHandle(), WM_SETICON, ICON_SMALL, (LPARAM)icon);
    SendMessage(gameWorld.GetWindowHandle(), WM_SETICON, ICON_BIG, (LPARAM)icon);*/
    GraphicsCommandList myCommand;
    //GraphicsEngine::Get().ChangeRenderTarget()
	MSG msg;
    while (true)
    {
	    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	    {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
	    }

		TimerManager::Get()->Update(CommonUtilities::Timer::Get().GetDeltaTime());
    	myEditor->Update(CommonUtilities::Timer::Get().GetDeltaTime());
        object->Update(CommonUtilities::Timer::Get().GetDeltaTime());
		CommonUtilities::Timer::Get().Update();

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        myCommand.Enqueue<GfxCmd_ClearBackBuffer>();
        myCommand.Enqueue<GfxCmd_ChangePipelineState>(GraphicsEngine::Get().GetBloomPSO());
        myCommand.Enqueue<GfxCmd_DrawQuad>();

        myCommand.Execute();
        if(myCommand.IsFinished())
        {
			myCommand.Reset();
        }

        myEditor->Render();
		ImGui::ShowDemoWindow();

        ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		myCommand.Enqueue<GfxCmd_Present>();

        myCommand.Execute();
        if(myCommand.IsFinished())
        {
			myCommand.Reset();
        }
    }
    ImGui::Shutdown();

    
    return 0;
}