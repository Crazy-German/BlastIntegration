#pragma once
#include "ScriptGraph/ScriptGraphNode.h"
#include "ScriptGraph/Nodes/EventNodes.h"

DECLARE_GRAPH_NODE(SGNode_OnTouchFound, ScriptGraphEventNode)
{
public:
	SGNode_OnTouchFound();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "BeginOverlap"; }
	FORCEINLINE std::string_view GetEntryHandle() const override{return "BeginOverlap";}
	FORCEINLINE std::string_view GetNodeCategory() const override{return "Events|Collision";}
	
};

DECLARE_GRAPH_NODE(SGNode_OnTouchLost, ScriptGraphEventNode)
{
public:
	SGNode_OnTouchLost();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "EndOverlap"; }
	FORCEINLINE std::string_view GetEntryHandle() const override{return "EndOverlap";}
	FORCEINLINE std::string_view GetNodeCategory() const override{return "Events|Collision";}
	
};