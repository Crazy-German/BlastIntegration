#pragma once
#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_OR, ScriptGraphNode)
{
public:
	SGNode_OR();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "OR"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "OR"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Flow Control|Logic"; }
	NodeResult DoOperation() override;
};

