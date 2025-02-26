#pragma once
#include "ScriptGraph/ScriptGraphNode.h"
#include "ScriptGraph/ScriptGraphCommon.h"

class ScriptGraphTimerEventNode : public ScriptGraphNode
{
public:
	ScriptGraphTimerEventNode();

	std::string_view GetNodeCategory() const override{return "Events|Timers";}
	ScriptGraphNodeType GetNodeType() const override{return ScriptGraphNodeType::TimerEvent;}
	NodeResult DoOperation() override;

	virtual FORCEINLINE std::string_view GetEntryHandle() { return myEntryHandle; }
	const std::vector<std::type_index>& GetSupportedSchemas() const override;
protected:
	std::string myEntryHandle;
};

