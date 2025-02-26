#pragma once

#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_MathSubb, ScriptGraphNode)
{
public:
	SGNode_MathSubb();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "Sub"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "-"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Math|Scalar"; }
	NodeResult DoOperation() override;
};