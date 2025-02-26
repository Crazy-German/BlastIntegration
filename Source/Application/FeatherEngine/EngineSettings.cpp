#include "EngineSettings.h"
#include <GameObjectSystem/PostMaster/GameObjectPostMaster.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <GraphicsEngine/GraphicsEngine.h>

#include "AudioEngine/AudioEngine.h"


//EngineSettings::EngineSettings()
//{
//	
//}

void EngineSettings::Put(Letter aLetter)
{
	switch (aLetter.myType)
	{
	case eLetterRegister::SetMasterVolume:
		SetMasterVolume(*static_cast<float*>(aLetter.myData));
		break;
	case eLetterRegister::SetMusicVolume:
		SetMusicVolume(*static_cast<float*>(aLetter.myData));
		break;
	case eLetterRegister::SetEffectsVolume:
		SetEffectsVolume(*static_cast<float*>(aLetter.myData));
		break;
	case eLetterRegister::SetEffects3DVolume:
		SetEffects3DVolume(*static_cast<float*>(aLetter.myData));
		break;
	case eLetterRegister::SetVoiceVolume:
		SetVoiceVolume(*static_cast<float*>(aLetter.myData));
		break;
	case eLetterRegister::SetResolution:
		SetResolution(static_cast<Resolution*>(aLetter.myData));
		break;
	case eLetterRegister::SetMuteAll:
		SetMute(*static_cast<bool*>(aLetter.myData));
		break;
	}
}

void EngineSettings::Initialize(HWND aWindowHandle)
{
	myWindowHandle = aWindowHandle;
	GameObjectPostMaster::Get()->Register(eLetterRegister::SetMasterVolume, this);
	GameObjectPostMaster::Get()->Register(eLetterRegister::SetMusicVolume, this);
	GameObjectPostMaster::Get()->Register(eLetterRegister::SetEffectsVolume, this);
	GameObjectPostMaster::Get()->Register(eLetterRegister::SetEffects3DVolume, this);
	GameObjectPostMaster::Get()->Register(eLetterRegister::SetVoiceVolume, this);
	GameObjectPostMaster::Get()->Register(eLetterRegister::SetResolution, this);
	GameObjectPostMaster::Get()->Register(eLetterRegister::SetMuteAll, this);

	myResolution = new Resolution();
	myResolutions = new std::vector<Resolution*>();
	myResolutions->push_back(new Resolution(1280, 720));
	myResolutions->push_back(new Resolution(1366, 768));
	myResolutions->push_back(new Resolution(1600, 900));
	myResolutions->push_back(new Resolution(1920, 1080));
	myResolutions->push_back(new Resolution(2560, 1440));
	myResolutions->push_back(new Resolution(3840, 2160));


	ReadSettingsFromFile();
}

void EngineSettings::WriteSettingsToFile()
{
	nlohmann::json data;

	data["master"] = myMasterVolume;
	data["effects"] = myEffectsVolume;
	data["effects3d"] = myEffects3DVolume;
	data["music"] = myMusicVolume;
	data["voice"] = myVoiceVolume;
	data["resolutionX"] = myResolution->width;
	data["resolutionY"] = myResolution->height;
	data["soundMute"] = mySoundMuted;

	std::ofstream output_file("settings\\EngineSettings.json");
	if (!output_file.is_open()) {
		std::cout << "\n Failed to open output file";
	}
	else {
		output_file << data;
		output_file.close();
	}
}

void EngineSettings::ReadSettingsFromFile()
{
	
	using namespace nlohmann;

	std::filesystem::path settingsPath = "settings\\EngineSettings.json";
	if (std::filesystem::exists(settingsPath))
	{
		std::ifstream jsonFile("settings\\EngineSettings.json");
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

		myMasterVolume = jsonData["master"].get<float>();
		myEffectsVolume = jsonData["effects"].get<float>();
		myEffects3DVolume = jsonData["effects3d"].get<float>();
		myMusicVolume = jsonData["music"].get<float>();
		myVoiceVolume = jsonData["voice"].get<float>();
		myResolution->width = jsonData["resolutionX"].get<int>();
		myResolution->height = jsonData["resolutionY"].get<int>();
		mySoundMuted = jsonData["soundMute"].get<bool>();

		//if no file, initialize values with defaults and create a file, then write to it
	}
	else
	{
		myMasterVolume = 0.5f;
		myEffectsVolume = 0.5f;
		myEffects3DVolume = 0.5f;
		myVoiceVolume = 0.5f;
		myMusicVolume = 0.5f;
		mySoundMuted = false;
		
		RECT clientRect{};
		GetClientRect(myWindowHandle, &clientRect);

		const unsigned clientWidth = clientRect.right - clientRect.left;
		const unsigned clientHeight = clientRect.bottom - clientRect.top;

		//CU::Vector2ui resolution = GraphicsEngine::Get().GetDeviceSize();
		myResolution->width = clientWidth;
		myResolution->height = clientHeight;

		
		WriteSettingsToFile();
	}

	SetMasterVolume(myMasterVolume);
	SetMusicVolume(myMusicVolume);
	SetEffectsVolume(myEffectsVolume);
	SetEffects3DVolume(myEffects3DVolume);
	SetVoiceVolume(myVoiceVolume);
	//GraphicsEngine::Get().SetInternalResolution(myResolution->width, myResolution->height);
}

const float EngineSettings::GetMasterVolume() const
{
	return myMasterVolume;
}

const float EngineSettings::GetMusicVolume() const
{
	return myMusicVolume;
}

const float EngineSettings::GetEffectsVolume() const
{
	return myEffectsVolume;
}

const float EngineSettings::GetEffects3DVolume() const
{
	return myEffects3DVolume;
}

const float EngineSettings::GetVoiceVolume() const
{
	return myVoiceVolume;
}

const bool EngineSettings::GetAudioMuted() const
{
	return mySoundMuted;
}

const float EngineSettings::GetResolutionScale() const
{
	return myResolution->height / 1080.0f;
}

const float EngineSettings::GetResolutionScaleInverse() const
{
	return 1080.0f / myResolution->height;
}

const float EngineSettings::GetCurrentResolutionScale() const
{
	return static_cast<float>(myResolution->height) / static_cast<float>(GraphicsEngine::Get().GetDeviceSize().y);
}

const Resolution EngineSettings::GetResolution() const
{
	return *myResolution;
}

const int EngineSettings::GetResolutionIndex() const
{
	for (int i = 0; i < myResolutions->size(); i++)
	{
		if (myResolutions->at(i)->width == myResolution->width)
		{
			return i;
		}
	}
	return - 1;
}

const int EngineSettings::GetResolutionIndex(Resolution* aResolution) const
{
	for (int i = 0; i < myResolutions->size(); i++)
	{
		if (myResolutions->at(i)->width == aResolution->width)
		{
			return i;
		}
	}
	return -1;
}

std::vector<Resolution*>* EngineSettings::GetResolutionList()
{
	return myResolutions;
}

void EngineSettings::SetMasterVolume(const float aVolume)
{
	myMasterVolume = aVolume;
	if (!mySoundMuted)
	{
		AudioEngine::Get().SetMasterVolume(myMasterVolume);
	}
	WriteSettingsToFile();
}

void EngineSettings::SetMusicVolume(const float aVolume)
{
	myMusicVolume = aVolume;
	AudioEngine::Get().SetVolume(myMusicVolume, SoundType::Music);
	WriteSettingsToFile();
}

void EngineSettings::SetEffectsVolume(const float aVolume)
{
	myEffectsVolume = aVolume;
	AudioEngine::Get().SetVolume(myEffectsVolume, SoundType::SFX);
	WriteSettingsToFile();
}

void EngineSettings::SetEffects3DVolume(const float aVolume)
{
	myEffects3DVolume = aVolume;
	AudioEngine::Get().SetVolume(myEffects3DVolume, SoundType::SFX3D);
	WriteSettingsToFile();
}

void EngineSettings::SetVoiceVolume(const float aVolume)
{
	myVoiceVolume = aVolume;
	AudioEngine::Get().SetVolume(myVoiceVolume, SoundType::Voice);
	WriteSettingsToFile();
}

void EngineSettings::SetMute(const bool aMute)
{
	mySoundMuted = aMute;
	InternalMuteAudio(mySoundMuted);
	WriteSettingsToFile();
}

void EngineSettings::InternalMuteAudio(const bool aMute)
{
	if (aMute)
	{
		AudioEngine::Get().SetMasterVolume(0.0f);
	}
	else
	{
		AudioEngine::Get().SetMasterVolume(myMasterVolume);
	}
}


void EngineSettings::SetResolution(Resolution* aResolution)
{
	myResolution = aResolution;
	//contact graphics engine and actually set the stuff
	//write to file?
	GraphicsEngine::Get().SetInternalResolution((float)myResolution->width, (float)myResolution->height);

	WriteSettingsToFile();
}

Resolution::Resolution()
{
}

Resolution::Resolution(unsigned aX, unsigned aY)
{
	width = aX; 
	height = aY;
}
