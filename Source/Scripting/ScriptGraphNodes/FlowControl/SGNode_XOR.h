#pragma once

#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_XOR, ScriptGraphNode)
{
public:
	SGNode_XOR();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "XOR"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "XOR"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Flow Control|Logic"; }
	NodeResult DoOperation() override;
};

