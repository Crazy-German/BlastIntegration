#pragma once
#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_Vector3Sub, ScriptGraphNode)
{
	public:
	SGNode_Vector3Sub();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "Vec3_Sub"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "-"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Math|Vector3"; }
	NodeResult DoOperation() override;
};