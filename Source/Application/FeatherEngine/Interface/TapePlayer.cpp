#include "FeatherEngine.pch.h"
#include "SpriteInstance.h"
#include <AssetManagement/AssetManager.h>
#include "Subtitles.h"
#include <GraphicsEngine/GfxCmd_ChangePipelineState.h>
#include <GraphicsEngine/GfxCmd_SetTextureResource.h>
#include <GraphicsEngine/GraphicsEngine.h>
#include "TextComponent.h"
#include <AssetManagement/AssetTypes/FontAsset.h>
#include "../GameObjectSystem/PostMaster/GameObjectPostMaster.h"
#include <GraphicsEngine/Text/TextFactory.h>
#include <codecvt>
#include <locale>
#include <EngineSettings.h>
#include <nlohmann/json.hpp>
#include "TapePlayer.h"
#include <MainSingleton.h>
#pragma warning(disable : 4996)

using json = nlohmann::json;
void to_json(json& j, const SubtitleLine aLine)
{
	j = json{ {"key", aLine.myKey }, {"line", aLine.myText} };
};

void from_json(const json& j, SubtitleLine& aLine)
{
	j.at("key").get_to(aLine.myKey);
	j.at("line").get_to(aLine.myText);
}

TapePlayer::TapePlayer()
{
}

TapePlayer::~TapePlayer()
{
}

void TapePlayer::Put(Letter aLetter)
{
	aLetter;
}

void TapePlayer::Initialize()
{
	myTape = std::make_shared<SpriteAnimation>();
	myTape->Initialize(0.1f, 0.866f, AssetManager::Get().GetAsset<SpriteAsset>("sprite_tape_anim"), 256.0f, 256.0f, 0.08f, 2);
	myCurrentSubtitles = nullptr;
}

void TapePlayer::Update(float aDeltaTime)
{
	if (myIsPlayingTape)
	{
		myTapeElapsed += aDeltaTime;
		if (myTapeElapsed >= myTapeLength)
		{
			myTape->Stop();
			myTapeDeactivationTimer += aDeltaTime;
			myTape->SetAlpha(1.0f - myTapeDeactivationTimer / myTapeDeactivationTimerReset);
			if (myTapeDeactivationTimer >= myTapeDeactivationTimerReset)
			{
				myIsPlayingTape = false;
			}
		}
		myTape->Update(aDeltaTime);
		if (myCurrentSubtitles != nullptr)
		{
			myCurrentSubtitles->Update(aDeltaTime);
		}
	}
}

void TapePlayer::Draw()
{
	if (myIsPlayingTape)
	{
		myTape->Draw();
		if (myCurrentSubtitles != nullptr)
		{
			myCurrentSubtitles->Draw();
		}
	}
}

void TapePlayer::PlaySubtitles(std::string& aFilename)
{
	myIsPlayingTape = true;
	myTapeElapsed = 0.0f;
	if (myCachedTapes.contains(aFilename))
	{
		myCurrentSubtitles = myCachedTapes[aFilename];
	}
	else
	{
		myCachedTapes[aFilename] = ReadSubtitleFile(aFilename);
		myCurrentSubtitles = myCachedTapes[aFilename];
		myUnlockedTapes.push_back(aFilename);
	}

	if (myLastTapePlayed == "none")
	{
		myLastTapePlayed = aFilename;
	}
	else
	{
		MainSingleton::Get().GetAudioEngine().StopSound(myLastTapePlayed);
		myLastTapePlayed = aFilename;
	}

	MainSingleton::Get().GetAudioEngine().PlaySoundFromKey(aFilename);
	myCurrentSubtitles->PlaySubtitles();
	myTapeLength = myCurrentSubtitles->GetLength();
	myTape->Play();
	myTapeDeactivationTimer = 0.0f;
	myTape->SetAlpha(1.0f);
}

const float TapePlayer::GetLength() const
{
	if (myCurrentSubtitles != nullptr)
	{
		return myCurrentSubtitles->GetLength();
	}
	else
	{
		return 0.0f;
	}
}

void TapePlayer::SetInventoryContent(std::array<std::shared_ptr<Button2D>, 4> aInventoryButtons, int offset)
{
	for (int i = 0; i < 4; i++)
	{
		if (i + offset >= myUnlockedTapes.size())
		{
			aInventoryButtons[i]->SetEventType(eLetterRegister::Test);
			aInventoryButtons[i]->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(" "));
		}
		else
		{
			aInventoryButtons[i]->SetEventType(eLetterRegister::TapeStart);
			aInventoryButtons[i]->SetText(std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(myUnlockedTapes[min(i + offset, (int)myUnlockedTapes.size() - 1)]));
			aInventoryButtons[i]->SetEventData(new std::string(myUnlockedTapes[min(i + offset, (int)myUnlockedTapes.size() - 1)]));
		}
	}
}

const int TapePlayer::GetInventorySize() const
{
	return static_cast<int>(myUnlockedTapes.size());
}

std::shared_ptr<Subtitles>  TapePlayer::ReadSubtitleFile(std::string& aFilename)
{
	using namespace nlohmann;
	std::shared_ptr<Subtitles> subtitles = std::make_shared<Subtitles>();
	std::filesystem::path subtitlesPath = "Assets\\UI\\Subtitles\\" + aFilename + ".json";
	if (std::filesystem::exists(subtitlesPath))
	{
		std::ifstream jsonFile(subtitlesPath);
		json jsonData = json();

		try
		{
			jsonData = nlohmann::json::parse(jsonFile);
		}
		catch (nlohmann::json::parse_error e)
		{
			std::cout << e.what();
		}

		jsonFile.close();

		subtitles->Initialize();
		subtitles->myDuration = jsonData["duration"].get<float>();
		std::vector<SubtitleLine> yeah;
		jsonData["lines"].get_to(subtitles->myTextLines);
		subtitles->PrepareSubtitles();
	}
	return subtitles;
}
