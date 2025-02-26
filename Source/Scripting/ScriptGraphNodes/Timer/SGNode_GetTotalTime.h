#pragma once

DECLARE_GRAPH_NODE(SGNode_GetTotalTime, ScriptGraphNode)
{
public:
	SGNode_GetTotalTime();

	FORCEINLINE std::string_view GetNodeTitle() const override { return "GetTotalTime";};
	std::string_view GetCompactTitle() const override {return "TotalTime";};
	FORCEINLINE std::string_view GetNodeCategory() const override { return "Timer"; }
	NodeResult DoOperation() override;
};