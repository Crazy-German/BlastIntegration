#pragma once
#include "CommonUtilities/InputMapper.h"
#include "GameObjectSystem/Scenes/SceneManager.h"
#include "GameObjectSystem\EnemyManager.h"
#include "EngineSettings.h"
#include "Renderer.h"
#include <mutex>
#include "ParticleLoader.h"

#include "../AudioEngine/AudioEngine.h"

struct MiscBuffer;

class MainSingleton
{
public:
	~MainSingleton() = default;

	void Awake();

	static MainSingleton& Get();

	void Update(float aDeltaTime);

	SceneManager& GetSceneManager() { return mySceneManager; }
	CommonUtilities::InputMapper& GetInputMapper() { return myInputMapper; }
	AudioEngine& GetAudioEngine() { return AudioEngine::Get(); }
	EngineSettings& GetSettings() { return EngineSettings::Get(); }
	inline ParticleLoader& GetParticleLoader() { return myParticleLoader; }
	MiscBuffer* GetMiscBuffer();  //be nice to the miscbuffer
	void InitMiscBuffer(MiscBuffer* aBuffer);  //be nice to the miscbuffer
	Renderer& GetRenderer() { return  myRenderer; }

	void SetMouseLock(bool aLock) { myMouseLock = aLock; }
	bool GetMouseLock() { return myMouseLock; }

	void SetWindowHandle(HWND& aHandle) { myGameWindowHandle = aHandle; }
	HWND GetWindowHandle() { return myGameWindowHandle; }
private:
	//AudioEngine myAudioEngine;
	SceneManager mySceneManager;
	CommonUtilities::InputMapper myInputMapper;
	std::unique_ptr<EnemyManager> myEnemyManager;
	ParticleLoader myParticleLoader;

	MiscBuffer* myMiscBuffer;
	Renderer myRenderer;

	bool myMouseLock = true;
	HWND myGameWindowHandle;
	//std::unique_ptr<PhysicsEngine> myPhysicsEngine TODO: Add physicsEngine
	//std::unique_ptr<UIManager> myUIManager TODO: Add UIManager

};
