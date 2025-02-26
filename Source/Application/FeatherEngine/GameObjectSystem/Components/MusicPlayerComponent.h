#pragma once


#include "Component.h"

#include <filesystem>

enum class SoundType;
enum class SoundPriority;

enum class MusicID
{
	MenuMusic,
	Level1,
	Level2,
	Level3,
	BossLevel
};

#define MUSIC_PLAYER_COMPONENT 1
class MusicPlayerComponent : public Component
{
public:
	MusicPlayerComponent();
	~MusicPlayerComponent();

	void Put(Letter aLetter) override;

	void Awake() override;
	void Init();
	void Destroy();

	void PlayMusic(MusicID anID);

private:

	void AddMusic(MusicID anID, const std::filesystem::path& aFilenameWithExtension);
	void LoadMusicFiles();
	void UnLoadMusicFiles();

private:
	std::unordered_map<MusicID, std::filesystem::path> myIDToKey;
};
