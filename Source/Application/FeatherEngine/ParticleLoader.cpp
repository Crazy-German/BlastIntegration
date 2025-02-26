#include <nlohmann/json.hpp>
#include "ParticleLoader.h"
#include <fstream>
#include <CommonUtilities/VectorJsonCompatibility.hpp>
#include <AssetManagement/AssetTypes/TextureAsset.h>
#include <AssetManagement/AssetManager.h>

ParticleLoader::ParticleLoader()
{
}

ParticleLoader::~ParticleLoader()
{
}

void ParticleLoader::Initialize()
{
}

ParticleEmitterSettings& ParticleLoader::LoadParticleSettings(std::filesystem::path aPath)
{
	std::string settingsName = aPath.stem().string();
	if (!mySavedEmitterSettings.contains(settingsName))
	{
		mySavedEmitterSettings[settingsName] = LoadParticleSettingsInternal(aPath);
	}

	return mySavedEmitterSettings[settingsName];
}

ParticleEmitterSettings& ParticleLoader::LoadParticleSettings(std::string aName)
{
	std::filesystem::path path;
	path = "Assets\\VFX\\Particle Systems\\";
	path.append(aName + ".json");
	
	return LoadParticleSettings(path);
}

ParticleEmitterSettings ParticleLoader::LoadParticleSettingsInternal(std::filesystem::path aPath)
{
	ParticleEmitterSettings particleSettings;
	struct EditorEmitterSettings
	{
		float			rotationalVelocity = 0.0f;
		float			rotationalVelocityEnd = 0.0f;
		bool			randomRotation = false;
		CU::Vector3f	angleStart = { 0.0f, 0.0f, 0.0f };
		CU::Vector3f	angleEnd = { 0.0f, 0.0f,  0.0f };
		CU::Vector3f	gravityStart = { 0.0f, 0.0f, 0.0f };
		CU::Vector3f	gravityEnd = { 0.0f, 0.0f,  0.0f };
		bool			noGravityChange = true;

		CU::Vector3f	velocity = { 0.0f, 0.0f, 0.0f };
		bool			randomVelocityMult = false;

		CU::Vector3f	randomVelocityMin = { 1.0f, 1.0f, 1.0f };
		CU::Vector3f	randomVelocityMax = { 1.0f, 1.0f, 1.0f };

		CU::Vector4f	channelMask = { 1.0f, 1.0f, 1.0f, 1.0f };

		float			lifetime = 5.0f;
		float			spawnRate = 100.0f;
		int				maxParticles = 100;

		bool			anchorToEmitter = false;
		bool			looping = true;

		float			sphereRadius = 1.0f;

		CU::Vector2f	planeSize = { 1.0f, 1.0f };

		CU::Vector3f	boxSize = { 1.0f, 1.0f, 1.0f };

		int				currentShapeIndex = 0;

		CU::Vector4f	colorStart = { 1.0f, 1.0f, 1.0f, 1.0f };
		CU::Vector4f	colorEnd = { 1.0f, 1.0f, 1.0f, 1.0f };
		bool			noColorChange = true;

		CU::Vector2f	sizeStart = { 1.0f, 1.0f };
		CU::Vector2f	sizeEnd = { 1.0f, 1.0f };
		bool			noSizeChange = true;

		float			spawnChance = 1.0f;

		CU::Vector2f	mirrorChance = { 0.0f, 0.0f };

		std::string myPixelShaderName;
	};

	EditorEmitterSettings settings;
	if (std::filesystem::exists(aPath))
	{
		std::ifstream emitterFile(aPath);
		nlohmann::json jsonData = nlohmann::json();

		try
		{
			jsonData = nlohmann::json::parse(emitterFile);
		}
		catch (nlohmann::json::parse_error e)
		{
			std::cout << e.what();
		}

		emitterFile.close();

		std::string textureName;

		settings.rotationalVelocity = jsonData["rotationalVelocityStart"].get<float>();
		settings.rotationalVelocityEnd = jsonData["rotationalVelocityEnd"].get<float>();
		settings.randomRotation = jsonData["randomRotation"].get<bool>();
		settings.angleStart = jsonData["angleStart"].template get<CU::Vector3f>();
		settings.angleEnd = jsonData["angleEnd"].template get<CU::Vector3f>();
		settings.gravityStart = jsonData["gravityStart"].template get<CU::Vector3f>();
		settings.gravityEnd = jsonData["gravityEnd"].template get<CU::Vector3f>();
		settings.noGravityChange = jsonData["noGravityChange"].get<bool>();
		settings.velocity = jsonData["velocity"].template get<CU::Vector3f>();
		settings.randomVelocityMult = jsonData["randomVelocityMultiplier"].get<bool>();
		settings.randomVelocityMin = jsonData["randomVelocityMin"].template get<CU::Vector3f>();
		settings.randomVelocityMax = jsonData["randomVelocityMax"].template get<CU::Vector3f>();
		settings.channelMask = jsonData["channelMask"].template get<CU::Vector4f>();
		settings.lifetime = jsonData["lifetime"].get<float>();
		settings.spawnRate = jsonData["spawnRate"].get<float>();
		settings.maxParticles = jsonData["maxParticles"].get<int>();
		settings.anchorToEmitter = jsonData["anchorToEmitter"].get<bool>();
		settings.looping = jsonData["looping"].get<bool>();
		textureName = jsonData["texturePath"].get<std::string>();
		settings.sphereRadius = jsonData["sphereRadius"].get<float>();
		settings.planeSize = jsonData["planeMin"].template get<CU::Vector2f>();
		settings.boxSize = jsonData["boxMin"].template get<CU::Vector3f>();
		settings.currentShapeIndex = jsonData["shapeIndex"].get<int>();
		settings.colorStart = jsonData["colorStart"].template get<CU::Vector4f>();
		settings.colorEnd = jsonData["colorEnd"].template get<CU::Vector4f>();
		settings.noColorChange = jsonData["noColorChange"].get<bool>();
		settings.sizeStart = jsonData["sizeStart"].template get<CU::Vector2f>();
		settings.sizeEnd = jsonData["sizeEnd"].template get<CU::Vector2f>();
		settings.noSizeChange = jsonData["noSizeChange"].get<bool>();

		//Compatibility
		if (jsonData.contains("spawnChance"))
		{
			settings.spawnChance = jsonData["spawnChance"].get<float>();
		}
		else
		{
			settings.spawnChance = 1.0f;
		}

		if (jsonData.contains("mirrorChance"))
		{
			settings.mirrorChance = jsonData["mirrorChance"].template get<CU::Vector2f>();
		}
		else
		{
			settings.mirrorChance = { 0.0f, 0.0f };
		}

		if (jsonData.contains("shaderName"))
		{
			settings.myPixelShaderName = jsonData["shaderName"].get<std::string>();
		}
		else
		{
			settings.myPixelShaderName.clear();
		}

		

		switch (settings.currentShapeIndex)
		{
		case 0:
			particleSettings.Shape = ParticleEmitterSettings::EmitterShape::Point;
			break;
		case 1:
			particleSettings.Shape = ParticleEmitterSettings::EmitterShape::Sphere;
			break;
		case 2:
			particleSettings.Shape = ParticleEmitterSettings::EmitterShape::Plane;
			break;
		case 3:
			particleSettings.Shape = ParticleEmitterSettings::EmitterShape::Box;
			break;
		}

		if (settings.randomRotation)
		{
			particleSettings.MinSpawnSpin = settings.rotationalVelocity;
			particleSettings.MaxSpawnSpin = settings.rotationalVelocityEnd;
		}
		else
		{
			particleSettings.Spin.AddKey(0.0f, settings.rotationalVelocity);
			particleSettings.Spin.AddKey(1.0f, settings.rotationalVelocity);
		}

		particleSettings.MaxParticles = settings.maxParticles;
		particleSettings.Looping = settings.looping;
		particleSettings.Radius = settings.sphereRadius;
		particleSettings.PlaneSize = settings.planeSize;
		particleSettings.BoxSize = settings.boxSize;
		particleSettings.Gravity.AddKey(0.0f, settings.gravityStart);
		if (settings.noGravityChange)
		{
			particleSettings.Gravity.AddKey(1.0f, settings.gravityStart);
		}
		else
		{
			particleSettings.Gravity.AddKey(1.0f, settings.gravityEnd);
		}

		particleSettings.Velocity = settings.velocity;
		if (settings.randomVelocityMult)
		{
			particleSettings.MinSpawnSpeedMult = settings.randomVelocityMin;
			particleSettings.MaxSpawnSpeedMult = settings.randomVelocityMax;
		}
		else
		{
			particleSettings.MinSpawnSpeedMult = { 1.0f, 1.0f, 1.0f };
			particleSettings.MaxSpawnSpeedMult = { 1.0f, 1.0f, 1.0f };
		}
		particleSettings.SpawnRate = settings.spawnRate;
		particleSettings.channelMask = settings.channelMask;
		particleSettings.AnchorToEmitter = settings.anchorToEmitter;
		
		particleSettings.MinSpawnAngle = settings.angleStart;
		particleSettings.MaxSpawnAngle = settings.angleEnd;
		particleSettings.LifeTime = settings.lifetime;

		if (settings.noColorChange)
		{
			particleSettings.Color.AddKey(0.0f, settings.colorStart);
			particleSettings.Color.AddKey(1.0f, settings.colorStart);
		}
		else
		{
			particleSettings.Color.AddKey(0.0f, settings.colorStart);
			particleSettings.Color.AddKey(1.0f, settings.colorEnd);
		}

		if (settings.noSizeChange)
		{
			particleSettings.Size.AddKey(0.0f, settings.sizeStart);
			particleSettings.Size.AddKey(1.0f, settings.sizeStart);
		}
		else
		{
			particleSettings.Size.AddKey(0.0f, settings.sizeStart);
			particleSettings.Size.AddKey(1.0f, settings.sizeEnd);
		}

		if (textureName.ends_with(".dds"))
		{
			for (int i = 0; i < 4; i++)
			{
				textureName.erase(textureName.end() - 1);
			}
		}

		particleSettings.Texture = AssetManager::Get().GetAsset<TextureAsset>(textureName)->myTexture;

		particleSettings.SpawnChance = settings.spawnChance;
		particleSettings.MirrorChance = settings.mirrorChance;

	}
	return particleSettings;
}
