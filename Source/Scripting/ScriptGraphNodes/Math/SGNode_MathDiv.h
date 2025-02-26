#pragma once

#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_MathDiv, ScriptGraphNode)
{
public:
	SGNode_MathDiv();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "Div"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "/"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Math|Scalar"; }
	NodeResult DoOperation() override;
};