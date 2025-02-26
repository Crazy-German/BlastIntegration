#pragma once
#include "ScriptGraph/ScriptGraphNode.h"
#include "ScriptGraph/Nodes/EventNodes.h"

DECLARE_GRAPH_NODE(SGNode_Tick, ScriptGraphEventNode)
{
public:
	SGNode_Tick();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "Tick"; }
	FORCEINLINE std::string_view GetEntryHandle() const override { return "Tick"; };
};
