#pragma once

#include "Component.h"

#include <filesystem>

enum class SoundType;
enum class SoundPriority;

struct ObjectSound
{
	ObjectSound() = delete;
	ObjectSound(const std::filesystem::path& aPath, SoundType aType, const char* aTrigger = "")
	{
		Key = aPath;
		Type = aType;
		Trigger = aTrigger;
	}
	std::filesystem::path Key;
	SoundType Type;
	const char* Trigger = "";
};


#define AUDIO_SOURCE_COMPONENT 1000
class AudioSourceComponent : public Component
{
public:
	AudioSourceComponent(bool aShouldUnloadWhenDestroyed = false);
	~AudioSourceComponent();

	void Awake() override;
	void Init();
	void Destroy();

	//Add update that changes the position of all 3dsounds
	void Update(const float& aDeltaTime);

	bool AddSound(const std::filesystem::path& aFilenameWithExtension, SoundType aType, SoundPriority aPriority);

	//Temp
	bool TagSound(const std::filesystem::path& aFileNameWithExtension, const char* aTag);

	void MakeTagSound();
	void MakeSound(const std::filesystem::path& aKey);

	void RecieveInternal(InternalMSG aEvent) override;

private:
	std::vector<ObjectSound> mySounds;
	std::unordered_map<std::filesystem::path, size_t> myKeyToIndex;
	std::unordered_map<const char*, std::filesystem::path> myTagToKey;
	bool myShouldUnload;

	//temp
	bool myHasTriggered = false;
};
