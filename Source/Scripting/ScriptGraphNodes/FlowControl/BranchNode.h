#pragma once

#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_BranchNode, ScriptGraphNode)
{
public:
	SGNode_BranchNode();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "Branch"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "Branch"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Flow Control"; }
	FORCEINLINE ScriptGraphNodeType GetNodeType() const override { return ScriptGraphNodeType::FlowControl; }

	NodeResult DoOperation() override;
};

