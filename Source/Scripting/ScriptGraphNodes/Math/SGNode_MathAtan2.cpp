#include "pch.h"
#include "SGNode_MathAtan2.h"
IMPLEMENT_GRAPH_NODE(SGNode_MathAtan2, ScriptGraphNode);

SGNode_MathAtan2::SGNode_MathAtan2()
{
	AddFlag(ScriptGraphNodeFlag_Compact);

	CreateDataPin<float>("X", PinDirection::Input, true);
	CreateDataPin<float>("Y", PinDirection::Input, true);
	CreateDataPin<float>("Result", PinDirection::Output, true);
}

NodeResult SGNode_MathAtan2::DoOperation()
{
	float inX = 0;
	float inY = 0;

	if (GetPinData("X", inX) && GetPinData("Y", inY))
	{
		const float result = std::atan2(inX, inY);
		SetPinData("Result", result);
		return NoExec();
	}

	return Error("Something went wrong adding up A and B!");
}