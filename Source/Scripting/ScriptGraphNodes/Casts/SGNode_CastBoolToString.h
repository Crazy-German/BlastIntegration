#pragma once
#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_CastBoolToString, ScriptGraphNode)
{
public:
	SGNode_CastBoolToString();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "Cast Bool To String"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "Cast"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Casts"; }
	NodeResult DoOperation() override;
};

