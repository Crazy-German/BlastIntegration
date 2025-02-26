#pragma once
#include "../Graphics/GraphicsEngine/ParticleEmitterSettings.h"

class ParticleLoader 
{
public:
	ParticleLoader();
	~ParticleLoader();

	void Initialize();
	//Loads existing particle settings or creates if filepath is valid and settings do not already exist.
	ParticleEmitterSettings& LoadParticleSettings(std::filesystem::path aPath);
	ParticleEmitterSettings& LoadParticleSettings(std::string aName);
private:

	ParticleEmitterSettings LoadParticleSettingsInternal(std::filesystem::path aPath);
	std::unordered_map<std::string, ParticleEmitterSettings> mySavedEmitterSettings;
};