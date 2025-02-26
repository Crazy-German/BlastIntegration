#include "LoadingScreen.h"
#include "TextComponent.h"
#include "AssetManagement/AssetManager.h"
#include "SpriteInstance.h"
#include <EngineSettings.h>
#include <AssetManagement/AssetTypes/FontAsset.h>
#include <GameObjectSystem/PostMaster/GameObjectPostMaster.h>
#include "MainSingleton.h"

LoadingScreen::LoadingScreen()
{
	MainSingleton::Get().GetAudioEngine().LoadSound("MaskBurning_SFX.wav", SoundType::SFX);
}

LoadingScreen::~LoadingScreen()
{
	GameObjectPostMaster::Get()->UnRegister(eLetterRegister::LoadingLevel, *this);
	GameObjectPostMaster::Get()->UnRegister(eLetterRegister::LoadLevel, *this);
	GameObjectPostMaster::Get()->UnRegister(eLetterRegister::LoadingDone, *this);
	GameObjectPostMaster::Get()->UnRegister(eLetterRegister::LoadingExit, *this);
	GameObjectPostMaster::Get()->UnRegister(eLetterRegister::SetLoadingScreenMask, *this);

	myBackground = nullptr;
	myLoadingText = nullptr;
}

void LoadingScreen::Put(Letter aLetter)
{
	switch (aLetter.myType)
	{
	case eLetterRegister::LoadingLevel:
		Start();
		break;
	case eLetterRegister::LoadLevel:
	{
		//unsigned index = *static_cast<unsigned*>(aLetter.myData);
		//switch (index)
		//{
		//case 1:
		//	myLevelIndex = 0;
		//	break;
		//case 2:
		//	myLevelIndex = 1;
		//	break;
		//case 3:
		//	myLevelIndex = 2;
		//	break;
		//case 4:
		//	myLevelIndex = 3;
		//	break;
		//}
	}
		break;
	case eLetterRegister::SetLoadingScreenMask:
	{
		unsigned index = *static_cast<unsigned*>(aLetter.myData);
		switch (index)
		{
		case 1:
			myLevelIndex = 0;
			break;
		case 2:
			myLevelIndex = 1;
			break;
		case 3:
			myLevelIndex = 2;
			break;
		case 4:
			myLevelIndex = 3;
			break;
		}
	}
	break;
	case eLetterRegister::LoadingDone:
		Stop();
		break;
	case eLetterRegister::LoadingExit:
		Fade();
		break;
	}
}

void LoadingScreen::Initialize()
{
	float resScale = EngineSettings::Get().GetResolutionScale();
	myBackground = std::make_shared<SpriteInstance>();
	myBackground->InitRelative(0.5f, 0.5f, 2048 * resScale, 2048 * resScale, AssetManager::Get().GetAsset<SpriteAsset>("sprite_void"));

	{
		std::shared_ptr<SpriteInstance> voidy = std::make_shared<SpriteInstance>();
		voidy->InitRelative(0.5f, 0.5f, 1024 * resScale, 1024 * resScale, AssetManager::Get().GetAsset<SpriteAsset>("sprite_preparation_load"));
		//voidy->SetAlpha(0.0f);

		std::shared_ptr<SpriteInstance> maskAnxiety = std::make_shared<SpriteInstance>();
		maskAnxiety->InitRelative(0.5f, 0.5f, 1024 * resScale, 1024 * resScale, AssetManager::Get().GetAsset<SpriteAsset>("sprite_mask_burn_anxiety"));

		std::shared_ptr<SpriteInstance> maskAngry = std::make_shared<SpriteInstance>();
		maskAngry->InitRelative(0.5f, 0.5f, 1024 * resScale, 1024 * resScale, AssetManager::Get().GetAsset<SpriteAsset>("sprite_mask_burn_angry"));

		std::shared_ptr<SpriteInstance> maskSad = std::make_shared<SpriteInstance>();
		maskSad->InitRelative(0.5f, 0.5f, 1024 * resScale, 1024 * resScale, AssetManager::Get().GetAsset<SpriteAsset>("sprite_mask_burn_sad"));

		myMasksBurning.push_back(voidy);
		myMasksBurning.push_back(maskAnxiety);
		myMasksBurning.push_back(maskAngry);
		myMasksBurning.push_back(maskSad);
	}

	myLoadingText = std::make_shared<TextComponent>();
	myLoadingText->InitRelative(0.5f, myTextHeight, resScale * 1.5f, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Loading"));
	GameObjectPostMaster::Get()->Register(eLetterRegister::LoadLevel, this);
	GameObjectPostMaster::Get()->Register(eLetterRegister::LoadingLevel, this);
	GameObjectPostMaster::Get()->Register(eLetterRegister::LoadingDone, this);
	GameObjectPostMaster::Get()->Register(eLetterRegister::LoadingExit, this);
	GameObjectPostMaster::Get()->Register(eLetterRegister::SetLoadingScreenMask, this);
}

void LoadingScreen::Update(float aDeltaTime)
{
	myTimer += aDeltaTime;
	float resScale = EngineSettings::Get().GetResolutionScale();

	if (myIsLoading)
	{
		int dotThing = static_cast<int>(myTimer) % 4;
		if (dotThing != myLastText)
		{
			myLastText = dotThing;
			switch (dotThing)
			{
			case 0:
				myLoadingText = nullptr;
				myLoadingText = std::make_shared<TextComponent>();
				myLoadingText->InitRelative(0.5f, myTextHeight, resScale * 1.5f, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Loading"));
				break;
			case 1:
				myLoadingText = nullptr;
				myLoadingText = std::make_shared<TextComponent>();
				myLoadingText->InitRelative(0.5f, myTextHeight, resScale * 1.5f, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Loading:"));
				break;
			case 2:
				myLoadingText = nullptr;
				myLoadingText = std::make_shared<TextComponent>();
				myLoadingText->InitRelative(0.5f, myTextHeight, resScale * 1.5f, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Loading::"));
				break;
			case 3:
				myLoadingText = nullptr;
				myLoadingText = std::make_shared<TextComponent>();
				myLoadingText->InitRelative(0.5f, myTextHeight, resScale * 1.5f, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Loading:::"));
				break;
			}
		}
	}
	
	if (myIsFadingOut)
	{

		myMasksBurning[myLevelIndex]->SetAlpha(std::lerp(1.0f, 0.0f, myTimer * 4.0f));
		
		myBackground->SetAlpha(std::lerp(1.0f, 0.0f, myTimer * 2.0f));
		if (myTimer >= 0.5f)
		{
			myIsFadingOut = false;
			myIsLoading = false;
			GameObjectPostMaster::Get()->Send({ eLetterRegister::LoadFadeDone, nullptr });
		}
	}
	

}

void LoadingScreen::Start()
{
	if (myLevelIndex > 0)
	{
		MainSingleton::Get().GetAudioEngine().PlaySoundFromKey("MaskBurning_SFX");
	}
	for (int i = 0; i < 4; i++)
	{
		myMasksBurning[i]->SetAlpha(1.0f);
	}
	myBackground->SetAlpha(1.0f);
	myIsLoading = true;
	myIsFadingOut = false;
	myTimer = 0;
	myLoadingText = nullptr;
	myLoadingText = std::make_shared<TextComponent>();
	myLoadingText->InitRelative(0.5f, myTextHeight, EngineSettings::Get().GetResolutionScale() * 1.5f, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Loading"));
}

void LoadingScreen::Stop()
{
	myIsLoading = false;
	myIsFadingOut = false;
	myTimer = 0;
	myLoadingText = nullptr;
	myLoadingText = std::make_shared<TextComponent>();
	myLoadingText->InitRelative(0.5f, myTextHeight, EngineSettings::Get().GetResolutionScale() * 1.5f, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Press space to continue"));
}

void LoadingScreen::Fade()
{
	myIsLoading = false;
	myIsFadingOut = true;
	myTimer = 0;
}

void LoadingScreen::Draw()
{
	myBackground->Draw();
	myMasksBurning[myLevelIndex]->Draw();
	if (!myIsFadingOut)
	{
		myLoadingText->Draw();
	}
}
