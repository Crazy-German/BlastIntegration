#pragma once
#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_Vector3Make, ScriptGraphNode)
{
	public:
	SGNode_Vector3Make();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "Vec3 Make"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "Make"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Math|Vector3"; }
	NodeResult DoOperation() override;
};