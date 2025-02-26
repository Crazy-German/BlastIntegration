#include "pch.h"
#include "SGNode_MathAbs.h"

IMPLEMENT_GRAPH_NODE(SGNode_MathAbs, ScriptGraphNode);

SGNode_MathAbs::SGNode_MathAbs()
{
	AddFlag(ScriptGraphNodeFlag_Compact);

	CreateDataPin<float>("Value", PinDirection::Input, true);
	CreateDataPin<float>("Result", PinDirection::Output, true);
}

NodeResult SGNode_MathAbs::DoOperation()
{
	float inVal = 0;

	if (GetPinData("Value", inVal))
	{
		if(inVal<0 )
		{
			inVal*=-1;
		}
		SetPinData("Result", inVal);
		return NoExec();
	}

	return Error("Something went wrong adding up A and B!");
}
