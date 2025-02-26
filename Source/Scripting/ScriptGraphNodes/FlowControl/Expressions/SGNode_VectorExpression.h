#pragma once
#pragma once
#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_VectorEquals, ScriptGraphNode)
{
public:
	SGNode_VectorEquals();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "=="; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "=="; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Flow Control|Compare|Vector"; }
	NodeResult DoOperation() override;
};

DECLARE_GRAPH_NODE(SGNode_VectorNotEquals, ScriptGraphNode)
{
public:
	SGNode_VectorNotEquals();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "!="; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "!="; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Flow Control|Compare|Vector"; }
	NodeResult DoOperation() override;
};

DECLARE_GRAPH_NODE(SGNode_VectorLessThan, ScriptGraphNode)
{
public:
	SGNode_VectorLessThan();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "<"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "<"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Flow Control|Compare|Vector"; }
	NodeResult DoOperation() override;
};


DECLARE_GRAPH_NODE(SGNode_VectorMoreThan, ScriptGraphNode)
{
public:
	SGNode_VectorMoreThan();

	FORCEINLINE std::string_view GetNodeTitle() const override { return ">"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return ">"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Flow Control|Compare|Vector"; }
	NodeResult DoOperation() override;
};


DECLARE_GRAPH_NODE(SGNode_VectorLessEqualThan, ScriptGraphNode)
{
public:
	SGNode_VectorLessEqualThan();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "<="; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "<="; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Flow Control|Compare|Vector"; }
	NodeResult DoOperation() override;
};


DECLARE_GRAPH_NODE(SGNode_VectorMoreEqualThan, ScriptGraphNode)
{
public:
	SGNode_VectorMoreEqualThan();

	FORCEINLINE std::string_view GetNodeTitle() const override { return ">="; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return ">="; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Flow Control|Compare|Vector"; }
	NodeResult DoOperation() override;
};
