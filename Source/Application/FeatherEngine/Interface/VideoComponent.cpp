#include "VideoComponent.h"
#include <GraphicsEngine/Video/Video.h>
#include <GraphicsEngine/GraphicsCommandList.h>
#include <GraphicsEngine/GfxCmd_RenderVideo.h>
#include <GameObjectSystem/PostMaster/GameObjectPostMaster.h>
#include <AssetManagement/AssetManager.h>
#include <AssetManagement/AssetTypes/FontAsset.h>
#include "TextComponent.h"
#include <EngineSettings.h>
#include "MainSingleton.h"


VideoComponent::VideoComponent()
{
	MainSingleton::Get().GetAudioEngine().LoadSound("EndMusic_MUSIC.wav", SoundType::Music);
}

VideoComponent::~VideoComponent()
{
	GameObjectPostMaster::Get()->UnRegister(eLetterRegister::PlayVideo, *this);
	GameObjectPostMaster::Get()->UnRegister(eLetterRegister::SkipVideo, *this);
	if (myVideo)
	{
		myVideo->Destroy();
		myVideo = nullptr;
	}
	myText = nullptr;
}

void VideoComponent::Put(Letter aLetter)
{
	switch (aLetter.myType)
	{
	case eLetterRegister::PlayVideo:
		myVideo = std::make_shared<VideoTexture>();
		myVideo->Create(static_cast<std::pair<std::string*, Letter*>*>(aLetter.myData)->first->c_str());
		myIsLoaded = true;
		myIsPaused = false;
		myClickTimer = 0.4f;
		MainSingleton::Get().GetAudioEngine().ChangeMusic("EndMusic_MUSIC");
		
		//vad i guds namn
		myFinishLetter = *static_cast<std::pair<std::string*, Letter*>*>(aLetter.myData)->second;
		break;
	case eLetterRegister::SkipVideo:
		StopAndDestroy();
		break;

	}
}

void VideoComponent::Initialize(float aX, float aY)
{
	myIsLoaded = false;
	myIsPaused = false;
	myClickTimer = 0.4f;
	myClickTimerReset = 0.4f;
	mySkipTextTimer = 0.0f;
	mySkipTextTimerReset = 3.0f;
	GameObjectPostMaster::Get()->Register(eLetterRegister::PlayVideo, this);
	GameObjectPostMaster::Get()->Register(eLetterRegister::SkipVideo, this);
	mySprite->InitRelative(0.5f, 0.5f, aX, aY, AssetManager::Get().GetAsset<SpriteAsset>("sprite_void")->myTexture);
	myText = std::make_shared<TextComponent>();
	myText->InitRelative(0.5f, 0.9f, EngineSettings::Get().GetResolutionScale() * 1.5f, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"Press ESC to skip..."));
}

void VideoComponent::Update(float aDeltaTime)
{
	if (myIsLoaded)
	{
		mySkipTextTimer -= aDeltaTime;
		myClickTimer -= aDeltaTime;
		if (!myIsPaused)
		{
			myVideo->Update(aDeltaTime);
		}
		if (myVideo->HasFinished())
		{
			StopAndDestroy();
		}
	}
}

void VideoComponent::ClickAction(const CU::Vector2f& aClickPosition)
{
	aClickPosition;
	if (myClickTimer < 0.0f)
	{
		mySkipTextTimer = mySkipTextTimerReset;
	}
	if (myShouldThisBePausable)
	{
		if (myIsLoaded)
		{
			if (myClickTimer < 0.0f)
			{
				myClickTimer = myClickTimerReset;
				if (myIsPaused)
				{
					myVideo->Resume();
				}
				else
				{
					myVideo->Pause();
				}
				myIsPaused = !myIsPaused;

			}
		}
	}
}

void VideoComponent::Draw()
{
	if (myIsLoaded)
	{
		//Needs to call renderer instead
		//GraphicsCommandList::Get().Enqueue<GfxCmd_RenderVideo>(myVideo->GetTexture());
		if (mySkipTextTimer > 0.0f)
		{
			myText->Draw();
		}
	}
}

void VideoComponent::StopAndDestroy()
{
	myVideo->Destroy();
	myVideo = nullptr;
	myIsLoaded = false;

	Letter letter;
	letter.myType = eLetterRegister::VideoOver;

	GameObjectPostMaster::Get()->Send(letter);
	GameObjectPostMaster::Get()->Send(myFinishLetter);
}
