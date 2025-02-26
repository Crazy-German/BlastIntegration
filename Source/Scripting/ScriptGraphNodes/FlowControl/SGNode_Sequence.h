#pragma once
#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_Sequence, ScriptGraphNode)
{
public:
	SGNode_Sequence();

	NodeResult Enter(size_t anEntryPinId, NodeEntryType aEntryType) override;
	
	FORCEINLINE std::string_view GetNodeTitle() const override { return "Sequence"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "Sequence"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Flow Control"; }
	FORCEINLINE ScriptGraphNodeType GetNodeType() const override { return ScriptGraphNodeType::FlowControl; }

	NodeResult DoOperation() override;
#ifndef NOEDITOR
	void OnUserAddedPin(ScriptGraphSchema* aSchema, size_t aPinId) override;
	void OnUserRemovedPin(ScriptGraphSchema* aSchema, size_t aPinId) override;
#endif
private:
	int myIndex;
	int myNodeCount;
};