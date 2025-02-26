#pragma once
#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_GetActor, ScriptGraphNode)
{
	public:
	SGNode_GetActor();
	FORCEINLINE std::string_view GetNodeTitle() const override { return "GetActor"; } 
	FORCEINLINE std::string_view GetCompactTitle() const override { return "GetActor"; }
	FORCEINLINE std::string_view GetNodeCategory() const override {return "Object";}

	NodeResult DoOperation() override;
};