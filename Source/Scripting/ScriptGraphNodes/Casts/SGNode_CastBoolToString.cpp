#include "pch.h"
#include "SGNode_CastBoolToString.h"

IMPLEMENT_GRAPH_NODE(SGNode_CastBoolToString, ScriptGraphNode)

SGNode_CastBoolToString::SGNode_CastBoolToString()
{
	AddFlag(ScriptGraphNodeFlag_Compact);

	CreateDataPin<bool>("In", PinDirection::Input, true);
	CreateDataPin<std::string>("Out", PinDirection::Output, true);
}

NodeResult SGNode_CastBoolToString::DoOperation()
{
	float input = 0;
	if(GetPinData("In", input))
	{
		std::string result = std::to_string(input);
		SetPinData("Out", result);
		return NoExec();
	}

	return Error("Could not read input!");
}
