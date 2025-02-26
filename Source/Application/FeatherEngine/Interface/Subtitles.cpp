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
#pragma warning(disable : 4996)



Subtitles::Subtitles()
{

}

Subtitles::~Subtitles()
{
	for (size_t i = 0; i < myTextLines.size(); i++)
	{
		myTextComponents[i] = nullptr;
	}
}

void Subtitles::Initialize()
{
	mySprite = std::make_shared<Sprite2D>();
	mySprite->InitRelative(0.5f, 0.9f, 1000.0f * EngineSettings::Get().GetResolutionScale(), 64.0f * EngineSettings::Get().GetResolutionScale(), AssetManager::Get().GetAsset<SpriteAsset>("sprite_void")->myTexture, AnchorPoint::Center);
	mySprite->SetAlpha(0.75f);

	myCurrentIndex = 0;
	myElapsed = 0.0f;
}

void Subtitles::Update(float aDeltaTime)
{
	myElapsed += aDeltaTime;
	if (myTextLines.size() == myCurrentIndex + 1)
	{

	}
	else
	{
		if (myTextLines[myCurrentIndex + 1].myKey < myElapsed)
		{
			myCurrentIndex++;
		}
	}
	
}

void Subtitles::Draw()
{
	mySprite->Draw();
	myTextComponents[myCurrentIndex]->Draw();
}

void Subtitles::PlaySubtitles()
{
	myCurrentIndex = 0;
	myElapsed = 0.0f;
}

void Subtitles::PrepareSubtitles()
{
	for (int i = 0; i < myTextLines.size(); i++)
	{
		std::shared_ptr<TextComponent> text = std::make_shared<TextComponent>();
		text->InitRelative(0.5f, 0.9f, 1.0f, AssetManager::Get().GetAsset<FontAsset>("F_cascadia-mono")->myFont, std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(myTextLines[i].myText));
		myTextComponents.push_back(text);
	}

	myCurrentIndex = 0;
	myElapsed = 0.0f;
}

const float Subtitles::GetLength() const
{
	return myDuration;
}

//void Subtitles::WriteSubtitleFile()
//{
//	nlohmann::json data;
//
//	float duration = 14.0f;
//
//	SubtitleLine sbubby;
//	sbubby.myKey = 0.2f;
//	sbubby.myText = "We have pinpointed the position of the radio tower.";
//
//	myTextLines.push_back(sbubby);
//
//	sbubby.myKey = 3.1f;
//	sbubby.myText = "If we're quick, we'll make it there in a couple of days.";
//
//	myTextLines.push_back(sbubby);
//
//	sbubby.myKey = 7.0f;
//	sbubby.myText = "We're currently on time and I plan to keep it that way.";
//
//	myTextLines.push_back(sbubby);
//
//	sbubby.myKey = 10.2f;
//	sbubby.myText = "Jane, expedition leader, out.";
//
//	myTextLines.push_back(sbubby);
//
//	data["duration"] = duration;
//	data["lines"] = myTextLines;
//
//	std::ofstream output_file("settings\\Sbubtitle.json");
//	if (!output_file.is_open()) {
//		std::cout << "\n Failed to open output file";
//	}
//	else {
//		output_file << data;
//		output_file.close();
//	}
//}
//
//void Subtitles::ReadSubtitleFile(std::string& aFilename)
//{
//	using namespace nlohmann;
//
//	std::filesystem::path subtitlesPath = "Assets\\UI\\Subtitles\\" + aFilename + ".json";
//	if (std::filesystem::exists(subtitlesPath))
//	{
//		std::ifstream jsonFile(subtitlesPath);
//		json jsonData = json();
//
//		try
//		{
//			jsonData = nlohmann::json::parse(jsonFile);
//		}
//		catch (nlohmann::json::parse_error e)
//		{
//			std::cout << e.what();
//		}
//
//		jsonFile.close();
//
//		myDuration = jsonData["duration"].get<float>();
//		jsonData["lines"].get_to(myTextLines);
//	}
//}
