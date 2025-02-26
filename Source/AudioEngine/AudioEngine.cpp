#include "AudioEngine.h"
#include "SoundInfo.h"
#include "Logger\Logger.h"

#include <winerror.h>


#ifdef _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(AudioEngineLog, "AudioEngine", Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(AudioEngineLog, "AudioEngine", Error);
#endif

DEFINE_LOG_CATEGORY(AudioEngineLog);

AudioEngine::AudioEngine()
{
}

AudioEngine::~AudioEngine()
{
}

bool AudioEngine::Awake()
{
	mySounds.reserve(100);
	myUpDirection = ToFMODVector(CU::Vector3f{ 0.0f, 1.0f, 0.0f });
	//mySoundVelocity = ToFMODVector(CU::Vector3f{ 3400.0f, 3400.0f, 3400.0f });

	FMOD_RESULT result;
	result = FMOD::System_Create(&mySystemHandle);
	if (FAILED(result))
	{
		LOG(AudioEngineLog, Error, "Failed to create FMOD studio system");
		return false;
	}

	result = mySystemHandle->init(256, FMOD_INIT_NORMAL, 0);
	if (FAILED(result))
	{
		LOG(AudioEngineLog, Error, "Failed to initialize FMOD!");
		return false;
	}
	//Set the distancefactor for attenuating 3Dsounds
	mySystemHandle->set3DSettings(1.0f, 100.0f, 0.1f);
	LOG(AudioEngineLog, Log, "FMOD Initialized!");
	//TODO: this is ugly, store in some kind of list
	result = mySystemHandle->createChannelGroup("MixBus", &myMixBus);
	myMixBus->setVolume(1.0f);
	result = mySystemHandle->createChannelGroup("SFXBus", &mySFXBus);
	mySFXBus->setVolume(1.0f);
	result = mySystemHandle->createChannelGroup("SFX3DBus", &mySFX3DBus);
	mySFX3DBus->setVolume(1.0f);
	result = mySystemHandle->createChannelGroup("MusicBus", &myMusicBus);
	myMusicBus->setVolume(1.0f);
	result = mySystemHandle->createChannelGroup("VoiceBus", &myVoiceBus);
	myVoiceBus->setVolume(1.0f);

	myMixBus->addGroup(mySFXBus);
	myMixBus->addGroup(mySFX3DBus);
	myMixBus->addGroup(myMusicBus);
	myMixBus->addGroup(myVoiceBus);


	return true;
}

bool AudioEngine::ShutDown()
{
	for (auto sound : mySounds)
	{
		sound.Sound->release();
	}
	mySystemHandle->close();
	return true;
}

bool AudioEngine::Init()
{
	if (Awake())
	{
		LoadMusicFiles();
		//PlaySoundFromKey("MenuMusic_MUSIC", true);
		return true;
	}
	return false;
}

bool AudioEngine::InitAndLoadAllFiles()
{
	Awake();
	const std::filesystem::path startDir = std::filesystem::current_path();
	const std::filesystem::path contentDir = std::filesystem::current_path() / "Assets" / "Audio";

	for (const auto& file : std::filesystem::recursive_directory_iterator(contentDir))
	{
		if (file.path().has_filename() && file.path().has_extension())
		{

			const std::filesystem::path relative = MakeRelative(file.path(), contentDir);
			const std::string name = relative.filename().stem().string();

			if (name.ends_with("MUSIC"))
			{
				LoadSound(relative, SoundType::Music);
			}
			else if (name.ends_with("SFX"))
			{
				LoadSound(relative, SoundType::SFX);
			}
			else if (name.ends_with("SFX3D"))
			{
				LoadSound(relative, SoundType::SFX3D);
			}
			else if (name.ends_with("VOICE"))
			{
				LoadSound(relative, SoundType::Voice);
			}

		}
	}

	return true;
}

void AudioEngine::Update3D(const CU::Vector3f& aListenerPosition, const CU::Vector3f& aForwardDirection, const CU::Vector3f& aUpDirection)
{
	myListenerPosition = ToFMODVector(aListenerPosition);
	myForwardDirection = ToFMODVector(aForwardDirection);
	myUpDirection = ToFMODVector(aUpDirection);
	mySystemHandle->set3DListenerAttributes(0, &myListenerPosition, 0, &myForwardDirection, &myUpDirection);
	mySystemHandle->update();
}

void AudioEngine::Update(const float aDeltaTime)
{
	UpdateCoolDowns(aDeltaTime);
	UpdateFades(aDeltaTime);
}

void AudioEngine::Set3DPosition(const std::filesystem::path& aFileName, const CU::Vector3f& aPosition)
{
	if (!myPathToIndex.contains(aFileName))
	{
		return;
	}
	FMOD_VECTOR pos = ToFMODVector(aPosition);
	mySounds[myPathToIndex.at(aFileName)].Channel->set3DAttributes(&pos, nullptr);
}

bool AudioEngine::LoadSound(const std::filesystem::path& aFileNameWithExtension, SoundType aType)
{
	std::filesystem::path file = TryGetFile(aFileNameWithExtension, aType);
	const std::filesystem::path correctPath = AUDIO_CONTENT_ROOT + file.string();
	if (!std::filesystem::exists(correctPath))
	{
		return false;
	}
	if (myPathToIndex.contains(correctPath.stem()))
	{
		return true;
	}

	myPathToIndex.insert({ correctPath.stem(), static_cast<unsigned>(mySounds.size())});
	mySounds.emplace_back(aType, correctPath, SoundPriority::None);

	FMOD_RESULT soundResult;

	if (ShouldStream(correctPath))
	{
		soundResult = mySystemHandle->createSound(mySounds[myPathToIndex.at(correctPath.stem())].File.string().c_str(), FMOD_CREATESTREAM, 0, &mySounds[myPathToIndex.at(correctPath.stem())].Sound);
	}
	else
	{
		soundResult = mySystemHandle->createSound(mySounds[myPathToIndex.at(correctPath.stem())].File.string().c_str(), FMOD_DEFAULT, 0, &mySounds[myPathToIndex.at(correctPath.stem())].Sound);
	}
	
	if (soundResult != FMOD_OK)
	{
		mySounds.erase(mySounds.begin() + myPathToIndex.at(correctPath.stem()));
		myPathToIndex.erase(correctPath.stem());

		LOG(AudioEngineLog, Error, "Unable to create sound from file {}", aFileNameWithExtension.string());
		return false;
	}

	if (aType == SoundType::Music)
	{
		mySounds[myPathToIndex.at(correctPath.stem())].Sound->setMode(FMOD_LOOP_NORMAL);
		mySounds[myPathToIndex.at(correctPath.stem())].Sound->setLoopCount(10);
		mySounds[myPathToIndex.at(correctPath.stem())].Channel->setChannelGroup(myMusicBus);
		mySounds[myPathToIndex.at(correctPath.stem())].Bus = myMusicBus;
	}
	else if (aType == SoundType::SFX)
	{
		mySounds[myPathToIndex.at(correctPath.stem())].Channel->setChannelGroup(mySFXBus);
		mySounds[myPathToIndex.at(correctPath.stem())].Bus = mySFXBus;
	}
	else if (aType == SoundType::Voice)
	{
		mySounds[myPathToIndex.at(correctPath.stem())].Channel->setChannelGroup(myVoiceBus);
		mySounds[myPathToIndex.at(correctPath.stem())].Bus = myVoiceBus;
	}

	//LOG(AudioEngineLog, Log, "Sound added: {}", aFileNameWithExtension.string());
	return true;
}

bool AudioEngine::LoadSound3D(const std::filesystem::path& aFileNameWithExtension, SoundType aType, SoundPriority aPriority, const CU::Vector3f& aPosition)
{
	std::filesystem::path file = TryGetFile(aFileNameWithExtension, aType);
	const std::filesystem::path correctPath = AUDIO_CONTENT_ROOT + file.string();
	if (!std::filesystem::exists(correctPath))
	{
		return false;
	}
	if (myPathToIndex.contains(correctPath.stem()))
	{
		return true;
	}


	myPathToIndex.insert({ correctPath.stem(), static_cast<unsigned>(mySounds.size()) });
	mySounds.emplace_back(aType, correctPath, aPriority);

	FMOD_RESULT soundResult;

	soundResult = mySystemHandle->createSound(mySounds[myPathToIndex.at(correctPath.stem())].File.string().c_str(), FMOD_3D_LINEARSQUAREROLLOFF, 0, &mySounds[myPathToIndex.at(correctPath.stem())].Sound);
	mySounds[myPathToIndex.at(correctPath.stem())].Sound->set3DMinMaxDistance(10.0f, 5000.0f);
	if (soundResult != FMOD_OK)
	{
		mySounds.erase(mySounds.begin() + myPathToIndex.at(correctPath.stem()));
		myPathToIndex.erase(correctPath.stem());
		LOG(AudioEngineLog, Error, "Unable to create sound from file {}", correctPath.string());
		return false;
	}
	FMOD_VECTOR soundPosition = ToFMODVector(aPosition);
	mySounds[myPathToIndex.at(correctPath.stem())].Channel->set3DAttributes(&soundPosition, 0);
	mySounds[myPathToIndex.at(correctPath.stem())].Channel->setPriority(static_cast<int>(aPriority));

	if (aType == SoundType::SFX3D)
	{
		mySounds[myPathToIndex.at(correctPath.stem())].Channel->setChannelGroup(mySFX3DBus);
		mySounds[myPathToIndex.at(correctPath.stem())].Bus = mySFX3DBus;
	}
	else if (aType == SoundType::Voice)
	{
		mySounds[myPathToIndex.at(correctPath.stem())].Channel->setChannelGroup(myVoiceBus);
		mySounds[myPathToIndex.at(correctPath.stem())].Bus = myVoiceBus;
	}

	//LOG(AudioEngineLog, Log, "3DSound added: {}", aFileNameWithExtension.filename().stem().string());
	return true;
}

void AudioEngine::UnloadSound(const std::filesystem::path& aFileName)
{
	if (!myPathToIndex.contains(aFileName))
	{
		LOG(AudioEngineLog, Log, "Sound could not be unloaded. Sound not found! {}", aFileName.string());
	}
	mySounds[myPathToIndex.at(aFileName)].Sound->release();
	mySounds[myPathToIndex.at(aFileName)].Sound = nullptr;;
	mySounds[myPathToIndex.at(aFileName)].Channel = nullptr;
	mySounds.erase(mySounds.begin() + myPathToIndex.at(aFileName));
	myPathToIndex.erase(aFileName);
}

void AudioEngine::PlaySoundFromKey(const std::filesystem::path& aFileName, bool aShouldFade)
{ 
	if (!myPathToIndex.contains(aFileName))
	{
		LOG(AudioEngineLog, Error, "PlaySound: Sound not found! {}", aFileName.string());
		return;
	}

	if (mySounds[myPathToIndex.at(aFileName)].Type == SoundType::Music)
	{
		if (aShouldFade)
		{
			mySystemHandle->playSound(mySounds[myPathToIndex.at(aFileName)].Sound, mySounds[myPathToIndex.at(aFileName)].Bus, false, &mySounds[myPathToIndex.at(aFileName)].Channel);
			mySounds[myPathToIndex.at(aFileName)].Channel->setVolume(0.0f);
			mySounds[myPathToIndex.at(aFileName)].State = SoundState::FadeIn;
		}
		else
		{
			mySystemHandle->playSound(mySounds[myPathToIndex.at(aFileName)].Sound, mySounds[myPathToIndex.at(aFileName)].Bus, false, &mySounds[myPathToIndex.at(aFileName)].Channel);
			mySounds[myPathToIndex.at(aFileName)].State = SoundState::Playing;
		}

	}
	else
	{
		if (mySounds[myPathToIndex.at(aFileName)].CoolDown < mySfxCoolDown)
		{
			return;
		}
		mySystemHandle->playSound(mySounds[myPathToIndex.at(aFileName)].Sound, mySounds[myPathToIndex.at(aFileName)].Bus, false, &mySounds[myPathToIndex.at(aFileName)].Channel);
		mySounds[myPathToIndex.at(aFileName)].CoolDown = 0.0f;
	}
}

void AudioEngine::ChangeMusic(const std::filesystem::path& aNewMusicFile)
{
	if (!myPathToIndex.contains(aNewMusicFile))
	{
		LOG(AudioEngineLog, Error, "Changemusic: Sound not found!");
		return;
	}

	if (mySounds[myPathToIndex.at(aNewMusicFile)].State == SoundState::Playing)
	{
		return;
	}

	for (auto& sound : mySounds)
	{
		if (sound.State == SoundState::Playing && sound.Type == SoundType::Music)
		{
			sound.State = SoundState::FadeOut;
		}
	}
	PlaySoundFromKey(aNewMusicFile, true);
}

void AudioEngine::StopSound(const std::filesystem::path& aFileName)
{
	if (!myPathToIndex.contains(aFileName))
	{
		LOG(AudioEngineLog, Error, "StopSound: Sound not found!");
		return;
	}
	mySounds[myPathToIndex.at(aFileName)].Channel->stop();
	mySounds[myPathToIndex.at(aFileName)].State = SoundState::Stopped;
}

void AudioEngine::StopAllMusic()
{
	myMusicBus->stop();
}

void AudioEngine::SetVolume(const float& aNewVolume, SoundType aSoundType)
{
	bool volumeValid = true;
	if (aNewVolume > 1)
	{
		volumeValid = false;
		LOG(AudioEngineLog, Error, "You cannot set volume to more than 1. Forcing volume to 1");
	}
	switch (aSoundType)
	{
	case SoundType::Music:
		myMusicBus->setVolume(volumeValid ? aNewVolume : 0.5f);
		return;
	case SoundType::SFX:
		mySFXBus->setVolume(volumeValid ? aNewVolume : 0.5f);
		break;
	case SoundType::SFX3D:
		mySFX3DBus->setVolume(volumeValid ? aNewVolume : 0.5f);
		break;
	case SoundType::Voice:
		myVoiceBus->setVolume(volumeValid ? aNewVolume : 0.5f);
		break;
	default:
		break;
	}
}

float& AudioEngine::GetVolume(SoundType aSoundType)
{
	myVolumeCache = 1.0f;
	switch (aSoundType)
	{
	case SoundType::Music:
		myMusicBus->getVolume(&myVolumeCache);
		break;
	case SoundType::SFX:
		mySFXBus->getVolume(&myVolumeCache);
		break;
	case SoundType::SFX3D:
		mySFX3DBus->getVolume(&myVolumeCache);
		break;
	case SoundType::Voice:
		myVoiceBus->getVolume(&myVolumeCache);
		break;
	default:
		break;
	}
	return myVolumeCache;
}

void AudioEngine::SetMasterVolume(const float& aNewVolume)
{
	bool volumeValid = true;
	if (aNewVolume > 1)
	{
		volumeValid = false;
		LOG(AudioEngineLog, Error, "You cannot set master volume to more than 1.");
	}

	myMixBus->setVolume(volumeValid ? aNewVolume : 0.5f);
}

float& AudioEngine::GetMasterVolume()
{
	myVolumeCache = 1.0f;
	myMixBus->getVolume(&myVolumeCache);
	return myVolumeCache;
}

void AudioEngine::SetSoundPriority(std::filesystem::path aFileName, SoundPriority aPriority)
{
	if (!myPathToIndex.contains(aFileName))
	{
		LOG(AudioEngineLog, Error, "Could not set sound priority for {} . Sound not found");
		return;
	}
	mySounds[myPathToIndex.at(aFileName)].Priority = aPriority;
}

void AudioEngine::UpdateFades(const float aDeltaTime)
{
	for (auto& sound : mySounds)
	{
		if (sound.Type != SoundType::Music)
		{
			continue;
		}
		if (sound.State == SoundState::FadeIn)
		{
			float volume = 0.0f;
			sound.Channel->getVolume(&volume);
			if (volume < 1.0f)
			{
				sound.Channel->setVolume(volume + myFadeSpeed * aDeltaTime);
			}
			else
			{
				sound.State = SoundState::Playing;
				sound.Channel->setVolume(1.0f);
			}
		}
		if (sound.State == SoundState::FadeOut)
		{
			float volume = 0.0f;
			sound.Channel->getVolume(&volume);
			if (volume > 0.0f)
			{
				sound.Channel->setVolume(volume - myFadeSpeed * 2.0f * aDeltaTime);
			}
			else if(volume <= 0.0f)
			{
				sound.State = SoundState::Stopped;
				sound.Channel->setVolume(0.0f);
			}
		}
	}
}

void AudioEngine::UpdateCoolDowns(const float aDeltaTime)
{
	for (auto& sound : mySounds)
	{
		sound.CoolDown += aDeltaTime;
		if (sound.CoolDown > mySfxCoolDown)
		{
			sound.CoolDown = mySfxCoolDown +0.5f;
		}
	}
}

void AudioEngine::LoadMusicFiles()
{
	//Nothing
}

std::filesystem::path AudioEngine::TryGetFile(const std::filesystem::path& aFileNameWithExtension, SoundType aSoundType)
{
	std::filesystem::path testPath = aFileNameWithExtension;
	if (!testPath.has_extension())
	{
		testPath = aFileNameWithExtension.string() + ".wav";
	}
	const char* dir = "";
	switch (aSoundType)
	{
	case SoundType::Music:
		dir = "music\\";
		break;
	case SoundType::SFX:
		dir = "sfx\\";
		break;
	case SoundType::SFX3D:
		dir = "sfx3d\\";
		break;
	case SoundType::Voice:
		dir = "voice\\";
		break;
	default:
		break;
	}
	std::string lowerString = testPath.string();
	for (int i = 0; i < lowerString.size(); i++)
	{
		lowerString[i] = (char)std::tolower(lowerString[i]);
	}

	if (!lowerString.starts_with(dir))
	{
		testPath = dir + testPath.string();
	}
	return testPath;
}

bool AudioEngine::ShouldStream(std::filesystem::path aFileNameWithExtension)
{
	if (std::filesystem::file_size(aFileNameWithExtension) >= 10000000)
	{
		return true;
	}
	return false;
}

std::filesystem::path AudioEngine::MakeRelative(const std::filesystem::path& aPath, const std::filesystem::path& aConentPath)
{
	std::filesystem::path targetPath = relative(aPath, aConentPath);
	targetPath = aConentPath / targetPath;
	targetPath = weakly_canonical(targetPath);

	auto [rootEnd, nothing] = std::mismatch(
		aConentPath.begin(),
		aConentPath.end(),
		targetPath.begin()
	);
	if (rootEnd != aConentPath.end())
	{
		throw std::invalid_argument("Path is not below the root!");
	}
	if (aPath.is_absolute())
	{
		return relative(aPath, aConentPath);
	}
	return aPath;
}

FMOD_VECTOR AudioEngine::ToFMODVector(const CU::Vector3f& aVector) const
{
	FMOD_VECTOR fmodVector;
	fmodVector.x = aVector.x;
	fmodVector.y = aVector.y;
	fmodVector.z = aVector.z;
	return fmodVector;
}

