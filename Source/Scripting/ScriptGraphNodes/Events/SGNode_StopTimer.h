#pragma once
#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_StopTimer, ScriptGraphNode)
{
public:
	SGNode_StopTimer();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "Stop Timer"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "Stop Timer"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Timer"; }

	NodeResult DoOperation() override;
};