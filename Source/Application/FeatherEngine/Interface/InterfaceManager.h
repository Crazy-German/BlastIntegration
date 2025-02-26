#pragma once
#include "GraphicsEngine/Sprite2D.h"
#include <array>
#include "SpriteAnimation.h"
#include "SplashScreen.h"
#include <unordered_map>
#include "InterfaceMenu.h"
#include "../GameObjectSystem/PostMaster/PostBox.h"
#include "SpriteInstance.h"
#include "TapePlayer.h"
#include "Button2D.h"

class Subtitles;
class VideoTexture;

class InterfaceManager : public PostBox
{
public:
	InterfaceManager();
	~InterfaceManager();

	void Put(Letter aLetter) override;

	static InterfaceManager* Get();

	void Initialize(bool aSkipMainMenu = false);
	void Update(float aDeltaTime);

	void RescaleAllMenus();

	void ChangeCurrentMenu(const std::string& aMenuName);
	void ChangeToMainMenu();
	void ChangeToInGame();

	const bool IsInMenu() const;
	const bool ShouldRenderScene() const;

	const std::string& GetCurrentMenu() const;

	void StopSplashScreen();

	//Returns false if no mouse click was made on a sprite contained in the current menu
	bool Input(CU::Vector2f aMousePosition);

	void Draw();
	void DrawTooltip(std::shared_ptr<Tooltip> aTooltip);

	std::array<std::shared_ptr<Button2D>, 4> GetInventoryContent();

	//P7 unique (I think) / Viktor

	bool GetLockMouse() const;

private:
	//p7 unique
	
	std::shared_ptr<TapePlayer> myTapePlayer;
	std::array<std::shared_ptr<Button2D>, 4> myInventoryContent;
	int myInventoryOffset = 0;


	//


	bool myHasHadTooltipOneFrame = false;
	bool myRescaleNextFrame = false;
	bool myRescaleNextFrameFr = false;
	std::string myCurrentMenuString;
	static InterfaceManager* myInstance;
	std::shared_ptr<SplashScreen> mySplashScreen;
	bool myRunSplashScreen = false;

	bool skibidiVignette = true;

	bool myLoadingIsFinished = false;

	float myCurrentVignetteRedness;

	unsigned myFirstFrameTicks = 0;
	bool myHasTickedFirstFrameOfLoading = false;

	std::shared_ptr<InterfaceMenu> myCurrentMenu;
	std::shared_ptr<InterfaceMenu> myHUD;

	std::shared_ptr<VideoTexture> myVideo;

	std::unordered_map<std::string, std::shared_ptr<InterfaceMenu>> myMenuMap;

	std::shared_ptr<Tooltip> myCurrentTooltip;
	std::shared_ptr<SpriteInstance> myVignette;

	void KeyInputs();

	void InitHud();
	void InitMainMenu();
	void InitPauseMenu();
	void InitSettingsMenu();
	void InitCreditsMenu();
	void InitLevelSelectMenu();
	void InitLoadingScreen();
	void InitCutscene();
	void RescaleAllMenusInternal();

	void InitTapeInventory();

	std::string mySettingsReturnPoint;

};