#pragma once
#include "TimerManager.h"
#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_Timer, ScriptGraphNode)
{
public:
	SGNode_Timer();

	NodeResult Enter(size_t anEntryPinId, NodeEntryType aEntryType) override;
	
	FORCEINLINE std::string_view GetNodeTitle() const override { return "Timer"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "Timer"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Timer"; }
	
	//FORCEINLINE ScriptGraphNodeType GetNodeType() const override { return ScriptGraphNodeType::Event; }
	NodeResult DoOperation() override;
	std::string Completed();
private:
	bool myTimer;
	bool wasAdded;
	unsigned myId;
};