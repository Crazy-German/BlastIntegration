#include "LevelSwapComponent.h"

#include "MainSingleton.h"
#include "GameObjectSystem/Scenes/SceneManager.h"

void LevelSwapComponent::Init(const std::string& aScene)
{
	myScene = aScene;
}

void LevelSwapComponent::RecieveInternal(InternalMSG aEvent)
{
	aEvent;
	MainSingleton::Get().GetSceneManager().LoadSceneNextFrame(myScene);
}
