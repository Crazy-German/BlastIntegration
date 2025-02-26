#pragma once
#include "ScriptGraph/Nodes/TimerEventNode.h"

DECLARE_GRAPH_NODE(SGNode_StartCustomEvent, ScriptGraphTimerEventNode)
{
public:
	SGNode_StartCustomEvent();
	FORCEINLINE std::string_view GetCompactTitle() const override{return "StartCustomEvent";}
	FORCEINLINE std::string_view GetNodeTitle() const override{return "Custom Event";}
	FORCEINLINE std::string_view GetNodeCategory() const override {return "Events|Custom";}
	FORCEINLINE ScriptGraphNodeType GetNodeType() const override { return ScriptGraphNodeType::CustomEvent; }
	FORCEINLINE std::string_view GetEntryHandle() override { return "No Entry";}

	NodeResult DoOperation() override;
};
DECLARE_GRAPH_NODE(SGNode_CustomEventEntry, ScriptGraphTimerEventNode)
{
public:
	SGNode_CustomEventEntry();
	FORCEINLINE std::string_view GetNodeTitle() const override{return "Custom Event Entry";}
	FORCEINLINE std::string_view GetCompactTitle() const override{return "Entry";}
	FORCEINLINE std::string_view GetNodeCategory() const override {return "Events|Custom";}
	FORCEINLINE ScriptGraphNodeType GetNodeType() const override { return ScriptGraphNodeType::CustomEvent; }
	std::string_view GetEntryHandle() override;
	NodeResult DoOperation() override;
};

