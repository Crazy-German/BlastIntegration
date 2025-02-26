#pragma once
#include "GameObjectSystem/GameObject.h"
#include "LevelManagement/JsonParser.h"
#include "GameObjectSystem/GameObjectFactory.h"
#include "AssetManagement/AssetManager.h"

#include <vector>

#include "CommonUtilities/InputObserver.h"

struct DebugBuffer;
struct LightBuffer;
class LightManager;
class MyGui;
struct GameObjectSort
{
	float DistanceToCamera;
	std::shared_ptr<GameObject> GameObject;
	bool Visible;
};

struct Scene
{
	std::shared_ptr<GameObject> ShadowCamera = nullptr;
	std::vector<std::shared_ptr<GameObject>> DeferredObjects;
	std::vector<std::shared_ptr<GameObject>> ForwardObjects;
	std::vector<std::shared_ptr<GameObject>> TAObjects;
	std::vector<std::shared_ptr<GameObject>> NavMeshSwitchers;
	std::vector<std::shared_ptr<GameObject>> myEnemies;
	bool IsGameScene = false;
};

struct GloballyLoadedObjects
{
	std::shared_ptr<GameObject> Camera = nullptr;
	std::shared_ptr<GameObject> Player;
	std::vector<std::shared_ptr<GameObject>> ShaderObjects; // TODO Refactor locations in memory 
	std::shared_ptr<GameObject> MusicPlayer; 
	std::shared_ptr<LightManager> LightManager;
	std::shared_ptr<GameObject> Skybox = nullptr;
	std::shared_ptr<GameObject> ParticleEmiter = nullptr;
};

class SceneManager : public PostBox
{
public:
	friend class MyGui;
	SceneManager();

	~SceneManager();

	void Awake();
	void Init();

	void LoadGlobalObjects();

	void LoadScene(const std::filesystem::path& aPath);

	//Automatically destroys the old scene. Call on this when changing scenes to avoid awkward situations.
	void LoadSceneNextFrame(const std::filesystem::path& aPath);

	void DestroyScene();

	void UpdateScene(float aDeltaTime);

	bool GetIsLoading();

	void RenderScene();

	Scene& GetScene();

	int GetCurrentLevel();

	void AddObject(std::shared_ptr<GameObject> aGameObject);

	void AddTAObject(std::shared_ptr<GameObject> aGameObject);

	GloballyLoadedObjects& GetGlobalObjects() { return myGloballyLoadedObjects; }

	//Objects are sorted from camera
	const std::vector<GameObjectSort>& GetSortedObjects() { return myDeferredSortedObjects; }

	void CreateModelViewerScene();
	bool& GetLoadedEnemies();

	void SetGameObjectFactory(std::shared_ptr<GameObjectFactory> aGameObjectFactory);
	std::shared_ptr<GameObjectFactory> GetGameObjectFactory() const;

	void ReloadScene();

	void Put(Letter aLetter) override;

	GameObject* GetGameoObject(const char* aObjectName);
private:
	void SortObjects();
	bool LoadPlayerSettings();
	void LoadShaderObjects();
	void InitObjectSort();

	void LoadInternal();

	void ViewFrustumCulling();
	CU::Matrix4x4f ExtractUnrealTransform(std::array<float, 16> unrealMatrix);
	CU::Matrix4x4f ExtractTransposedUnrealTransform(std::array<float, 16> unrealMatrix);
	//void RadiusCulling(const float aRadius);

	void DebugRendering();

	//This function will use player settings json to avoid more jsons in bin
	void LoadCameraSettings();
	void AddObjectToMap(const char* aName, std::shared_ptr<GameObject> aObject);
private:
	Scene myScene;
	std::unordered_map<std::string, std::shared_ptr<GameObject>> myGameObjectMap;
	GloballyLoadedObjects myGloballyLoadedObjects;

	std::vector<GameObjectSort> myDeferredSortedObjects;
	std::vector<GameObjectSort> myForwardSortedObjects;
	std::vector<GameObjectSort> myTASortedObjects;

	std::vector<GameObjectSort> myTAVSDefaultPSSortedObjects;
	std::vector<GameObjectSort> myDefaultVSTAPSSortedObjects;
	std::vector<GameObjectSort> myTAVSTAPSSortedObjects;

	GaiJsonParser myJsonParser;

	AssetManager* myAssetManagerRef = nullptr;

	std::shared_ptr<GameObjectFactory> myGameObjectFactory = nullptr;

#ifndef _RETAIL
	std::shared_ptr<DebugBuffer> myDebugBuffer;
#endif
	float myCameraFov;

	std::thread loadThread;
	std::atomic<bool> myIsLoadingScene = true;
	bool myHasLoadedScene = false;
	bool myIsDebugMode;
	bool myHasLoadedEnemies = false;
	int myLatestID;
	int myCurrentLevel;
	bool myFirst = false;

	std::filesystem::path myCurrentLevelPath;

	std::filesystem::path mySceneToLoad;

	std::unordered_map<std::string, std::filesystem::path> myLevels;
};