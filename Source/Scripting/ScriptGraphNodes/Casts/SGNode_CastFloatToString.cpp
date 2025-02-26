#include "pch.h"
#include "SGNode_CastFloatToString.h"

IMPLEMENT_GRAPH_NODE(SGNode_CastFloatToString, ScriptGraphNode)

SGNode_CastFloatToString::SGNode_CastFloatToString()
{
	AddFlag(ScriptGraphNodeFlag_Compact);

	CreateDataPin<float>("In", PinDirection::Input, true);
	CreateDataPin<std::string>("Out", PinDirection::Output, true);
}

NodeResult SGNode_CastFloatToString::DoOperation()
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
