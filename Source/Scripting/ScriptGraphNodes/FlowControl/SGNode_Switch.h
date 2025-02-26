#pragma once

#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_IntSwitch, ScriptGraphNode)
{
public:
	SGNode_IntSwitch();
	FORCEINLINE std::string_view GetNodeTitle() const override { return "IntSwitch"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "IntSwitch"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Flow Control"; }
	FORCEINLINE ScriptGraphNodeType GetNodeType() const override { return ScriptGraphNodeType::FlowControl; }

	NodeResult DoOperation() override;
	#ifndef NOEDITOR
	void OnUserAddedPin(ScriptGraphSchema* aSchema, size_t aPinId) override;
	void OnUserRemovedPin(ScriptGraphSchema* aSchema, size_t aPinId) override;
	private:
		int caseCount;
	#endif
};
//
//DECLARE_GRAPH_NODE(SGNode_EnumSwitch, ScriptGraphNode)
//{
//public:
//	SGNode_EnumSwitch();
//	FORCEINLINE std::string_view GetNodeTitle() const override { return "EnmSwitch"; }
//	FORCEINLINE std::string_view GetCompactTitle() const override { return "EnmSwitch"; }
//	FORCEINLINE std::string_view GetNodeCategory() const override { return "Flow Control"; }
//	FORCEINLINE ScriptGraphNodeType GetNodeType() const override { return ScriptGraphNodeType::FlowControl; }
//
//	NodeResult DoOperation() override;
//	#ifndef NOEDITOR
//	void OnUserAddedPin(ScriptGraphSchema* aSchema, size_t aPinId) override;
//	void OnUserRemovedPin(ScriptGraphSchema* aSchema, size_t aPinId) override;
//	private:
//		int caseCount;
//	#endif
//};