#pragma once
#include "../../Scripting/MuninGraph/ScriptGraph/ScriptGraph.h"
#include "../../Scripting/MuninGraph/ScriptGraph/ScriptGraphNode.h"
#include "../../Scripting/MuninGraph/ScriptGraph/ScriptGraphEdge.h"
#include "../../Scripting/MuninGraph/ScriptGraph/ScriptGraphSchema.h"
#include "AssetManagement/AssetTypes/ScriptAsset.h"


#define SCRIPT_COMPONENT 25000

class ScriptComponent : public Component
{
public:
	ScriptComponent();
	~ScriptComponent();
	void Awake() override;
	void Update(const float& aDeltaTime) override;
	void Destroy() override;
	void RecieveInternal(InternalMSG aEvent) override;
	unsigned AddScript(std::shared_ptr<ScriptAsset> aScript);
	void ExecuteScript(unsigned aId, const std::string& aExecutionPoint) const;
	void ExecuteScripts(const std::string& anExecutionPoint) const;
	
	void ExecuteScriptWithPayload(unsigned aId, const std::string& aExecutionPoint, const ScriptGraphPayload& aPayload) const;
	void ExecuteScriptsWithPayload(const std::string& aExecutionPoint, const ScriptGraphPayload& aPayload) const;
#ifndef NOEDITOR
	void SetScript(ScriptGraph* aGraph);
#endif
private:
	std::unordered_map<unsigned, ScriptGraph*> myScripts;
	unsigned myScriptId;
};
