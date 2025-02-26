#include "ScriptComponent.h"

#include "AssetManagement/AssetTypes/ScriptAsset.h"
#include <ranges>
#include "Scripting/ScriptGraphNodes/ScriptGraphTypes.h"
#include "Scripting/ScriptGraphNodes/ScriptGraphNodes.h"
#include "Types/TypeRegistry.h"


ScriptComponent::ScriptComponent()
{
}

ScriptComponent::~ScriptComponent()
{
	for (auto& script : myScripts | std::views::values) // Safety if destroy was not called
	{
		delete script;
	}
	myScripts.clear();
}

void ScriptComponent::Awake()
{
	for(auto& script : myScripts | std::views::values)
	{
		script->Execute("Begin Play");
	}
}

void ScriptComponent::Update(const float& aDeltaTime)
{
	ScriptGraphPayload payload;
	payload.SetPinValue("Tick", aDeltaTime);
	for(auto& script : myScripts | std::views::values)
	{
		script->ExecuteWithPayload("Tick", payload);
		script->ExecuteInput();
	}
}

void ScriptComponent::Destroy()
{
	for (auto& script : myScripts | std::views::values)
	{
		delete script;
	}
	myScripts.clear();
}

void ScriptComponent::RecieveInternal(InternalMSG aEvent)
{
	ScriptGraphPayload payload;
	switch (aEvent.myEvent)
	{
	case InternalEvent::eHit:
		break;
	case InternalEvent::eDamage:
		break;
	case InternalEvent::eTouchFound:
		payload.SetPinValue("Other Actor", aEvent.myData.aExternalGameObject);
		ExecuteScriptsWithPayload("BeginOverlap", payload);
		break;
	case InternalEvent::eTouchLost:
		payload.SetPinValue("Other Actor", aEvent.myData.aExternalGameObject);
		ExecuteScriptsWithPayload("EndOverlap", payload);
		break;
	case InternalEvent::eCustomEvent:
		ExecuteScripts(aEvent.myData.aMSG);
		break;
	default:
		break;
	}
}

unsigned ScriptComponent::AddScript(std::shared_ptr<ScriptAsset> aScript)
{
	MuninGraph::Get().Initialize();
	myScripts.emplace(myScriptId, new ScriptGraph(myGameObject));
	myScripts.at(myScriptId)->Deserialize(*aScript->myScriptData);
	myScripts.at(myScriptId++)->Execute("Begin Play");
	return myScriptId-1;
}

void ScriptComponent::ExecuteScript(unsigned aId, const std::string& aExecutionPoint) const
{
	if(myScripts.contains(aId))
	{
		myScripts.at(aId)->Execute(aExecutionPoint);
	}
}

void ScriptComponent::ExecuteScripts(const std::string& anExecutionPoint) const
{
	for(auto& script : myScripts | std::views::values)
	{
		script->Execute(anExecutionPoint);
	}
}

void ScriptComponent::ExecuteScriptWithPayload(unsigned aId, const std::string& aExecutionPoint,
	const ScriptGraphPayload& aPayload) const
{
	if(aId <myScripts.size())
	{
		myScripts.at(aId)->ExecuteWithPayload(aExecutionPoint, aPayload);
	}
}

void ScriptComponent::ExecuteScriptsWithPayload(const std::string& aExecutionPoint, const ScriptGraphPayload& aPayload) const
{
	for(auto& script : myScripts | std::views::values)
	{
		script->ExecuteWithPayload(aExecutionPoint, aPayload);
	}
}
#ifndef NOEDITOR
void ScriptComponent::SetScript(ScriptGraph* aGraph)
{
	myScripts.emplace(0,aGraph);
}
#endif