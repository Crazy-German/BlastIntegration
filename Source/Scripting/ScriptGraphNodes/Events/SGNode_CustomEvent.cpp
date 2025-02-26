#include "pch.h"
#include "SGNode_CustomEvent.h"

#include "GameObjectSystem/GameObject.h"

IMPLEMENT_GRAPH_NODE(SGNode_StartCustomEvent, ScriptGraphTimerEventNode)
IMPLEMENT_GRAPH_NODE(SGNode_CustomEventEntry, ScriptGraphTimerEventNode);


SGNode_StartCustomEvent::SGNode_StartCustomEvent()
{
	AddFlag(ScriptGraphNodeFlag_Compact);
	CreateExecPin("In", PinDirection::Input, false);
	CreateDataPin<GameObject*>("Actor", PinDirection::Input);
	CreateDataPin<std::string>("Event Name", PinDirection::Input);
	CreateDataPin<std::string>("Event", PinDirection::Output);
}

NodeResult SGNode_StartCustomEvent::DoOperation()
{
	GameObject* actor;
	std::string aEvent;
	if(GetPinData("Actor", actor) && GetPinData("Event Name", aEvent))
	{
		SetPinData("Event", aEvent);
		if(actor == nullptr)
		{
			actor = static_cast<GameObject*>(reinterpret_cast<ScriptGraph*>(GetOwner())->GetOwner());;
		}
		InternalMSG msg;
		msg.myEvent = InternalEvent::eCustomEvent;
		msg.myData.aMSG = aEvent.c_str();
		msg.myData.aExternalGameObject = actor;
		msg.myValue = 0;
		actor->SendInternal(msg);
	}
	return NoExec();
}

SGNode_CustomEventEntry::SGNode_CustomEventEntry()
{
	AddFlag(ScriptGraphNodeFlag_Compact);
	CreateDataPin<std::string>("Event", PinDirection::Input);
	CreateDataPin<std::string>("Event Name", PinDirection::Output);
}

std::string_view SGNode_CustomEventEntry::GetEntryHandle()
{
	std::string event;
	if(GetPinData("Event", event))
	{
		if(event.empty())
		{
			return "SGNode_CustomEvent";
		}
		myEntryHandle = event;
		return myEntryHandle;
	}
	return "SGNode_CustomEvent";
}

NodeResult SGNode_CustomEventEntry::DoOperation()
{
	std::string event;
	if(GetPinData("Event", event))
	{
		SetPinData("Event Name", event);
		return ExecPin("Out");
	}
	return NoExec();
}
