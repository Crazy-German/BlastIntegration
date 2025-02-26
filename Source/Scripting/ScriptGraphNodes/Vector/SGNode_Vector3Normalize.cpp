#include "pch.h"
#include "SGNode_Vector3Normalize.h"
#include "CommonUtilities/Vector3.hpp"

IMPLEMENT_GRAPH_NODE(SGNode_Vector3Normalize, ScriptGraphNode);

SGNode_Vector3Normalize::SGNode_Vector3Normalize()
{AddFlag(ScriptGraphNodeFlag_Compact);

	CreateDataPin<CommonUtilities::Vector3<float>>("Result", PinDirection::Output, true);
	CreateDataPin<CommonUtilities::Vector3<float>>("Input", PinDirection::Input, true);
}

NodeResult SGNode_Vector3Normalize::DoOperation()
{
	CommonUtilities::Vector3<float> inVect;

	if (GetPinData("Input", inVect))
	{
		inVect.Normalize();
		SetPinData("Result", inVect);
		return NoExec();
	}

	return Error("Something went wrong Vector Break!");
}