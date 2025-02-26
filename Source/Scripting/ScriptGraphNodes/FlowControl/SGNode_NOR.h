#pragma once
#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_NOR, ScriptGraphNode)
{
public:
	SGNode_NOR();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "NOR"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "NOR"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Flow Control|Logic"; }
	NodeResult DoOperation() override;
};