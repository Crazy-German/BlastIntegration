#pragma once
#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_Vector3ScalarMul, ScriptGraphNode)
{
	public:
	SGNode_Vector3ScalarMul();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "Vec3_ScalarMul"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "*"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Math|Vector3"; }
	NodeResult DoOperation() override;
};