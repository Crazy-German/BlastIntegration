#pragma once

#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_CastFloatToString, ScriptGraphNode)
{
public:
	SGNode_CastFloatToString();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "Cast Float To String"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "Cast"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Casts"; }
	NodeResult DoOperation() override;
};

