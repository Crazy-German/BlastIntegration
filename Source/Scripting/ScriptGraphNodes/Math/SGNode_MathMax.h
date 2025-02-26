#pragma once

#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_MathMax, ScriptGraphNode)
{
public:
	SGNode_MathMax();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "Max"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "Max"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Math|Scalar"; }
	NodeResult DoOperation() override;
};