#include "pch.h"
#include "SGNode_Vector3Cross.h"
#include "CommonUtilities/Vector3.hpp"

IMPLEMENT_GRAPH_NODE(SGNode_Vector3Cross, ScriptGraphNode);

SGNode_Vector3Cross::SGNode_Vector3Cross()
{
	AddFlag(ScriptGraphNodeFlag_Compact);

	CreateDataPin<CommonUtilities::Vector3<float>>("Result", PinDirection::Output, true);
	CreateDataPin<CommonUtilities::Vector3<float>>("FirstVector", PinDirection::Input, true);
	CreateDataPin<CommonUtilities::Vector3<float>>("SecondVector", PinDirection::Input, true);
}

NodeResult SGNode_Vector3Cross::DoOperation()
{
	CommonUtilities::Vector3<float> inVectOne;
	CommonUtilities::Vector3<float> inVectTwo;

	if (GetPinData("FirstVector", inVectOne) && GetPinData("SecondVector", inVectTwo))
	{
		SetPinData("Result", inVectOne.Cross(inVectTwo));
		return NoExec();
	}

	return Error("Something went wrong Vector Break!");
}
