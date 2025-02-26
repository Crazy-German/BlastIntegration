#pragma once
#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_MathCos, ScriptGraphNode)
{
	public:
	SGNode_MathCos();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "Cos"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "Cos"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Math|Scalar"; }
	NodeResult DoOperation() override;
};