#pragma once

#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_While, ScriptGraphNode)
{
public:
	SGNode_While();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "While Loop"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "While"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Flow Control"; }
	FORCEINLINE ScriptGraphNodeType GetNodeType() const override { return ScriptGraphNodeType::FlowControl; }

	NodeResult DoOperation() override;
};