#include "pch.h"
#include "SGNode_MathCos.h"

IMPLEMENT_GRAPH_NODE(SGNode_MathCos, ScriptGraphNode);

SGNode_MathCos::SGNode_MathCos()
{
	AddFlag(ScriptGraphNodeFlag_Compact);

	CreateDataPin<float>("Value", PinDirection::Input, true);
	CreateDataPin<float>("Result", PinDirection::Output, true);
}

NodeResult SGNode_MathCos::DoOperation()
{
	float inVal = 0;

	if (GetPinData("Value", inVal))
	{
		const float result = std::cos(inVal);
		SetPinData("Result", result);
		return NoExec();
	}

	return Error("Something went wrong adding up A and B!");
}
