#include "pch.h"
#include "SGNode_CastVector3ToString.h"
#include "../Utilities/CommonUtilities/Vector3.hpp"

IMPLEMENT_GRAPH_NODE(SGNode_CastVector3ToString, ScriptGraphNode)

SGNode_CastVector3ToString::SGNode_CastVector3ToString()
{
	AddFlag(ScriptGraphNodeFlag_Compact);

	CreateDataPin<CommonUtilities::Vector3<float>>("In", PinDirection::Input, true);
	CreateDataPin<std::string>("Out", PinDirection::Output, true);
}

NodeResult SGNode_CastVector3ToString::DoOperation()
{
	CommonUtilities::Vector3<float> input;
	if(GetPinData("In", input))
	{
		std::string result = "X: " + std::to_string(input.x) + " Y: " + std::to_string(input.y)+" Z: " + std::to_string(input.z);
		SetPinData("Out", result);
		return NoExec();
	}

	return Error("Could not read input!");
}
