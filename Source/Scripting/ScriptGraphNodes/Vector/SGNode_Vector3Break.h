#pragma once
#include "ScriptGraph/ScriptGraphNode.h"

DECLARE_GRAPH_NODE(SGNode_Vector3Break, ScriptGraphNode)
{
	public:
	SGNode_Vector3Break();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "Vec3 Break"; }
	FORCEINLINE std::string_view GetCompactTitle() const override { return "Break"; }
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Math|Vector3"; }
	NodeResult DoOperation() override;
};