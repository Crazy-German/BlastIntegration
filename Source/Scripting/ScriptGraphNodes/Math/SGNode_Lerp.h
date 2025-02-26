#pragma once
#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_Lerp, ScriptGraphNode)
{
public:
	SGNode_Lerp();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "Lerp"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "Lerp"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Math|Scalar"; }
	FORCEINLINE ScriptGraphNodeType GetNodeType() const override { return ScriptGraphNodeType::Standard; }
	
	NodeResult DoOperation() override;
};