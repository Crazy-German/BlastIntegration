#pragma once
#include "ScriptGraph/ScriptGraph.h"
#include "ScriptGraph/Nodes/TimerEventNode.h"

DECLARE_GRAPH_NODE(SGNode_Timeline, ScriptGraphTimerEventNode)
{
public:
	SGNode_Timeline();
	NodeResult Enter(size_t anEntryPinId, NodeEntryType aEntryType) override;

	FORCEINLINE std::string_view GetNodeTitle() const override { return "Timeline"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "Timeline"; }
	FORCEINLINE std::string_view GetNodeCategory() const override{return "Timer";}
	FORCEINLINE ScriptGraphNodeType GetNodeType() const override{return ScriptGraphNodeType::TimerEvent;}
	std::string GetEntryHandle() const;

	NodeResult DoOperation() override;
	std::string Update();
	std::string Complete();
private:
	float myCurrentTime;
	unsigned myTimerUpdateID;
	unsigned myDoneUpdateID;
};