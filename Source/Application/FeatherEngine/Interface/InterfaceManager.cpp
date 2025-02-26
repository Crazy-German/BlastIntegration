#include "FeatherEngine.pch.h"
#include "InterfaceManager.h"
#include <GraphicsEngine/GraphicsCommandList.h>
#include <GraphicsEngine/GfxCmd_BeginEvent.h>
#include <GraphicsEngine/GfxCmd_ChangePipelineState.h>
#include <GraphicsEngine/GfxCmd_RenderInterface.h>
#include <GraphicsEngine/GfxCmd_EndEvent.h>
#include <GraphicsEngine/GraphicsEngine.h>
#include <AssetManagement/AssetManager.h>
#include "Button2D.h"
#include "EngineDefines.h"
#include <GraphicsEngine/GfxCmd_RenderText.h>
#include <GraphicsEngine/Text/TextFactory.h>
#include <AssetManagement/AssetTypes/FontAsset.h>
#include "SpriteInstance.h"
#include "TextComponent.h"
#include <CommonUtilities/Input.h>
#include <CommonUtilities/Color.h>
#include <GameObjectSystem/PostMaster/GameObjectPostMaster.h>
#include <GraphicsEngine/MiscBuffer.h>
#include <GraphicsEngine/GfxCmd_SetMiscBuffer.h>
#include <GraphicsEngine/GraphicsCommandList.h>
#include "Tooltip.h"
#include "GraphicsEngine/GfxCmd_SetShaders.h"
#include "Slider.h"
#include "EngineSettings.h"
#include "OptionsList.h"
#include <MainSingleton.h>
#include "LoadingScreen.h"
#include <GameObjectSystem/Components/MVCameraComponent.h>
#include "VideoComponent.h"
#include <GraphicsEngine/Video/Video.h>
#include "Checkbox.h"
#include "Subtitles.h"

InterfaceManager* InterfaceManager::myInstance;

InterfaceManager::InterfaceManager()
{

}

InterfaceManager::~InterfaceManager()
{
	VideoTexture::DestroyAPI();
}

void InterfaceManager::Put(Letter aLetter)
{
	switch (aLetter.myType)
	{
		case eLetterRegister::StartGame:
		{
			myLoadingIsFinished = false;
#ifndef _RETAIL
			SceneManager& sceneManager = MainSingleton::Get().GetSceneManager();
			std::filesystem::path current = std::filesystem::current_path();
			std::filesystem::path pontusPath = "Assets\\Exported Levels\\l_level1\\L_Level1_Level.json";
			std::filesystem::path eliasPath = "Assets\\Exported Levels\\l_level2\\L_Level2_Level.json";
			std::filesystem::path annaPath = "Assets\\Exported Levels\\l_level3\\L_Level3_Level.json";

			std::filesystem::path gymPlayerPath = "Assets\\Exported Levels\\l_playergym\\L_PlayerGym_Level.json";
			std::filesystem::path gymAssetPath = "Assets\\Exported Levels\\l_assetgym\\L_AssetGym_Level.json";
			std::filesystem::path gymAnimationPath = "Assets\\Exported Levels\\l_animationgym\\L_AnimationGym_Level.json";
			std::filesystem::path gymVFXPath = "Assets\\Exported Levels\\l_vfxgym\\L_VFXGym_Level.json";

			std::string filePath = "settings\\LevelSelection.json";
			nlohmann::json jsonReader;
			std::fstream file(filePath);

			if (!file.good())
			{

				std::cout << "Unable to load levelselection json file!";
				return;
			}
			file >> jsonReader;
			int level = jsonReader.at("Level");
			assert(level < 5);
			switch (level)
			{
			case 1:
				sceneManager.LoadSceneNextFrame(pontusPath);
				break;
			case 2:
				sceneManager.LoadSceneNextFrame(eliasPath);
				break;
			case 3:
				sceneManager.LoadSceneNextFrame(annaPath);
				break;
			case 4:
				sceneManager.LoadSceneNextFrame(gymPlayerPath);
				break;
			case 5:
				sceneManager.LoadSceneNextFrame(gymAssetPath);
				break;
			case 6:
				sceneManager.LoadSceneNextFrame(gymAnimationPath);
				break;
			case 7:
				sceneManager.LoadSceneNextFrame(gymVFXPath);
				break;
			default:
				break;
			}
#else
			MainSingleton::Get().GetSceneManager().LoadSceneNextFrame(std::filesystem::path("Assets\\Exported Levels\\l_level1\\L_Level1_Level.json"));
#endif
			Letter levelLetter;
			levelLetter.myType = eLetterRegister::LoadingLevel;
			GameObjectPostMaster::Get()->Send(levelLetter);
			ChangeCurrentMenu("loading_screen");
			myHasTickedFirstFrameOfLoading = false;
		}
		break;
		case eLetterRegister::GoToMainMenu:
		{
			ChangeToMainMenu();
			//MainSingleton::Get().GetSceneManager().GetGlobalObjects().Player->GetComponent<PlayerComponent>()->LockAbilities();

			Letter healthLetter;
			healthLetter.myType = eLetterRegister::BossActivated;
			bool active = false;
			std::wstring name = L"Pathos";
			std::pair<bool, std::wstring> bossActive;
			bossActive.first = active;
			bossActive.second = name;
			healthLetter.myData = &bossActive;
			GameObjectPostMaster::Get()->Send(healthLetter);
		}
		break;
		case eLetterRegister::BackToGame:
		{
			ChangeToInGame();
		}
		break;
		case eLetterRegister::GoToLevelSelect:
		{
			ChangeCurrentMenu("level_menu");
		}
		break;
		case eLetterRegister::GoToSettingsMenu:
		{
			ChangeCurrentMenu("settings_menu");
		}
		break;
		case eLetterRegister::GoToCreditsMenu:
		{
			ChangeCurrentMenu("credits_menu");
		}
		break;
		case eLetterRegister::ReturnFromSettings:
		{
			ChangeCurrentMenu(mySettingsReturnPoint);
		}
		break;
		case eLetterRegister::SetResolution:
		{
			//RescaleAllMenus();
			RescaleAllMenusInternal();
		}
		break;
		//now this is evil
		case eLetterRegister::LoadLevel:
			myLoadingIsFinished = false;

			if (1 == *static_cast<unsigned*>(aLetter.myData))
			{
				MainSingleton::Get().GetSceneManager().LoadSceneNextFrame(std::filesystem::path("Assets\\Exported Levels\\l_level1\\L_Level1_Level.json"));
			}
			if (2 == *static_cast<unsigned*>(aLetter.myData))
			{
				MainSingleton::Get().GetSceneManager().LoadSceneNextFrame(std::filesystem::path("Assets\\Exported Levels\\l_level2\\L_Level2_Level.json"));
			}
			if (3 == *static_cast<unsigned*>(aLetter.myData))
			{
				MainSingleton::Get().GetSceneManager().LoadSceneNextFrame(std::filesystem::path("Assets\\Exported Levels\\l_level3\\L_Level3_Level.json"));
			}
			if (4 == *static_cast<unsigned*>(aLetter.myData))
			{
				MainSingleton::Get().GetSceneManager().LoadSceneNextFrame(std::filesystem::path("Assets\\Exported Levels\\l_playergym\\L_PlayerGym_Level.json"));
			}
			if (5 == *static_cast<unsigned*>(aLetter.myData))
			{
				MainSingleton::Get().GetSceneManager().LoadSceneNextFrame(std::filesystem::path("Assets\\Exported Levels\\l_assetgym\\L_AssetGym_Level.json"));
			}
			if (6 == *static_cast<unsigned*>(aLetter.myData))
			{
				MainSingleton::Get().GetSceneManager().LoadSceneNextFrame(std::filesystem::path("Assets\\Exported Levels\\l_animationgym\\L_AnimationGym_Level.json"));
			}
			if (7 == *static_cast<unsigned*>(aLetter.myData))
			{
				MainSingleton::Get().GetSceneManager().LoadSceneNextFrame(std::filesystem::path("Assets\\Exported Levels\\l_vfxgym\\L_VFXGym_Level.json"));
			}

			Letter levelLetter;
			levelLetter.myType = eLetterRegister::LoadingLevel;
			GameObjectPostMaster::Get()->Send(levelLetter);
			ChangeCurrentMenu("loading_screen");
			myHasTickedFirstFrameOfLoading = false;

			break;
		case eLetterRegister::LoadingLevel:

			ChangeCurrentMenu("loading_screen");
			myHasTickedFirstFrameOfLoading = false;
			break;
		case eLetterRegister::LoadingDone:

			myLoadingIsFinished = true;
			break;
		case eLetterRegister::LoadFadeDone:
			ChangeToInGame();
			MainSingleton::Get().GetSceneManager().GetGlobalObjects().Player->SetEnabled(true);
			break;
		case eLetterRegister::PlayVideo:
			break;
		case eLetterRegister::VideoOver:
			break;
		case eLetterRegister::TapeStart:
		{
			std::string letterData = *static_cast<std::string*>(aLetter.myData);
			myTapePlayer->PlaySubtitles(letterData);
		}
			
			break;
		case eLetterRegister::InventoryScroll:
			myInventoryOffset += *static_cast<int*>(aLetter.myData);
			if (myInventoryOffset < 0)
			{
				myInventoryOffset = 0;
			}
			else if (myInventoryOffset >= myTapePlayer->GetInventorySize())
			{
				myInventoryOffset = max(0,myTapePlayer->GetInventorySize() - 1);
			}
			break;
	}
}

InterfaceManager* InterfaceManager::Get()
{
	if (myInstance != nullptr)
	{
		return myInstance;
	}
	else
	{
		myInstance = new InterfaceManager();
		return myInstance;
	}
}

void InterfaceManager::Initialize(bool aSkipMainMenu)
{	
	VideoTexture::CreateAPI();
	float resScale = EngineSettings::Get().GetResolutionScale();
	mySettingsReturnPoint = "main_menu";
	GameObjectPostMaster::Get()->Register(eLetterRegister::StartGame, this);
	GameObjectPostMaster::Get()->Register(eLetterRegister::BackToGame, this);
	GameObjectPostMaster::Get()->Register(eLetterRegister::GoToMainMenu, this);
	GameObjectPostMaster::Get()->Register(eLetterRegister::GoToCreditsMenu, this);
	GameObjectPostMaster::Get()->Register(eLetterRegister::GoToLevelSelect, this);
	GameObjectPostMaster::Get()->Register(eLetterRegister::GoToSettingsMenu, this);
	GameObjectPostMaster::Get()->Register(eLetterRegister::ReturnFromSettings, this);
	GameObjectPostMaster::Get()->Register(eLetterRegister::SetResolution, this);
	GameObjectPostMaster::Get()->Register(eLetterRegister::LoadLevel, this);
	GameObjectPostMaster::Get()->Register(eLetterRegister::LoadingLevel, this);
	GameObjectPostMaster::Get()->Register(eLetterRegister::LoadingDone, this);
	GameObjectPostMaster::Get()->Register(eLetterRegister::LoadFadeDone, this);
	GameObjectPostMaster::Get()->Register(eLetterRegister::PlayVideo, this);
	GameObjectPostMaster::Get()->Register(eLetterRegister::VideoOver, this);
	GameObjectPostMaster::Get()->Register(eLetterRegister::TapeStart, this);
	GameObjectPostMaster::Get()->Register(eLetterRegister::InventoryScroll, this);

	myVignette = std::make_shared<SpriteInstance>();
	myVignette->InitRelative(0.5f, 0.5f, 2048 * resScale, 2048 * resScale, AssetManager::Get().GetAsset<SpriteAsset>("sprite_vignette"));

#ifdef _RETAIL
	myRunSplashScreen = true;
#endif
	mySplashScreen = std::make_shared<SplashScreen>();

	mySplashScreen->Initialize();

	ChangeToInGame();

	InitMainMenu();
	InitHud();
	InitPauseMenu();
	InitSettingsMenu();
	InitCreditsMenu();
	InitLevelSelectMenu();
	InitLoadingScreen();
	InitCutscene();
	InitTapeInventory();

	myCurrentVignetteRedness = 0.0f;

	
	if (aSkipMainMenu)
	{

	}
	else
	{
		ChangeToMainMenu();
	}
}

void InterfaceManager::Update(float aDeltaTime)
{

	myTapePlayer->Update(aDeltaTime);
	myTapePlayer->SetInventoryContent(myInventoryContent, myInventoryOffset);

	{
		float johnHealth = MainSingleton::Get().GetMiscBuffer()->healthLevel;
		float vignetteRed = 0.0f;
		if (johnHealth < 0.5f)
		{
			vignetteRed = 0.5f - johnHealth;
		}
		myCurrentVignetteRedness = std::lerp(myCurrentVignetteRedness, vignetteRed, aDeltaTime);
		myVignette->SetColor(myCurrentVignetteRedness, 0.0f, 0.0f);
	}

	if (myRescaleNextFrameFr)
	{
		myRescaleNextFrameFr = false;
		RescaleAllMenusInternal();
		ChangeCurrentMenu(myCurrentMenuString);
	}
	if (myRescaleNextFrame)
	{
		myRescaleNextFrame = false;
		myRescaleNextFrameFr = true;
	}
	auto mousePoint = CU::Input::GetMouseAbsolutePosition();
	CU::Vector2f mousepos = { static_cast<float>(mousePoint.x), static_cast<float>(mousePoint.y) };

	KeyInputs();

	if (myCurrentMenu)
	{
		if (!myHasTickedFirstFrameOfLoading && myCurrentMenu->GetName() == "loading_screen")
		{
			myHasTickedFirstFrameOfLoading = true;
		}
	}
	

	if (myCurrentTooltip && !myHasHadTooltipOneFrame)
	{
		myHasHadTooltipOneFrame = true;
	}
	else if (myCurrentTooltip)
	{
		myCurrentTooltip = nullptr;
		myHasHadTooltipOneFrame = false;
	}

	if (myRunSplashScreen)
	{
		if (mySplashScreen != nullptr)
		{
			if (!mySplashScreen->Run(aDeltaTime))
			{
				mySplashScreen = nullptr;
				myRunSplashScreen = false;
			}
		}
	}
	else
	{
		if (myCurrentMenu)
		{
			myCurrentMenu->Update(aDeltaTime);
			
		}
		else 
		{
			
		}
		myHUD->Update(aDeltaTime);
	}
	
	
}

void InterfaceManager::RescaleAllMenus()
{
	myRescaleNextFrame = true;
	myCurrentMenuString = myCurrentMenu->GetName();
}

void InterfaceManager::RescaleAllMenusInternal()
{
	myCurrentMenuString = myCurrentMenu->GetName();
	float resScale = EngineSettings::Get().GetResolutionScale();
	myVignette = std::make_shared<SpriteInstance>();
	myVignette->InitRelative(0.5f, 0.5f, 2048 * resScale, 2048 * resScale, AssetManager::Get().GetAsset<SpriteAsset>("sprite_vignette"));


	myCurrentMenu = nullptr;

	InitMainMenu();
	InitHud();
	InitPauseMenu();
	InitSettingsMenu();
	InitCreditsMenu();
	InitLevelSelectMenu();
	InitLoadingScreen();
	InitCutscene();
	InitTapeInventory();

	ChangeCurrentMenu(myCurrentMenuString);
}

void InterfaceManager::InitTapeInventory()
{
	float resScale = EngineSettings::Get().GetResolutionScale();
	std::shared_ptr<InterfaceMenu> inventoryMenu = std::make_shared<InterfaceMenu>();
	inventoryMenu->SetName("inventory_menu");
	myMenuMap[inventoryMenu->GetName()] = inventoryMenu;

	SpriteInstance background;
	background.InitRelative({ 0.5f, 0.5f }, { 2048 * resScale, 2048 * resScale }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_void"), SAnchorPoint::Center);
	background.SetAlpha(0.25f);

	inventoryMenu->AddInterfaceComponent(std::make_shared<SpriteInstance>(background));

	SpriteInstance inventorybg;
	inventorybg.InitRelative({ 0.5f, 0.5f }, { 512 * resScale, 1024 * resScale }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_inventory_background"), SAnchorPoint::Center);
	inventoryMenu->AddInterfaceComponent(std::make_shared<SpriteInstance>(inventorybg));

	TextComponent pauseText;
	pauseText.InitRelative(0.5f, 0.2f, 1.5f * resScale, AssetManager::Get().GetAsset<FontAsset>("F_cascadia-mono")->myFont, std::wstring(L"TAPES"));
	pauseText.SetColor(CU::BLACK);

	inventoryMenu->AddInterfaceComponent(std::make_shared<TextComponent>(pauseText));

	//okay so here's the trick:
	//make buttons 1-4 member variables as well
	//add a function to be able to set event data values to play different tapes
	//add events for scroll inventory up/down and set to the up and down buttons
	//figure out a way to store tapes in order in tape player class

	float buttonOffset = 0.075f;
	Button2D buttonUp;
	buttonUp.InitRelative({ 0.5f,0.3f }, { 512 * resScale, 64 * resScale }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_button"));
	buttonUp.SetText(std::wstring(L" "));
	buttonUp.SetEventType(eLetterRegister::InventoryScroll);
	buttonUp.SetEventData(new int(-1));

	myInventoryContent[0] = std::make_shared<Button2D>();
	myInventoryContent[0]->InitRelative({ 0.5f,0.3f + buttonOffset * 1.0f }, { 512 * resScale, 64 * resScale }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_button"));
	myInventoryContent[0]->SetText(std::wstring(L"Tape 1"));
	myInventoryContent[0]->SetEventType(eLetterRegister::TapeStart);
	myInventoryContent[0]->SetEventData(new std::string("MedicMain1_VOICE"));

	myInventoryContent[1] = std::make_shared<Button2D>();
	myInventoryContent[1]->InitRelative({ 0.5f,0.3f + buttonOffset * 2.0f }, { 512 * resScale, 64 * resScale }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_button"));
	myInventoryContent[1]->SetText(std::wstring(L"Tape 2"));
	myInventoryContent[1]->SetEventType(eLetterRegister::TapeStart);
	myInventoryContent[1]->SetEventData(new std::string("LocalMain2_VOICE"));

	myInventoryContent[2] = std::make_shared<Button2D>();
	myInventoryContent[2]->InitRelative({ 0.5f,0.3f + buttonOffset * 3.0f }, { 512 * resScale, 64 * resScale }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_button"));
	myInventoryContent[2]->SetText(std::wstring(L"Tape 3"));
	myInventoryContent[2]->SetEventType(eLetterRegister::TapeStart);
	myInventoryContent[2]->SetEventData(new std::string("LocalMain3_VOICE"));

	myInventoryContent[3] = std::make_shared<Button2D>();
	myInventoryContent[3]->InitRelative({ 0.5f,0.3f + buttonOffset * 4.0f }, { 512 * resScale, 64 * resScale }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_button"));
	myInventoryContent[3]->SetText(std::wstring(L"Tape 4"));
	myInventoryContent[3]->SetEventType(eLetterRegister::TapeStart);
	myInventoryContent[3]->SetEventData(new std::string("LocalMain1_VOICE"));

	Button2D buttonDown;
	buttonDown.InitRelative({ 0.5f,0.3f + buttonOffset * 5.0f }, { 512 * resScale, 64 * resScale }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_button"));
	buttonDown.SetText(std::wstring(L" "));
	buttonDown.SetEventType(eLetterRegister::InventoryScroll);
	buttonDown.SetEventData(new int(1));

	inventoryMenu->AddInterfaceComponent(std::make_shared<Button2D>(buttonUp), true);
	inventoryMenu->AddInterfaceComponent(myInventoryContent[0], true);
	inventoryMenu->AddInterfaceComponent(myInventoryContent[1], true);
	inventoryMenu->AddInterfaceComponent(myInventoryContent[2], true);
	inventoryMenu->AddInterfaceComponent(myInventoryContent[3], true);
	inventoryMenu->AddInterfaceComponent(std::make_shared<Button2D>(buttonDown), true);
}

void InterfaceManager::ChangeCurrentMenu(const std::string& aMenuName)
{
	if (myMenuMap.find(aMenuName) == myMenuMap.end())
	{
		//should output menu with name does not exist
		return;
	}

	if (aMenuName == "main_menu")
	{
		mySettingsReturnPoint = "main_menu";
	}

	if (aMenuName == "pause_menu")
	{
		mySettingsReturnPoint = "pause_menu";
	}

	
	//MainSingleton::Get().SetMouseLock(false);
	myCurrentMenu = myMenuMap.at(aMenuName);
	myCurrentMenu->StartCooldown();
}

void InterfaceManager::ChangeToMainMenu()
{
	ChangeCurrentMenu("main_menu");
	
	MainSingleton::Get().GetSceneManager().GetGlobalObjects().Player->SetEnabled(false);
}

void InterfaceManager::ChangeToInGame()
{
	MainSingleton::Get().SetMouseLock(true);
	myCurrentMenu = nullptr;
	MainSingleton::Get().GetSceneManager().GetGlobalObjects().Player->SetEnabled(true);
}

const bool InterfaceManager::IsInMenu() const
{
	if (myCurrentMenu == nullptr)
	{
		return false;
	}
	else if (GetCurrentMenu() == "loading_screen")
	{
		if (myHasTickedFirstFrameOfLoading)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	

	return true;
}

const bool InterfaceManager::ShouldRenderScene() const
{
	if (myCurrentMenu == nullptr)
	{
		return true;
	}
	else if (GetCurrentMenu() == "loading_screen")
	{
		return true;
	}
	else if (GetCurrentMenu() == "pause_menu")
	{
		return true;
	}
	return false;
}

bool InterfaceManager::GetLockMouse() const
{
	return !myCurrentMenu;
}

const std::string& InterfaceManager::GetCurrentMenu() const
{
	return myCurrentMenu->GetName();
}

void InterfaceManager::StopSplashScreen()
{
#ifndef _RETAIL
	myRunSplashScreen = false;
	mySplashScreen = nullptr;
#endif
}

bool InterfaceManager::Input(CU::Vector2f aMousePosition)
{
	CU::Vector2f offset(0, 0);
	if (myCurrentMenu)
	{
		return myCurrentMenu->Input(aMousePosition * EngineSettings::Get().GetCurrentResolutionScale()); //can also do tooltip hover checks in here, as inputmanager is static
	}
	else
	{
		return myHUD->Input(aMousePosition * EngineSettings::Get().GetCurrentResolutionScale()); //can also do tooltip hover checks
	}
}

void InterfaceManager::Draw()
{
	//Needs to call renderer instead
	auto& renderer = MainSingleton::Get().GetRenderer();
	
	renderer.Enqueue<GfxCmd_BeginEvent>("SpriteRender");
	
	renderer.Enqueue<GfxCmd_SetPixelShader>("UI_PS");
	renderer.Enqueue<GfxCmd_SetVertexShader>("UI_VS");
	
	myVignette->Draw();
//
	//myHUD->Draw();
	//
	myTapePlayer->Draw();

	if (myCurrentMenu)
	{
		myCurrentMenu->Draw();
	}
	//
	//commandList->Enqueue<GfxCmd_ChangePipelineState>(GraphicsEngine::Get().GetSpritePSO());
	//
	//if (myCurrentTooltip)
	//{
	//	myCurrentTooltip->Draw();
	//}
	//
	//if (myRunSplashScreen)
	//{
	//	if (mySplashScreen != nullptr)
	//	{
	//		commandList->Enqueue<GfxCmd_ChangePipelineState>(GraphicsEngine::Get().GetSpritePSO());
	//		mySplashScreen->Draw();
	//	}
	//}
	//
	renderer.Enqueue<GfxCmd_EndEvent>();
}

void InterfaceManager::DrawTooltip(std::shared_ptr<Tooltip> aTooltip)
{
	myCurrentTooltip = aTooltip;
	myHasHadTooltipOneFrame = false;
}
std::array<std::shared_ptr<Button2D>, 4> InterfaceManager::GetInventoryContent()
{
	return myInventoryContent;
}

void InterfaceManager::KeyInputs()
{
	if (CU::Input::GetKeyUp(CU::Keys::T))
	{
		std::string tapeName = "LeaderMain1_VOICE";
		GameObjectPostMaster::Get()->Send({ eLetterRegister::TapeStart, &tapeName });
	}
	if (CU::Input::GetKeyUp(CU::Keys::G))
	{
		std::string tapeName = "LeaderMain2_VOICE";
		GameObjectPostMaster::Get()->Send({ eLetterRegister::TapeStart, &tapeName });
	}
	if (CU::Input::GetKeyUp(CU::Keys::B))
	{
		std::string tapeName = "LeaderMain3_VOICE";
		GameObjectPostMaster::Get()->Send({ eLetterRegister::TapeStart, &tapeName });
	}

	if (myLoadingIsFinished)
	{
		if (CU::Input::GetKeyDown(CU::Keys::SPACE))
		{
			GameObjectPostMaster::Get()->Send({ eLetterRegister::LoadingExit });
		}
	}
	if (CU::Input::GetKeyDown(CU::Keys::ESCAPE))
	{
		if (myCurrentMenu)
		{
			if (myCurrentMenu->GetName() == "main_menu") 
			{
				//this should actually not do anything
			}
			else if (myCurrentMenu->GetName() == "pause_menu")
			{
				ChangeToInGame();
			}
			else if (myCurrentMenu->GetName() == "settings_menu")
			{
				ChangeCurrentMenu(mySettingsReturnPoint);
			}
			else if (myCurrentMenu->GetName() == "level_menu")
			{
				ChangeToMainMenu();
			}
			else if (myCurrentMenu->GetName() == "credits_menu")
			{
				ChangeToMainMenu();
			}
			else if (myCurrentMenu->GetName() == "cutscene")
			{
				GameObjectPostMaster::Get()->Send({ eLetterRegister::SkipVideo });
			}
			else if (myCurrentMenu->GetName() == "inventory_menu")
			{
				ChangeToInGame();
			}
		}
		else
		{
			ChangeCurrentMenu("pause_menu");
		}
		
	}

	if (CU::Input::GetKeyDown(CU::Keys::TAB) || CU::Input::GetKeyDown(CU::Keys::I))
	{
		if (myCurrentMenu)
		{
			if (myCurrentMenu->GetName() == "inventory_menu")
			{
				ChangeToInGame();
			}
		}
		else
		{
			ChangeCurrentMenu("inventory_menu");
		}
	}

	if (CU::Input::myXInput.GetButtonDown(XINPUT_GAMEPAD_B))
	{
		if (myCurrentMenu)
		{
			if (myCurrentMenu->GetName() == "main_menu")
			{
				//this should actually not do anything
			}
			else if (myCurrentMenu->GetName() == "pause_menu")
			{
				ChangeToInGame();
			}
			else if (myCurrentMenu->GetName() == "settings_menu")
			{
				ChangeCurrentMenu(mySettingsReturnPoint);
			}
			else if (myCurrentMenu->GetName() == "level_menu")
			{
				ChangeToMainMenu();
			}
			else if (myCurrentMenu->GetName() == "credits_menu")
			{
				ChangeToMainMenu();
			}
		}
	}
}

void InterfaceManager::InitHud()
{
	float resScale = EngineSettings::Get().GetResolutionScale();
	resScale *= 0.74f;

	if (myHUD)
	{
		myHUD->Reset();
		myHUD = nullptr;
	}

	myHUD = std::make_shared<InterfaceMenu>();
	myHUD->SetName("HUD");

	myTapePlayer = std::make_shared<TapePlayer>();
	myTapePlayer->Initialize();
	//myHUD->AddInterfaceComponent(tape);
}

void InterfaceManager::InitMainMenu()
{
	float resScale = EngineSettings::Get().GetResolutionScale();
	if (myMenuMap.contains("main_menu"))
	{
		myMenuMap["main_menu"]->Reset();
		myMenuMap["main_menu"] = nullptr;
	}
	std::shared_ptr<InterfaceMenu> mainMenu = std::make_shared<InterfaceMenu>();
	mainMenu->SetName("main_menu");
	myMenuMap[mainMenu->GetName()] = mainMenu;

	SpriteInstance background;
	background.InitRelative({ 0.5f, 0.5f }, { 2048 * resScale, 2048 * resScale }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_title_screen_background"), SAnchorPoint::Center);

	mainMenu->AddInterfaceComponent(std::make_shared<SpriteInstance>(background));

	SpriteInstance logo;
	logo.InitRelative({ 0.5f, 0.2f }, { 1024 * resScale,512 * resScale }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_gamelogo"), SAnchorPoint::Center);

	mainMenu->AddInterfaceComponent(std::make_shared<SpriteInstance>(logo));


	float buttonOffset = 0.08f;
	Button2D buttonStart;
	buttonStart.InitRelative({ 0.5f,0.5f }, { 512 * resScale, 64 * resScale }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_button"));
	buttonStart.SetText(std::wstring(L"Start Game"));
	buttonStart.SetEventType(eLetterRegister::PlayVideo);

	{
		Letter* crazyLetter = new Letter();
		crazyLetter->myType = eLetterRegister::StartGame;
		buttonStart.SetEventData(new std::pair<std::string*, Letter*>(new std::string("Assets/Video/intro.mp4"), crazyLetter));
	}
	

	Button2D buttonLevel;
	buttonLevel.InitRelative({ 0.5f,0.5f + buttonOffset * 1.0f}, { 512 * resScale, 64 * resScale }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_button"));
	buttonLevel.SetText(std::wstring(L"Level Select"));
	buttonLevel.SetEventType(eLetterRegister::GoToLevelSelect);

	Button2D buttonSettings;
	buttonSettings.InitRelative({ 0.5f,0.5f + buttonOffset * 2.0f }, { 512 * resScale, 64 * resScale }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_button"));
	buttonSettings.SetText(std::wstring(L"Settings"));
	buttonSettings.SetEventType(eLetterRegister::GoToSettingsMenu);

	Button2D buttonCredits;
	buttonCredits.InitRelative({ 0.5f,0.5f + buttonOffset * 3.0f }, { 512 * resScale, 64 * resScale }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_button"));
	buttonCredits.SetText(std::wstring(L"Credits"));
	buttonCredits.SetEventType(eLetterRegister::GoToCreditsMenu);

	Button2D buttonExit;
	buttonExit.InitRelative({ 0.5f,0.5f + buttonOffset * 5.0f }, { 256 * resScale, 64 * resScale }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_small_button"));
	buttonExit.SetText(std::wstring(L"Exit Game"));
	buttonExit.SetEventType(eLetterRegister::ExitGame);

	mainMenu->AddInterfaceComponent(std::make_shared<Button2D>(buttonStart), true);
	mainMenu->AddInterfaceComponent(std::make_shared<Button2D>(buttonLevel), true);
	mainMenu->AddInterfaceComponent(std::make_shared<Button2D>(buttonSettings), true);
	mainMenu->AddInterfaceComponent(std::make_shared<Button2D>(buttonCredits), true);
	mainMenu->AddInterfaceComponent(std::make_shared<Button2D>(buttonExit), true);

}

void InterfaceManager::InitPauseMenu()
{
	float resScale = EngineSettings::Get().GetResolutionScale();
	std::shared_ptr<InterfaceMenu> pauseMenu = std::make_shared<InterfaceMenu>();
	pauseMenu->SetName("pause_menu");
	myMenuMap[pauseMenu->GetName()] = pauseMenu;

	SpriteInstance background;
	background.InitRelative({ 0.5f, 0.5f }, { 2048 * resScale, 2048 * resScale }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_void"), SAnchorPoint::Center);
	background.SetAlpha(0.5f);

	pauseMenu->AddInterfaceComponent(std::make_shared<SpriteInstance>(background));

	
	TextComponent pauseText;
	pauseText.InitRelative(0.5f, 0.325f, 1.5f * resScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"PAUSED"));
	pauseText.SetColor(CU::WHITE);

	pauseMenu->AddInterfaceComponent(std::make_shared<TextComponent>(pauseText));

	float buttonOffset = 0.08f;
	Button2D buttonStart;
	buttonStart.InitRelative({ 0.5f,0.4f }, { 512 * resScale, 64 * resScale }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_button"));
	buttonStart.SetText(std::wstring(L"Return to game"));
	buttonStart.SetEventType(eLetterRegister::BackToGame);

	Button2D buttonSettings;
	buttonSettings.InitRelative({ 0.5f,0.4f + buttonOffset * 1.0f }, { 512 * resScale, 64 * resScale }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_button"));
	buttonSettings.SetText(std::wstring(L"Settings"));
	buttonSettings.SetEventType(eLetterRegister::GoToSettingsMenu);

	Button2D buttonMenu;
	buttonMenu.InitRelative({ 0.5f,0.5f + buttonOffset * 2.0f }, { 512 * resScale, 64 * resScale }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_button"));
	buttonMenu.SetText(std::wstring(L"Exit to main menu"));
	buttonMenu.SetEventType(eLetterRegister::GoToMainMenu);

	Button2D buttonExit;
	buttonExit.InitRelative({ 0.5f,0.5f + buttonOffset * 3.0f }, { 512 * resScale, 64 * resScale }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_button"));
	buttonExit.SetText(std::wstring(L"Exit to desktop"));
	buttonExit.SetEventType(eLetterRegister::ExitGame);

	pauseMenu->AddInterfaceComponent(std::make_shared<Button2D>(buttonStart), true);
	pauseMenu->AddInterfaceComponent(std::make_shared<Button2D>(buttonSettings), true);
	pauseMenu->AddInterfaceComponent(std::make_shared<Button2D>(buttonMenu), true);
	pauseMenu->AddInterfaceComponent(std::make_shared<Button2D>(buttonExit), true);
}

void InterfaceManager::InitSettingsMenu()
{
	float resScale = EngineSettings::Get().GetResolutionScale();
	std::shared_ptr<InterfaceMenu> settingsMenu = std::make_shared<InterfaceMenu>();
	settingsMenu->SetName("settings_menu");
	myMenuMap[settingsMenu->GetName()] = settingsMenu;

	SpriteInstance background;
	background.InitRelative({ 0.5f, 0.5f }, { 2048 * resScale, 2048 * resScale }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_settings_background"), SAnchorPoint::Center);

	settingsMenu->AddInterfaceComponent(std::make_shared<SpriteInstance>(background));


	TextComponent pauseText;
	pauseText.InitRelative(0.5f, 0.15f, 1.75f * resScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"SETTINGS"));
	pauseText.SetColor(CU::WHITE);

	settingsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(pauseText));

	float buttonOffset = 0.10f;
	float buttonStart = 0.25f;

	float textScale = 1.1f * resScale;
	float buttonWidth = 512.0f * resScale;
	float buttonHeight = 64.0f * resScale;

	TextComponent textSliderMaster;
	textSliderMaster.InitRelative(0.5f, buttonStart - buttonOffset * 0.5f, textScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Master Volume"));
	settingsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(textSliderMaster));
	Slider  sliderVolumeMaster;
	sliderVolumeMaster.InitRelative(0.5f, buttonStart, buttonWidth, buttonHeight);
	sliderVolumeMaster.SetValue(EngineSettings::Get().GetMasterVolume());
	sliderVolumeMaster.SetEventType(eLetterRegister::SetMasterVolume);

	TextComponent textSliderMusic;
	textSliderMusic.InitRelative(0.5f, buttonStart + buttonOffset * 1.0f - buttonOffset * 0.5f, textScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Music Volume"));
	settingsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(textSliderMusic));
	Slider  sliderVolumeMusic;
	sliderVolumeMusic.InitRelative(0.5f, buttonStart + buttonOffset * 1.0f, buttonWidth, buttonHeight);
	sliderVolumeMusic.SetValue(EngineSettings::Get().GetMusicVolume());
	sliderVolumeMusic.SetEventType(eLetterRegister::SetMusicVolume);

	TextComponent textSliderEffects;
	textSliderEffects.InitRelative(0.5f, buttonStart + buttonOffset * 2.0f - buttonOffset * 0.5f, textScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Effects Volume"));
	settingsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(textSliderEffects));
	Slider  sliderVolumeEffects;
	sliderVolumeEffects.InitRelative(0.5f, buttonStart + buttonOffset * 2.0f, buttonWidth, buttonHeight);
	sliderVolumeEffects.SetValue(EngineSettings::Get().GetEffectsVolume());
	sliderVolumeEffects.SetEventType(eLetterRegister::SetEffectsVolume);

	TextComponent textCheckbox;
	textCheckbox.InitRelative(0.5f, buttonStart + buttonOffset * 4.0f - buttonOffset * 0.5f, textScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Mute All Audio"));
	settingsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(textCheckbox));
	Checkbox checkbox;
	checkbox.InitRelative(0.5f, buttonStart + buttonOffset * 4.0f, buttonHeight, buttonHeight);
	checkbox.SetState(EngineSettings::Get().GetAudioMuted());
	checkbox.SetEventType(eLetterRegister::SetMuteAll);


	TextComponent textSliderVoice;
	textSliderVoice.InitRelative(0.5f, buttonStart + buttonOffset * 3.0f - buttonOffset * 0.5f, textScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Voice Volume"));
	settingsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(textSliderVoice));
	Slider  sliderVolumeVoice;
	sliderVolumeVoice.InitRelative(0.5f, buttonStart + buttonOffset * 3.0f, buttonWidth, buttonHeight);
	sliderVolumeVoice.SetValue(EngineSettings::Get().GetVoiceVolume());
	sliderVolumeVoice.SetEventType(eLetterRegister::SetVoiceVolume);

	TextComponent textOptionsResolution;
	textOptionsResolution.InitRelative(0.5f, buttonStart + buttonOffset * 5.0f - buttonOffset * 0.5f, textScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Resolution"));
	settingsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(textOptionsResolution));
	OptionsList  optionsListResolution;
	optionsListResolution.InitRelative(0.5f, buttonStart + buttonOffset * 5.0f, buttonWidth, buttonHeight, AssetManager::Get().GetAsset<SpriteAsset>("sprite_options_list"));
	{
		optionsListResolution.SetData(EngineSettings::Get().GetResolutionList());
	}
	optionsListResolution.SetCurrentOption(EngineSettings::Get().GetResolutionIndex());
	optionsListResolution.SetEventType(eLetterRegister::SetResolution);

	Button2D buttonExit;
	buttonExit.InitRelative({ 0.5f, buttonStart + buttonOffset * 6.5f }, { 256 * resScale, 64 * resScale }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_small_button"));
	buttonExit.SetText(std::wstring(L"Return"));
	buttonExit.SetEventType(eLetterRegister::ReturnFromSettings);

	settingsMenu->AddInterfaceComponent(std::make_shared<Button2D>(buttonExit), true);
	settingsMenu->AddInterfaceComponent(std::make_shared<Slider>(sliderVolumeMaster), true);
	settingsMenu->AddInterfaceComponent(std::make_shared<Slider>(sliderVolumeMusic), true);
	settingsMenu->AddInterfaceComponent(std::make_shared<Slider>(sliderVolumeEffects), true);
	settingsMenu->AddInterfaceComponent(std::make_shared<Slider>(sliderVolumeVoice), true);
	settingsMenu->AddInterfaceComponent(std::make_shared<Checkbox>(checkbox), true);
	settingsMenu->AddInterfaceComponent(std::make_shared<OptionsList>(optionsListResolution), true);
}

void InterfaceManager::InitCreditsMenu()
{
	float resScale = EngineSettings::Get().GetResolutionScale();
	std::shared_ptr<InterfaceMenu> creditsMenu = std::make_shared<InterfaceMenu>();
	creditsMenu->SetName("credits_menu");
	myMenuMap[creditsMenu->GetName()] = creditsMenu;

	SpriteInstance background;
	background.InitRelative({ 0.5f, 0.5f }, { 2048 * resScale, 2048 * resScale }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_credits_background"), SAnchorPoint::Center);

	creditsMenu->AddInterfaceComponent(std::make_shared<SpriteInstance>(background));

	float buttonOffset = 0.03f;
	float buttonStart = 0.25f;

	float headerScale = 1.25f * resScale;
	float breadScale = 1.0f * resScale;
	float buttonWidth = 512.0f * resScale;
	float buttonHeight = 64.0f * resScale;

	float techHeight = 0.2f;
	float techHorizontal = 0.25f;

	float programmerHeight = 0.5f;
	float programmerHorizontal = 0.25f;

	float graphicsHeight = 0.2f;
	float graphicsHorizontal = 0.5f;

	float levelHeight = 0.5f;
	float levelHorizontal = 0.5f;

	float animatorHeight = 0.2f;
	float animatorHorizontal = 0.75f;

	float additionalHeight = 0.5f;
	float additionalHorizontal = 0.75f;

	float audioEngineHeight = 0.75f;
	float audioEngineHorizontal = 0.5f;

	TextComponent pauseText;
	pauseText.InitRelative(0.5f, buttonStart - buttonOffset * 5.5f, 1.5f * resScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"CREDITS"));
	//pauseText.SetColor(CU::WHITE);
	creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(pauseText));
	//TextComponent creditText;
	//creditText.InitRelative(0.5f, 0.355f, 1.2f * resScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"hana"));
	////creditText.SetColor(CU::WHITE);
	//creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(creditText));

	//ANIMATORS
	{
		TextComponent headerText;
		headerText.InitRelative(animatorHorizontal, animatorHeight, headerScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"ANIMATION"));

		TextComponent text1;
		text1.InitRelative(animatorHorizontal, animatorHeight + buttonOffset * 1.0f, breadScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Agnes Bok"));

		TextComponent text2;
		text2.InitRelative(animatorHorizontal, animatorHeight + buttonOffset * 2.0f, breadScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Philip Anderson"));

		TextComponent text3;
		text3.InitRelative(animatorHorizontal, animatorHeight + buttonOffset * 3.0f, breadScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Vilma Löfgren"));

		creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(headerText));
		creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(text1));
		creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(text2));
		creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(text3));
	}

	//PROGRAMMERS
	{
		TextComponent headerText;
		headerText.InitRelative(programmerHorizontal, programmerHeight, headerScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"PROGRAMMING"));

		TextComponent text1;
		text1.InitRelative(programmerHorizontal, programmerHeight + buttonOffset * 1.0f, breadScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Hana Miklos"));

		TextComponent text2;
		text2.InitRelative(programmerHorizontal, programmerHeight + buttonOffset * 2.0f, breadScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Harry Örnö Persson"));

		TextComponent text3;
		text3.InitRelative(programmerHorizontal, programmerHeight + buttonOffset * 3.0f, breadScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Jesse Emmoth"));

		TextComponent text4;
		text4.InitRelative(programmerHorizontal, programmerHeight + buttonOffset * 4.0f, breadScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Linus Svensson"));

		TextComponent text5;
		text5.InitRelative(programmerHorizontal, programmerHeight + buttonOffset * 5.0f, breadScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Viktor Andersson"));

		TextComponent text6;
		text6.InitRelative(programmerHorizontal, programmerHeight + buttonOffset * 6.0f, breadScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Nils Mårtensson"));

		creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(headerText));
		creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(text1));
		creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(text2));
		creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(text3));
		creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(text4));
		creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(text5));
		creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(text6));
	}

	//TECH ARTIST
	{
		TextComponent headerText;
		headerText.InitRelative(techHorizontal, techHeight, headerScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"TECHNICAL ART"));

		TextComponent text1;
		text1.InitRelative(techHorizontal, techHeight + buttonOffset * 1.0f, breadScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Albin Karlsson"));

		TextComponent text2;
		text2.InitRelative(techHorizontal, techHeight + buttonOffset * 2.0f, breadScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Filip Renemark"));

		creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(headerText));
		creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(text1));
		creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(text2));

	}

	//GRAPHICS
	{
		TextComponent headerText;
		headerText.InitRelative(graphicsHorizontal, graphicsHeight, headerScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"GRAPHICS"));

		TextComponent text1;
		text1.InitRelative(graphicsHorizontal, graphicsHeight + buttonOffset * 1.0f, breadScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Aina Hargne"));

		TextComponent text2;
		text2.InitRelative(graphicsHorizontal, graphicsHeight + buttonOffset * 2.0f, breadScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Maja Rosvall"));

		TextComponent text3;
		text3.InitRelative(graphicsHorizontal, graphicsHeight + buttonOffset * 3.0f, breadScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Fredrik Henningsson"));

		TextComponent text4;
		text4.InitRelative(graphicsHorizontal, graphicsHeight + buttonOffset * 4.0f, breadScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Jonas Harper"));

		creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(headerText));
		creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(text1));
		creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(text2));
		creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(text3));
		creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(text4));

	}

	//LEVEL DESIGN
	{
		TextComponent headerText;
		headerText.InitRelative(levelHorizontal, levelHeight, headerScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"LEVEL DESIGN"));

		TextComponent text1;
		text1.InitRelative(levelHorizontal, levelHeight + buttonOffset * 1.0f, breadScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Pontus Hedenberg"));

		TextComponent text2;
		text2.InitRelative(levelHorizontal, levelHeight + buttonOffset * 2.0f, breadScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Elias Olsson"));

		TextComponent text3;
		text3.InitRelative(levelHorizontal, levelHeight + buttonOffset * 3.0f, breadScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Anna Sjöberg"));


		creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(headerText));
		creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(text1));
		creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(text2));
		creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(text3));

	}

	//SOUND DESIGN
	{
		TextComponent headerText;
		headerText.InitRelative(additionalHorizontal, additionalHeight, headerScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"SOUND DESIGN"));

		TextComponent text1;
		text1.InitRelative(additionalHorizontal, additionalHeight + buttonOffset * 1.0f, breadScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Jesse Emmoth"));



		creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(headerText));
		creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(text1));

	}

	//AUDIO Engine
	{
		TextComponent headerText;
		headerText.InitRelative(audioEngineHorizontal, audioEngineHeight, headerScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"AUDIO ENGINE"));

		TextComponent text1;
		text1.InitRelative(audioEngineHorizontal, audioEngineHeight + buttonOffset * 1.0f, breadScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"FMOD Studio by Firelight Technologies Pty Ltd."));



		creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(headerText));
		creditsMenu->AddInterfaceComponent(std::make_shared<TextComponent>(text1));

	}

	//SpriteInstance fmodLogo;
	//fmodLogo.InitRelative({ 0.775f, 0.85f }, { 768 * resScale, 192 * resScale }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_fmodlogo"), SAnchorPoint::Center);
	//creditsMenu->AddInterfaceComponent(std::make_shared<SpriteInstance>(fmodLogo));

	//fmod logo not required in credist

	Button2D buttonExit;
	buttonExit.InitRelative({ 0.5f, buttonStart + 0.1f * 6.5f }, { buttonWidth * 0.5f, buttonHeight}, AssetManager::Get().GetAsset<SpriteAsset>("sprite_small_button"));
	buttonExit.SetText(std::wstring(L"Return"));
	buttonExit.SetEventType(eLetterRegister::GoToMainMenu);

	creditsMenu->AddInterfaceComponent(std::make_shared<Button2D>(buttonExit), true);
}

void InterfaceManager::InitLevelSelectMenu()
{
	float resScale = EngineSettings::Get().GetResolutionScale();
	std::shared_ptr<InterfaceMenu> levelMenu = std::make_shared<InterfaceMenu>();
	levelMenu->SetName("level_menu");
	myMenuMap[levelMenu->GetName()] = levelMenu;

	float buttonOffset = 0.10f;
	float buttonStart = 0.25f;

	//float textScale = 1.1f * resScale;
	float buttonWidth = 512.0f * resScale;
	float buttonHeight = 64.0f * resScale;

	SpriteInstance background;
	background.InitRelative({ 0.5f, 0.5f }, { 2048 * resScale, 2048 * resScale }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_level_background"), SAnchorPoint::Center);

	levelMenu->AddInterfaceComponent(std::make_shared<SpriteInstance>(background));


	TextComponent levelSelectMenuText;
	levelSelectMenuText.InitRelative(0.5f, buttonStart - buttonOffset * 0.5f, 1.5f * resScale, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"LEVEL SELECT"));
	levelSelectMenuText.SetColor(CU::WHITE);

	levelMenu->AddInterfaceComponent(std::make_shared<TextComponent>(levelSelectMenuText));

	Button2D buttonLevel1;
	buttonLevel1.InitRelative({ 0.5f, buttonStart + buttonOffset }, { buttonWidth, buttonHeight }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_button"));
	buttonLevel1.SetText(std::wstring(L"Level 1"));
	buttonLevel1.SetEventType(eLetterRegister::LoadLevel);
	buttonLevel1.SetEventData(static_cast<void*>(new unsigned(1)));


	Button2D buttonLevel2;
	buttonLevel2.InitRelative({ 0.5f, buttonStart + buttonOffset * 2.0f }, { buttonWidth, buttonHeight }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_button"));
	buttonLevel2.SetText(std::wstring(L"Level 2"));
	buttonLevel2.SetEventType(eLetterRegister::LoadLevel);
	buttonLevel2.SetEventData(static_cast<void*>(new unsigned(2)));

	Button2D buttonLevel3;
	buttonLevel3.InitRelative({ 0.5f, buttonStart + buttonOffset * 3.0f }, { buttonWidth, buttonHeight }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_button"));
	buttonLevel3.SetText(std::wstring(L"Level 3"));
	buttonLevel3.SetEventType(eLetterRegister::LoadLevel);
	buttonLevel3.SetEventData(static_cast<void*>(new unsigned(3)));

	Button2D buttonGymPlayer;
	buttonGymPlayer.InitRelative({ 0.5f, buttonStart + buttonOffset * 3.66f }, { buttonWidth, buttonHeight }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_button"));
	buttonGymPlayer.SetText(std::wstring(L"Player Gym"));
	buttonGymPlayer.SetEventType(eLetterRegister::LoadLevel);
	buttonGymPlayer.SetEventData(static_cast<void*>(new unsigned(4)));

	Button2D buttonGymAsset;
	buttonGymAsset.InitRelative({ 0.5f, buttonStart + buttonOffset * 4.32f }, { buttonWidth, buttonHeight }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_button"));
	buttonGymAsset.SetText(std::wstring(L"Asset Gym"));
	buttonGymAsset.SetEventType(eLetterRegister::LoadLevel);
	buttonGymAsset.SetEventData(static_cast<void*>(new unsigned(5)));

	Button2D buttonGymAnimation;
	buttonGymAnimation.InitRelative({ 0.5f, buttonStart + buttonOffset * 5.f }, { buttonWidth, buttonHeight }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_button"));
	buttonGymAnimation.SetText(std::wstring(L"Animation Gym"));
	buttonGymAnimation.SetEventType(eLetterRegister::LoadLevel);
	buttonGymAnimation.SetEventData(static_cast<void*>(new unsigned(6)));

	Button2D buttonGymVFX;
	buttonGymVFX.InitRelative({ 0.5f, buttonStart + buttonOffset * 5.66f }, { buttonWidth, buttonHeight }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_button"));
	buttonGymVFX.SetText(std::wstring(L"VFX Gym"));
	buttonGymVFX.SetEventType(eLetterRegister::LoadLevel);
	buttonGymVFX.SetEventData(static_cast<void*>(new unsigned(7)));

	Button2D buttonExit;
	buttonExit.InitRelative({ 0.5f, buttonStart + buttonOffset * 6.5f }, { buttonWidth * 0.5f, buttonHeight }, AssetManager::Get().GetAsset<SpriteAsset>("sprite_small_button"));
	buttonExit.SetText(std::wstring(L"Return"));
	buttonExit.SetEventType(eLetterRegister::ReturnFromSettings);

	levelMenu->AddInterfaceComponent(std::make_shared<Button2D>(buttonLevel1), true);
	levelMenu->AddInterfaceComponent(std::make_shared<Button2D>(buttonLevel2), true);
	levelMenu->AddInterfaceComponent(std::make_shared<Button2D>(buttonLevel3), true);
	levelMenu->AddInterfaceComponent(std::make_shared<Button2D>(buttonGymPlayer), true);
	levelMenu->AddInterfaceComponent(std::make_shared<Button2D>(buttonGymAsset), true);
	levelMenu->AddInterfaceComponent(std::make_shared<Button2D>(buttonGymAnimation), true);
	levelMenu->AddInterfaceComponent(std::make_shared<Button2D>(buttonGymVFX), true);
	levelMenu->AddInterfaceComponent(std::make_shared<Button2D>(buttonExit), true);
}

void InterfaceManager::InitLoadingScreen()
{
	//float resScale = EngineSettings::Get().GetResolutionScale();
	std::shared_ptr<InterfaceMenu> loadingScreen = std::make_shared<InterfaceMenu>();
	loadingScreen->SetName("loading_screen");
	myMenuMap[loadingScreen->GetName()] = loadingScreen;

	std::shared_ptr<LoadingScreen> loading = std::make_shared<LoadingScreen>();
	loading->Initialize();

	loadingScreen->AddInterfaceComponent(loading);
}

void InterfaceManager::InitCutscene()
{
	//float resScale = EngineSettings::Get().GetResolutionScale();
	std::shared_ptr<InterfaceMenu> cutscene = std::make_shared<InterfaceMenu>();
	cutscene->SetName("cutscene");
	myMenuMap[cutscene->GetName()] = cutscene;

	std::shared_ptr<VideoComponent> video = std::make_shared<VideoComponent>();

	video->Initialize(static_cast<float>(EngineSettings::Get().GetResolution().width), static_cast<float>(EngineSettings::Get().GetResolution().height));

	cutscene->AddInterfaceComponent(video);
}
