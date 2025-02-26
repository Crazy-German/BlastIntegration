#pragma once
#include "GameObjectSystem/PostMaster/PostBox.h"
//#include "AudioEngine/AudioEngine.h"
#include <Windows.h>
//#include "windef.h"

struct Resolution
{
	Resolution();
	Resolution(unsigned aX, unsigned aY);
	unsigned width;
	unsigned height;
};

class EngineSettings : public PostBox
{
public:
	//EngineSettings();
	~EngineSettings() = default;

	void Put(Letter aLetter) override;
	static EngineSettings& Get() { static EngineSettings myInstance; return myInstance; };

	void Initialize(HWND aWindowHandle);

	void WriteSettingsToFile();
	void ReadSettingsFromFile();

	const float GetMasterVolume() const;
	const float GetMusicVolume() const;
	const float GetEffectsVolume() const;
	const float GetEffects3DVolume() const;
	const float GetVoiceVolume() const;
	const bool GetAudioMuted() const;

	//Gets ratio of current resolution to 1080p resolution
	const float GetResolutionScale() const;

	const float GetResolutionScaleInverse() const;

	//Gets ratio of current resolution to screen resolution
	const float GetCurrentResolutionScale() const;

	const Resolution GetResolution() const;
	const int GetResolutionIndex() const;
	const int GetResolutionIndex(Resolution* aResolution) const;
	std::vector<Resolution*>* GetResolutionList();

private:

	HWND myWindowHandle;

	float myMasterVolume;
	float myMusicVolume;
	float myEffectsVolume;
	float myEffects3DVolume;
	float myVoiceVolume;

	bool mySoundMuted;

	Resolution* myResolution;

	std::vector<Resolution*>* myResolutions;

	void SetMasterVolume(const float aVolume);
	void SetMusicVolume(const float aVolume);
	void SetEffectsVolume(const float aVolume);
	void SetEffects3DVolume(const float aVolume);
	void SetVoiceVolume(const float aVolume);
	void SetMute(const bool aMute);

	void InternalMuteAudio(const bool aMute);

	//void SetVolumeLevels(const float aMaster, const float aMusic, const float aEffects, const float aEffects3D, const float aVoice);
	void SetResolution(Resolution* aResolution);

};
