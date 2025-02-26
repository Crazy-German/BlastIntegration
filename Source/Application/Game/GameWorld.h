#pragma once
#include <memory>
#include "GraphicsEngine/GraphicsEngine.h"
#include "Interface/SplashScreen.h"

#pragma region WindowsIncludes
#define	WIN32_LEAN_AND_MEAN

#define NOGDICAPMASKS     
#define NOVIRTUALKEYCODES 
//#define NOWINMESSAGES     
//#define NOWINSTYLES       
//#define NOSYSMETRICS      
#define NOMENUS           
#define NOICONS           
#define NOKEYSTATES       
#define NOSYSCOMMANDS     
#define NORASTEROPS       
#define NOSHOWWINDOW      
#define OEMRESOURCE       
#define NOATOM            
#define NOCLIPBOARD       
#define NOCOLOR           
#define NOCTLMGR          
#define NODRAWTEXT        
#define NOGDI             
//#define NOKERNEL          
//#define NOUSER            
//#define NONLS
#define NOMB              
#define NOMEMMGR          
#define NOMETAFILE        
#define NOMINMAX          
//#define NOMSG             
#define NOOPENFILE        
#define NOSCROLL          
#define NOSERVICE         
#define NOSOUND           
#define NOTEXTMETRIC      
#define NOWH              
#define NOWINOFFSETS      
#define NOCOMM            
#define NOKANJI           
#define NOHELP            
#define NOPROFILER        
#define NODEFERWINDOWPOS  
#define NOMCX
#define YIELD_IMPLEMENTATION
#ifndef YIELD_IMPLEMENTATION
#include <condition_variable>
#include "mutex"
#endif // !YIELD_IMPLEMENTATION
#include <Windows.h>
#include "CommonUtilities/InputMapper.h"
#include <Interface/SpriteAnimation.h>

#include "Renderer.h"
#include "CommonUtilities/Timer.h"
#include "GameObjectSystem/Scenes/SceneManager.h"

#pragma endregion

class Sprite2D;
class GameObject;
class MyGui;

//extern HCURSOR globalCursorHandle;

class GameWorld : public InputObserver
{
public:
	GameWorld();
	~GameWorld();

	bool Initialize(SIZE aWindowSize, WNDPROC aWindowProcess, LPCWSTR aWindowTitle);
	int Run();
	//Start level from commandline-args
	int RunFromUnreal(const std::string& aLevel);
	static void RegisterAssets();
	void CreateScene();
	void UpdateScene(float aTimeDelta);
	void RenderScene();

	HWND GetWindowHandle();
	void GameLoop();

	void RecieveEvent(const ActionEvent& anEvent) override;
private:
	void MapInputs();
	void InitPlayer();
	void CheckLoadingDone();


private:
	std::vector<std::shared_ptr<Sprite2D>> mySprites;
	std::vector<std::shared_ptr<SpriteAnimation>> myAnimatedSprites;

	std::string myAnimations[2];
	unsigned myCurrentAnimation;

	CU::Timer* myTimer;
	bool myIsRunning;
	
	HWND myMainWindowHandle;
	HCURSOR myCursorHandle;
	std::unique_ptr<MyGui> myGui;
	std::string myCmdLineScene;

	//For toggle purposes
	bool myLockMouse = true;  

	std::atomic<bool> myHasLoadedScene = false;
	std::mutex myLoadLock;
	std::condition_variable myLoadCondition;
};
