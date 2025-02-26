#include "pch.h"
#include "SGNode_MathAdd.h"

IMPLEMENT_GRAPH_NODE(SGNode_MathAdd, ScriptGraphNode);

SGNode_MathAdd::SGNode_MathAdd()
{
	AddFlag(ScriptGraphNodeFlag_Compact);

	CreateDataPin<float>("A", PinDirection::Input, true);
	CreateDataPin<float>("B", PinDirection::Input, true);
	CreateDataPin<float>("Result", PinDirection::Output, true);
}

NodeResult SGNode_MathAdd::DoOperation()
{
	float inA = 0;
	float inB = 0;

	if (GetPinData("A", inA) && GetPinData("B", inB))
	{
		const float result = inA + inB;
		SetPinData("Result", result);
		return NoExec();
	}

	return Error("Something went wrong adding up A and B!");
}