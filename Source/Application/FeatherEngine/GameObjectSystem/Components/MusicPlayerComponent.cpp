#include "FeatherEngine.pch.h"
#include "MusicPlayerComponent.h"

#include "MainSingleton.h"
#include "GameObjectSystem/PostMaster/GameObjectPostMaster.h"

#include "External\nlohmann\json.hpp"

MusicPlayerComponent::MusicPlayerComponent()
{
}

MusicPlayerComponent::~MusicPlayerComponent()
{
}

void MusicPlayerComponent::Put(Letter aLetter)
{
	int* ptr = nullptr;
	int* levelName;
	switch (aLetter.myType)
	{
	case eLetterRegister::GoToMainMenu:
		PlayMusic(MusicID::MenuMusic);
		return;
	case eLetterRegister::LoadingDone:
		ptr = static_cast<int*>(aLetter.myData);
		levelName = ptr;
		if (*levelName == 1)
		{
			PlayMusic(MusicID::Level1);
			break;
		}
		if (*levelName == 2)
		{
			PlayMusic(MusicID::Level2);
			break;
		}
		if (*levelName == 3)
		{
			PlayMusic(MusicID::Level3);
			break;
		}
		if (*levelName == 4)
		{
			PlayMusic(MusicID::BossLevel);
			break;
		}
		break;

	default:
		break;
	}
	ptr = nullptr;
	levelName = nullptr;
}

void MusicPlayerComponent::Awake()
{
}

void MusicPlayerComponent::Init()
{
	GameObjectPostMaster::Get()->Register(eLetterRegister::LoadingDone, this);
	GameObjectPostMaster::Get()->Register(eLetterRegister::GoToMainMenu, this);

	std::string filePath = "settings\\MusicSettings.json";
	nlohmann::json jsonReader;
	std::fstream file(filePath);

	if (!file.good())
	{
#ifdef _DEBUG
		std::cout << "Unable to load levelselection json file!";
#endif // _DEBUG
		return;
	}
	file >> jsonReader;
	if (jsonReader.contains("MenuMusic"))
	{
		myIDToKey.insert({ MusicID::MenuMusic,jsonReader.at("MenuMusic") });
	}
	if (jsonReader.contains("Level1"))
	{
		myIDToKey.insert({ MusicID::Level1,jsonReader.at("Level1") });
	}
	if (jsonReader.contains("Level2"))
	{
		myIDToKey.insert({ MusicID::Level1,jsonReader.at("Level2") });
	}
	if (jsonReader.contains("Level3"))
	{
		myIDToKey.insert({ MusicID::Level1,jsonReader.at("Level3") });
	}
	if (jsonReader.contains("BossLevel"))
	{
		myIDToKey.insert({ MusicID::Level1,jsonReader.at("BossLevel") });
	}
	LoadMusicFiles();
}

void MusicPlayerComponent::Destroy()
{

}

void MusicPlayerComponent::PlayMusic(MusicID anID)
{
	if (!myIDToKey.contains(anID))
	{
		return;
	}
	//MainSingleton::Get().GetAudioEngine().StopAllMusic();
	MainSingleton::Get().GetAudioEngine().ChangeMusic(myIDToKey.at(anID).stem());
}

void MusicPlayerComponent::AddMusic(MusicID anID, const std::filesystem::path& aFilenameWithExtension)
{
	MainSingleton::Get().GetAudioEngine().LoadSound(aFilenameWithExtension, SoundType::Music);
	myIDToKey.insert({ anID, aFilenameWithExtension.stem() });
}

void MusicPlayerComponent::LoadMusicFiles()
{
	for (auto& music : myIDToKey)
	{
		MainSingleton::Get().GetAudioEngine().LoadSound(music.second, SoundType::Music);
	}
}

void MusicPlayerComponent::UnLoadMusicFiles()
{
	for (auto& music : myIDToKey)
	{
		MainSingleton::Get().GetAudioEngine().UnloadSound(music.second);
	}
}
