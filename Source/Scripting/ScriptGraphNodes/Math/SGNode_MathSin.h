#pragma once
#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_MathSin, ScriptGraphNode)
{
	public:
	SGNode_MathSin();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "Sin"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "Sin"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Math|Scalar"; }
	NodeResult DoOperation() override;
};