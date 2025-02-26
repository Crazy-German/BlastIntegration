#include "pch.h"
#include "SGNode_Vector3Sub.h"
#include "CommonUtilities/Vector3.hpp"

IMPLEMENT_GRAPH_NODE(SGNode_Vector3Sub, ScriptGraphNode);

SGNode_Vector3Sub::SGNode_Vector3Sub()
{
	AddFlag(ScriptGraphNodeFlag_Compact);

	CreateDataPin<CommonUtilities::Vector3<float>>("A", PinDirection::Input, true);
	CreateDataPin<CommonUtilities::Vector3<float>>("B", PinDirection::Input, true);
	CreateDataPin<CommonUtilities::Vector3<float>>("Result", PinDirection::Output, true);
}

NodeResult SGNode_Vector3Sub::DoOperation()
{
	CommonUtilities::Vector3<float> inVectOne;
	CommonUtilities::Vector3<float> inVectTwo;

	if (GetPinData("A", inVectOne) && GetPinData("B", inVectTwo))
	{
		SetPinData("Result", inVectOne-inVectTwo);
		return NoExec();
	}

	return Error("Something went wrong Vector Break!");
}