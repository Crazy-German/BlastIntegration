#include "pch.h"
#include "SGNode_MathMin.h"
IMPLEMENT_GRAPH_NODE(SGNode_MathMin, ScriptGraphNode);

SGNode_MathMin::SGNode_MathMin()
{
	AddFlag(ScriptGraphNodeFlag_Compact);

	CreateDataPin<float>("A", PinDirection::Input, true);
	CreateDataPin<float>("B", PinDirection::Input, true);
	CreateDataPin<float>("Result", PinDirection::Output, true);
}

NodeResult SGNode_MathMin::DoOperation()
{
	float inA = 0;
	float inB = 0;

	if (GetPinData("A", inA) && GetPinData("B", inB))
	{
		const float result = inA<inB ? inA : inB;
		SetPinData("Result", result);
		return NoExec();
	}

	return Error("Something went wrong adding up A and B!");
}