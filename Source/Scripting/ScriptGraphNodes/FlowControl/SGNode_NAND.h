#pragma once

#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_NAND, ScriptGraphNode)
{
public:
	SGNode_NAND();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "NAND"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "NAND"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Flow Control|Logic"; }
	NodeResult DoOperation() override;
};

