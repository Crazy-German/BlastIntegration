#include "FeatherEngine.pch.h"
#include "AudioSourceComponent.h"

#include "MainSingleton.h"

AudioSourceComponent::AudioSourceComponent(bool aShouldUnloadWhenDestroyed)
	:myShouldUnload(aShouldUnloadWhenDestroyed)
{
}

AudioSourceComponent::~AudioSourceComponent()
{
}

void AudioSourceComponent::Awake()
{

}

void AudioSourceComponent::Init()
{
	mySounds.reserve(10);
}

void AudioSourceComponent::Destroy()
{
	if (myShouldUnload)
	{
		for (auto& sound : mySounds)
		{
			MainSingleton::Get().GetAudioEngine().UnloadSound(sound.Key);
		}
	}

	myIsMarkedForDestroy = true;
	mySounds.clear();
}

void AudioSourceComponent::Update(const float& aDeltaTime)
{
	aDeltaTime;
	for (auto& sound : mySounds)
	{
		if (sound.Type == SoundType::SFX3D)
		{
			MainSingleton::Get().GetAudioEngine().Set3DPosition(sound.Key, GetGameObject()->GetTransform()->GetPosition());
		}
	}
}

bool AudioSourceComponent::AddSound(const std::filesystem::path& aFilenameWithExtension, SoundType aType, SoundPriority aPriority)
{
	if (aType == SoundType::SFX3D)
	{
		CU::Vector3f objectPositon = GetGameObject()->GetTransform()->GetPosition();
		if (MainSingleton::Get().GetAudioEngine().LoadSound3D(aFilenameWithExtension.filename(), aType, aPriority, objectPositon))
		{
			myKeyToIndex.insert({ aFilenameWithExtension.stem(), mySounds.size() });
			mySounds.emplace_back(aFilenameWithExtension.stem(), aType);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (MainSingleton::Get().GetAudioEngine().LoadSound(aFilenameWithExtension.filename(), aType))
		{
			myKeyToIndex.insert({ aFilenameWithExtension.stem(), mySounds.size() });
			mySounds.emplace_back(aFilenameWithExtension.stem(), aType);
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool AudioSourceComponent::TagSound(const std::filesystem::path& aFileNameWithExtension, const char* aTag)
{
	if (!myKeyToIndex.contains(aFileNameWithExtension.stem()))
	{
		return false;
	}
	myTagToKey.insert({ aTag, aFileNameWithExtension.stem()});
	return true;
}

void AudioSourceComponent::MakeTagSound()
{
	for (auto& tag : myTagToKey)
	{
		MakeSound(tag.second);
	}
}

void AudioSourceComponent::MakeSound(const std::filesystem::path& aKey)
{
	MainSingleton::Get().GetAudioEngine().PlaySoundFromKey(aKey);
}

void AudioSourceComponent::RecieveInternal(InternalMSG aEvent)
{
	if ((aEvent.myEvent == InternalEvent::eHit) && myHasTriggered == false)
	{
		myHasTriggered = true;
		MakeTagSound();
	}
}

