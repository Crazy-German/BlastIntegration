#include "pch.h"
#include "SGNode_GetActor.h"

#include "MainSingleton.h"
#include "GameObjectSystem/GameObject.h"
#include "GameObjectSystem/Scenes/SceneManager.h"

IMPLEMENT_GRAPH_NODE(SGNode_GetActor, ScriptGraphNode);

SGNode_GetActor::SGNode_GetActor()
{
	CreateDataPin<GameObject*>("Actor", PinDirection::Output);
	CreateDataPin<std::string>("Name", PinDirection::Input);
	CreateDataPin<std::string>("ActorName", PinDirection::Output);
}

NodeResult SGNode_GetActor::DoOperation()
{
	std::string actorName;
	if(GetPinData("Name", actorName))
	{
		MainSingleton::Get().GetSceneManager().GetGameoObject(actorName.c_str());
		SetPinData("Actor", MainSingleton::Get().GetSceneManager().GetGameoObject(actorName.c_str()));
		SetPinData("ActorName", actorName);
	}
	return NoExec();
}
