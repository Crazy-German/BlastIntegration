#include "pch.h"
#include "SGNode_MathSin.h"

IMPLEMENT_GRAPH_NODE(SGNode_MathSin, ScriptGraphNode);

SGNode_MathSin::SGNode_MathSin()
{
	AddFlag(ScriptGraphNodeFlag_Compact);

	CreateDataPin<float>("Value", PinDirection::Input, true);
	CreateDataPin<float>("Result", PinDirection::Output, true);
}

NodeResult SGNode_MathSin::DoOperation()
{
	float inVal = 0;

	if (GetPinData("Value", inVal))
	{
		const float result = std::sin(inVal);
		SetPinData("Result", result);
		return NoExec();
	}

	return Error("Something went wrong adding up A and B!");
}
