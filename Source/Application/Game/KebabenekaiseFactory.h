#pragma once
#include "GameObjectSystem/GameObjectFactory.h"

class KebabenekaiseFactory : public GameObjectFactory
{
public:
	KebabenekaiseFactory(std::shared_ptr<ComponentSystem> aComponentSystem);
	void Fabricate(std::shared_ptr<GameObject> outGameObject, const std::string& aGameObjectType, const std::vector<std::string>& aTags) override;
	void Fabricate(std::shared_ptr<GameObject> outGameObject, const std::string& aGameObjectType) override;
private:
};