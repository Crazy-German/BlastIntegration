#pragma once

#include "ScriptGraph/ScriptGraphNode.h"

#define DECLARE_NODE(NAME) \
DECLARE_GRAPH_NODE(SGNode_##NAME, ScriptGraphNode) \
{ \
public: \
	SGNode_##NAME(); \
\
	FORCEINLINE std::string_view GetNodeTitle() const override { return #NAME; } \
	FORCEINLINE std::string_view GetCompactTitle() const override { return #NAME; } \
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Object|Transform"; } \
	NodeResult DoOperation() override; \
};

DECLARE_NODE(GetPosition)
DECLARE_NODE(SetPosition)
DECLARE_NODE(AddPosition)
DECLARE_NODE(GetRotation)
DECLARE_NODE(SetRotation)
DECLARE_NODE(AddRotation)