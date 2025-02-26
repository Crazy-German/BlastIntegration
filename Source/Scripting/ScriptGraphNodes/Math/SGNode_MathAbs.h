#pragma once
//#include "ScriptGraph/ScriptGraphNode.h"
#include "../../MuninGraph/ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_MathAbs, ScriptGraphNode)
{
	public:
	SGNode_MathAbs();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "Abs"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "| |"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Math|Scalar"; }
	NodeResult DoOperation() override;
};