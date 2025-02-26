#pragma once

#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_AND, ScriptGraphNode)
{
public:
	SGNode_AND();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "AND"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "AND"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Flow Control|Logic"; }
	NodeResult DoOperation() override;
};

