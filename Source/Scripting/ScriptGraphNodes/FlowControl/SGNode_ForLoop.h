#pragma once

#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_ForLoop, ScriptGraphNode)
{
public:
	SGNode_ForLoop();
	NodeResult Enter(size_t anEntryPinId, NodeEntryType aEntryType) override;
	FORCEINLINE std::string_view GetNodeTitle() const override { return "For Loop"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "For"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Flow Control"; }
	FORCEINLINE ScriptGraphNodeType GetNodeType() const override { return ScriptGraphNodeType::FlowControl; }

	NodeResult DoOperation() override;
private:
	int myIndex;
};
