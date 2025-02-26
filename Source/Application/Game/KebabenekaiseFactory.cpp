#include "KebabenekaiseFactory.h"

KebabenekaiseFactory::KebabenekaiseFactory(std::shared_ptr<ComponentSystem> aComponentSystem) : GameObjectFactory(aComponentSystem)
{
	// Här specifierar man spel unika spel objekt
}

void KebabenekaiseFactory::Fabricate(std::shared_ptr<GameObject> outGameObject, const std::string& aGameObjectType, const std::vector<std::string>& aTags)
{
	GameObjectFactory::Fabricate(outGameObject, aGameObjectType, aTags);
}

void KebabenekaiseFactory::Fabricate(std::shared_ptr<GameObject> outGameObject, const std::string& aGameObjectType)
{
	GameObjectFactory::Fabricate(outGameObject, aGameObjectType);

}
