#pragma once

#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_MathMul, ScriptGraphNode)
{
public:
	SGNode_MathMul();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "Mul"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "*"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Math|Scalar"; }
	NodeResult DoOperation() override;
};