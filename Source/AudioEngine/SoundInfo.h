#pragma once

#include <filesystem>
#include "../External/FMOD/core/inc/fmod.hpp"

enum class SoundType
{
	Music,
	SFX,
	SFX3D,
	Voice
};

enum class SoundPriority
{
	None = 1,
	High = 2,
	Medium = 3,
	Low = 4
};

enum class SoundState
{
	FadeIn,
	Playing,
	FadeOut,
	Stopped
};

struct SoundFile
{
	SoundFile() = default;
	SoundFile(SoundType aType, const std::filesystem::path& aFile, SoundPriority aPriority)
	{
		Type = aType;
		File = aFile;
		Sound = nullptr;
		Channel = nullptr;
		Priority = aPriority;
		State = SoundState::Stopped;
		CoolDown = 0.f;
		
	}
	SoundType Type;
	std::filesystem::path File;
	FMOD::Sound* Sound = nullptr;
	FMOD::Channel* Channel = nullptr;
	FMOD::ChannelGroup* Bus = nullptr;
	SoundPriority Priority;
	SoundState State;
	float CoolDown = 0.0f;
};
