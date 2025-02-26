#include "pch.h"
#include "SGNode_Vector3Break.h"

#include "CommonUtilities/Vector3.hpp"

IMPLEMENT_GRAPH_NODE(SGNode_Vector3Break, ScriptGraphNode);

SGNode_Vector3Break::SGNode_Vector3Break()
{
	AddFlag(ScriptGraphNodeFlag_Compact);

	CreateDataPin<CommonUtilities::Vector3<float>>("Input", PinDirection::Input, true);
	CreateDataPin<float>("X", PinDirection::Output, true);
	CreateDataPin<float>("Y", PinDirection::Output, true);
	CreateDataPin<float>("Z", PinDirection::Output, true);
}

NodeResult SGNode_Vector3Break::DoOperation()
{
	CommonUtilities::Vector3<float> inVect;

	if (GetPinData("Input", inVect))
	{
		SetPinData("X", inVect.x);
		SetPinData("Y", inVect.y);
		SetPinData("Z", inVect.z);
		return NoExec();
	}

	return Error("Something went wrong Vector Break!");
}
