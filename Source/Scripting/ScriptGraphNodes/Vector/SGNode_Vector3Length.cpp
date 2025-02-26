#include "pch.h"
#include "SGNode_Vector3Lengt.h"
#include "CommonUtilities/Vector3.hpp"

IMPLEMENT_GRAPH_NODE(SGNode_Vector3Length, ScriptGraphNode);

SGNode_Vector3Length::SGNode_Vector3Length()
{
	AddFlag(ScriptGraphNodeFlag_Compact);

	CreateDataPin<float>("Result", PinDirection::Output, true);
	CreateDataPin<CommonUtilities::Vector3<float>>("Input", PinDirection::Input, true);
}

NodeResult SGNode_Vector3Length::DoOperation()
{
	CommonUtilities::Vector3<float> inVect;

	if (GetPinData("Input", inVect))
	{
		SetPinData("Result", inVect.Length());
		return NoExec();
	}

	return Error("Something went wrong Vector Break!");
}
