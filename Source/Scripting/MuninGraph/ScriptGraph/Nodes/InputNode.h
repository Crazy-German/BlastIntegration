#pragma once
#include "ScriptGraph/ScriptGraphCommon.h"
#include "ScriptGraph/ScriptGraphNode.h"

class ScriptGraphInputNode : public ScriptGraphNode
{
public:
	ScriptGraphInputNode();
	
	NodeResult DoOperation() override final;

	FORCEINLINE std::string_view GetNodeCategory() const override final { return "Events|Input"; }
	FORCEINLINE ScriptGraphNodeType GetNodeType() const override { return ScriptGraphNodeType::Event; }
	const std::vector<std::type_index>& GetSupportedSchemas() const override;

	/**
	 * \brief Returns the Entry Handle to use for this node, i.e. what to give to ScriptGraph::Execute.
	 *		  By default it returns the base name of the node. For SGNode_MyEvent this is "MyEvent".
	 */
	virtual FORCEINLINE std::string_view GetEntryHandle() const { return ""; } 
protected:
	int myKey;
};