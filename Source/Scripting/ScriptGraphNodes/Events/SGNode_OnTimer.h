#pragma once
#include "ScriptGraph/Nodes/TimerEventNode.h"

DECLARE_GRAPH_NODE(SGNode_OnTimer, ScriptGraphTimerEventNode)
{
public:
	SGNode_OnTimer();

	FORCEINLINE std::string_view GetNodeTitle() const override{return "OnTimer";}
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Timer"; }
	//FORCEINLINE ScriptGraphNodeType GetNodeType() const override { return ScriptGraphNodeType::Event; }

	std::string_view GetEntryHandle() override;
private:
	std::string myEntryHandle;
};