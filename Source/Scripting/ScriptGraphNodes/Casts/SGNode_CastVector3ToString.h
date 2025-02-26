#pragma once
#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_CastVector3ToString, ScriptGraphNode)
{
public:
	SGNode_CastVector3ToString();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "Cast Vector3 To String"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "Cast"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Casts"; }
	NodeResult DoOperation() override;
};

