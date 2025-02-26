#include "pch.h"
#include "SGNode_MathMax.h"

IMPLEMENT_GRAPH_NODE(SGNode_MathMax, ScriptGraphNode);

SGNode_MathMax::SGNode_MathMax()
{
	AddFlag(ScriptGraphNodeFlag_Compact);

	CreateDataPin<float>("A", PinDirection::Input, true);
	CreateDataPin<float>("B", PinDirection::Input, true);
	CreateDataPin<float>("Result", PinDirection::Output, true);
}

NodeResult SGNode_MathMax::DoOperation()
{
	float inA = 0;
	float inB = 0;

	if (GetPinData("A", inA) && GetPinData("B", inB))
	{
		const float result = inB<inA ? inA : inB;
		SetPinData("Result", result);
		return NoExec();
	}

	return Error("Something went wrong adding up A and B!");
}