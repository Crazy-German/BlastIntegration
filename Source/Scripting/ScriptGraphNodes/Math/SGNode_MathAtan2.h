#pragma once
#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_MathAtan2, ScriptGraphNode)
{
	public:
	SGNode_MathAtan2();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "Atan2"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "Atan2"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Math|Scalar"; }
	NodeResult DoOperation() override;
};