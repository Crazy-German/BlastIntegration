#define WIN32_LEAN_AND_MEAN
#include <cstdio>
#include <exception>

#include "Application.h"

#include "GameWorld.h"
#include "Windows.h"

#include "StringHelpers.h"

#include "DearImgui/imgui.h"
#include "DearImgui/imgui_impl_win32.h"
#include "DearImgui/imgui_impl_dx11.h"

#include <External\DebugStuff\MemoryTracker.h>

#include "DearImgui/imgui_internal.h"
#include "Physcics/PhysicsEngine.h"
#include "Physcics/PhysXScene.h"
#include "Resources/resource.h"

#include <shellapi.h>

HCURSOR cursor;
HICON icon;
bool running = true;
bool spawnObject = false;
LRESULT CALLBACK WinProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

struct debugStruct
{
    debugStruct()
    {
        bool stuff = true;
        Tga::MemoryTrackingSettings settings;
        settings.myShouldStoreStackTraces = stuff;
        settings.myShouldTrackAllAllocations = false;

        Tga::StartMemoryTracking(settings);
    }
    ~debugStruct()
    {
        Tga::StopMemoryTrackingAndPrint();
    }
};


bool CheckForCommandLineArgs(std::string& inOutArg)
{
    LPWSTR* szArglist;
    int nArgs;

    szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);

    if (nArgs < 2)
    {
        return false;
    }

    LPWSTR level = GetCommandLineW();
    std::string levelStr;
    UINT codepage = CP_ACP;
    char* p = 0;
    int bsz;

    bsz = WideCharToMultiByte(codepage,
        0,
        level, -1,
        0, 0,
        0, 0);
    if (bsz > 0)
    {
        p = new char[bsz];
        int rc = WideCharToMultiByte(codepage,
            0,
            level, -1,
            p, bsz,
            0, 0);
        if (rc != 0)
        {
            p[bsz - 1] = 0;
            levelStr = p;
        }
    }
    size_t spacePos = levelStr.find(" ");
    inOutArg = std::string(levelStr.begin() + spacePos + 1, levelStr.end());
    delete[] p;
    return true;

}

bool VerifyFile(std::string_view aFile)
{
    std::filesystem::path file = aFile;
    if (aFile.empty())
    {
        return false;
    }
    if (!std::filesystem::exists(aFile))
    {
        return false;
    }
    if (!file.has_extension() || !file.has_filename())
    {
        return false;
    }
    return true;
}

int GuardedMain(HINSTANCE aHInstance)
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

    /*
     * A note about strings:
     * Strings in C++ are terrible things. They come in a variety of formats which causes issues.
     * Many modern libraries expect UTF-8 (ImGui, FMOD, FBX SDK, etc) but Windows defaults to UTF-16LE
     * which is not compatible 1=1.
     *
     * To avoid weird problems with compatibility, mangled characters and other problems it is highly
     * recommended to store everything as UTF-8 encoded strings whenever possible. This means that
     * when we store i.e. the name of a Model, Level, Sound File, etc we do so in a UTF-8 formatted
     * std::string, and when we need to communicate with the Windows (or DirectX) API we need to use
     * std::wstring which represents a UTF-16LE string.
     *
     * There are functions available for conversion between these formats in the str namespace in
     * StringHelpers.h.
     *
     * The provided Logging library expects UTF-8 format strings which should provide minimal headaches
     * for any involved situation. For anything non-unicode (like non swedish signs, accents, etc) you
     * can just use normal strings as you would anywhere since UTF-8 is backwards compatible with ASCii
     *
     * SetConsoleOutputCP(CP_UTF8) tells the Windows Console that we'll output UTF-8. This DOES NOT
     * affect file output in any way, that's a whole other can of worms. But if you always write and
     * read your strings in the same format, and always treat them as byte blocks, you'll be fine.
     */

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
    constexpr LPCWSTR windowTitle = L"FM HPE 102.4";

   // constexpr LPCWSTR windowClassName = L"ModelViewerMainWindow";

	// First we create our Window Class
	//WNDCLASS windowClass = {};
	//windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	//windowClass.lpfnWndProc = WinProc;
	//windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//windowClass.lpszClassName = windowClassName;
	//RegisterClass(&windowClass);

	//LONG posX = (GetSystemMetrics(SM_CXSCREEN) - windowSize.cx) / 2;
	//if (posX < 0)
	//	posX = 0;

	//LONG posY = (GetSystemMetrics(SM_CYSCREEN) - windowSize.cy) / 2;
	//if (posY < 0)
	//	posY = 0;

	//// Then we use the class to create our window
	//auto handle = CreateWindow(
	//	windowClassName,                                // Classname
	//	windowTitle,                                   // Window Title
	//	WS_POPUP | WS_VISIBLE,                 // Flags
	//	posX,
	//	posY,
	//	windowSize.cx,
	//	windowSize.cy,
	//	nullptr, nullptr, nullptr,
	//	nullptr
	//);
 //   ShowWindow(handle, SW_SHOW);
	//SetForegroundWindow(handle);

 //   MSG msg;
	//ZeroMemory(&msg, sizeof(MSG));

    //Squish::PhysXScene myScene= *Squish::PhysicsEngine::Get()->CreateScene();
    GameWorld gameWorld;
    gameWorld.Initialize(windowSize, WinProc, windowTitle);

    cursor = LoadCursorFromFile(L"spite_cursor.cur");

    icon = LoadIcon(aHInstance, MAKEINTRESOURCE(IDI_ICON1));
    //these have to be here, windows is too stupid to set the program icon otherwise
    /*SendMessage(gameWorld.GetWindowHandle(), WM_SETICON, ICON_SMALL, (LPARAM)icon);
    SendMessage(gameWorld.GetWindowHandle(), WM_SETICON, ICON_BIG, (LPARAM)icon);*/



    std::string levelArg;
    bool hasLevelArgs = CheckForCommandLineArgs(levelArg);
    hasLevelArgs = VerifyFile(levelArg);

    std::thread updateThread(&GameWorld::GameLoop, &gameWorld);
    //This is the option to start from commandlineargs!
    if (hasLevelArgs)
    {
        //Remove
         gameWorld.RunFromUnreal(levelArg);
    }
    else
    {
        gameWorld.Run();
    }
    updateThread.join();
    return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);
#ifdef _DEBUG
    //std::shared_ptr<debugStruct> ah = std::make_shared<debugStruct>();
    return GuardedMain(hInstance);
#endif


    
    try
	{
        if (GuardedMain(hInstance) == 0)
        {
            //ah.~shared_ptr();
            return 0;
        }
	}
    catch(const std::exception& e)
    {
        std::string message = e.what();
        if(!str::is_valid_utf8(message))
        {
	        message = str::acp_to_utf8(message);
        }
        MVLOG(Error, "Exception caught!\n{}", message);
        return -1;
    }
    return 0;
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
            SetCursor(cursor);
            return true;
        }
    }
    
    CommonUtilities::Input::BuiltInWinProc(hWnd, uMsg, wParam, lParam);
    if (uMsg == WM_DESTROY || uMsg == WM_CLOSE)
    {
        PostQuitMessage(0);
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

