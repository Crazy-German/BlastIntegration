#pragma once

#include <unordered_map>
#include "SoundInfo.h"

#include "CommonUtilities\Vector3.hpp"
#include "../External/FMOD/core/inc/fmod.hpp"

#define AUDIO_CONTENT_ROOT "Assets\\Audio\\"


enum class SoundType;


	class AudioEngine
	{
	public:
		AudioEngine();
		~AudioEngine();
		static AudioEngine& Get() { static AudioEngine myInstance; return myInstance; }

		bool Awake();

		bool ShutDown();

		bool Init();

		bool InitAndLoadAllFiles();

		//USE Player or Cameraposition as listeningposition
		void Update3D(const CU::Vector3f& aListenerPosition, const CU::Vector3f& aForwardDirection, const CU::Vector3f& aUpDirection = {0,1,0});

		void Update(const float aDeltaTime);

		void Set3DPosition(const std::filesystem::path& aFileName, const CU::Vector3f& aPosition);

		//The path is relative to the Bin/Assets/Audio.
		bool LoadSound(const std::filesystem::path& aFileNameWithExtension, SoundType aType);

		//The path is relative to the Bin/Assets/Audio.
		//SFX3D-components adds their files automatically -> not during InitAndLoadAllFiles()!
		bool LoadSound3D(const std::filesystem::path& aFileNameWithExtension, SoundType aType, SoundPriority aPriority, const CU::Vector3f& aPosition);

		//Release resources for specified sound
		void UnloadSound(const std::filesystem::path& aFileName);

		//Filename is without directory and extension
		void PlaySoundFromKey(const std::filesystem::path& aKey, bool aShouldFade = false);

		//X-Fades the current music and specified music
		void ChangeMusic(const std::filesystem::path& aNewMusicFile);

		//Filename is without directory and extension
		void StopSound(const std::filesystem::path& aFileName);

		void StopAllMusic();

		//Volume range is 0.0f-1.0f
		void SetVolume(const float& aNewVolume, SoundType aSoundType);

		//Volume range is 0.0f-1.0f
		float& GetVolume(SoundType aSoundType);

		//Volume range is 0.0f-1.0f
		void SetMasterVolume(const float& aNewVolume);

		//Volume range is 0.0f-1.0f
		float& GetMasterVolume();

		void SetSoundPriority(std::filesystem::path aFileName, SoundPriority aPriority);
	private:
		void UpdateFades(const float aDeltaTime);
		void UpdateCoolDowns(const float aDeltaTime);
		void LoadMusicFiles();
		std::filesystem::path TryGetFile(const std::filesystem::path& aFileNameWithExtension, SoundType aSoundType);
		bool ShouldStream(std::filesystem::path aFileNameWithExtension);
		std::filesystem::path MakeRelative(const std::filesystem::path& aPath, const std::filesystem::path& aContentPath);
		FMOD_VECTOR ToFMODVector(const CU::Vector3f& aVector) const;
	private:
		FMOD_VECTOR myListenerPosition = { 0.0f, 0.0f, 0.0f };
		FMOD_VECTOR myForwardDirection = { 0.0f, 0.0f, 0.0f };
		FMOD_VECTOR myUpDirection = { 0.0f, 0.0f, 0.0f };
		//FMOD_VECTOR mySoundVelocity; Only used for doppler-effect
		FMOD::System* mySystemHandle = nullptr;
		std::unordered_map<std::filesystem::path, unsigned> myPathToIndex;
		std::vector<SoundFile> mySounds;
		FMOD::ChannelGroup* myMixBus = nullptr;;
		FMOD::ChannelGroup* mySFXBus = nullptr;
		FMOD::ChannelGroup* mySFX3DBus = nullptr;
		FMOD::ChannelGroup* myMusicBus = nullptr;
		FMOD::ChannelGroup* myVoiceBus = nullptr;
		float myVolumeCache = 1.0f;
		float myFadeSpeed = 2.0f;
		float mySfxCoolDown = 0.5f;
	};

