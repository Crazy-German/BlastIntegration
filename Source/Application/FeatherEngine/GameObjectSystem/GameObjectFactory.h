#pragma once
#include "GameObject.h"
#include "ComponentSystem.h"
#include <unordered_map>
#include <string>
#include <vector>

enum class eGameObjectType
{
    StaticMesh, //Ger MeshComponent o tranform
    SkeletalMesh, //Ger MeshComponent, animatör o tranform
    Light, //behöver lightComponent och en transform
    Trigger, //Collidercomponent och transform
	Landscape, //Ger Meshcomponent och checkar tags på ett specifikt sätt för att ge rätt mesh
	MusicPlayer,
	ShaderObject,
	_Last
};

enum class eTagType
{
	Empty,
	Forward,
	TA,
	_Last
};


class GameObjectFactory
{
public:
	GameObjectFactory(std::shared_ptr<ComponentSystem> aComponentSystem);
    void Init();
	virtual void Fabricate(std::shared_ptr<GameObject> outGameObject, const std::string& aGameObjectType);
	virtual void Fabricate(std::shared_ptr<GameObject> outGameObject, const std::string& aGameObjectType, const std::vector<std::string>& aTags);

	virtual std::shared_ptr<GameObject> Fabricate(const std::string& aArchetype, const std::vector<std::string>& aTags);
	virtual std::shared_ptr<GameObject> Fabricate(const std::string& aArchetype);

protected:
	std::shared_ptr<ComponentSystem> myComponentSystem;
	std::unordered_map<std::string, eGameObjectType> myComponentTypeMap;
	std::unordered_map<std::string, eTagType> myTagTypeMap;
};