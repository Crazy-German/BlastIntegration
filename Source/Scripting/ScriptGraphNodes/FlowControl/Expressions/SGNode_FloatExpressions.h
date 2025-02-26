#pragma once
#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_FloatEquals, ScriptGraphNode)
{
public:
	SGNode_FloatEquals();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "=="; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "=="; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Flow Control|Compare|Float"; }
	NodeResult DoOperation() override;
};

DECLARE_GRAPH_NODE(SGNode_FloatNotEquals, ScriptGraphNode)
{
public:
	SGNode_FloatNotEquals();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "!="; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "!="; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Flow Control|Compare|Float"; }
	NodeResult DoOperation() override;
};

DECLARE_GRAPH_NODE(SGNode_FloatLessThan, ScriptGraphNode)
{
public:
	SGNode_FloatLessThan();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "<"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "<"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Flow Control|Compare|Float"; }
	NodeResult DoOperation() override;
};


DECLARE_GRAPH_NODE(SGNode_FloatMoreThan, ScriptGraphNode)
{
public:
	SGNode_FloatMoreThan();

	FORCEINLINE std::string_view GetNodeTitle() const override { return ">"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return ">"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Flow Control|Compare|Float"; }
	NodeResult DoOperation() override;
};


DECLARE_GRAPH_NODE(SGNode_FloatLessEqualThan, ScriptGraphNode)
{
public:
	SGNode_FloatLessEqualThan();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "<="; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "<="; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Flow Control|Compare|Float"; }
	NodeResult DoOperation() override;
};


DECLARE_GRAPH_NODE(SGNode_FloatMoreEqualThan, ScriptGraphNode)
{
public:
	SGNode_FloatMoreEqualThan();

	FORCEINLINE std::string_view GetNodeTitle() const override { return ">="; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return ">="; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Flow Control|Compare|Float"; }
	NodeResult DoOperation() override;
};
