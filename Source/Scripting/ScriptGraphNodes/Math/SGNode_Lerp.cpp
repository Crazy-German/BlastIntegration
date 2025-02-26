#include "pch.h"
#include "SGNode_Lerp.h"

IMPLEMENT_GRAPH_NODE(SGNode_Lerp, ScriptGraphNode);

SGNode_Lerp::SGNode_Lerp()
{

	CreateDataPin<float>("A", PinDirection::Input);
	CreateDataPin<float>("B", PinDirection::Input);
	CreateDataPin<float>("Alpha", PinDirection::Input);
	CreateDataPin<float>("Return Value", PinDirection::Output);
}

NodeResult SGNode_Lerp::DoOperation()
{
	float inA = 0;
	float inAlpha = 0;
	float inB = 0;

	if (GetPinData("A", inA) && GetPinData("B", inB)&& GetPinData("Alpha", inAlpha))
	{
		float result = inA + (inB - inA) * inAlpha;
		SetPinData("Return Value", result);
		return NoExec();
	}
	return Error("Lerp error");
}
