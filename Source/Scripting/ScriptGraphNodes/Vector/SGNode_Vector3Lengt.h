#pragma once
#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_Vector3Length, ScriptGraphNode)
{
	public:
	SGNode_Vector3Length();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "Vec3_Length"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "|| ||"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Math|Vector3"; }
	NodeResult DoOperation() override;
};