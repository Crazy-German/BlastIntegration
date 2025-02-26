#pragma once

#include "../../MuninGraphEditor/Fonts/IconsFontAwesome6.h"
#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_MathMin, ScriptGraphNode)
{
public:
	SGNode_MathMin();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "Min"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "Min"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Math|Scalar"; }
	NodeResult DoOperation() override;
};