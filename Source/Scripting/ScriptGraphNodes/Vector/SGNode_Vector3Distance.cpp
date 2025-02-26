#include "pch.h"
#include "SGNode_Vector3Distance.h"
#include "CommonUtilities/Vector3.hpp"

IMPLEMENT_GRAPH_NODE(SGNode_Vector3Distance, ScriptGraphNode);

SGNode_Vector3Distance::SGNode_Vector3Distance()
{
	AddFlag(ScriptGraphNodeFlag_Compact);

	CreateDataPin<float>("Result", PinDirection::Output, true);
	CreateDataPin<CommonUtilities::Vector3<float>>("FirstVector", PinDirection::Input, true);
	CreateDataPin<CommonUtilities::Vector3<float>>("SecondVector", PinDirection::Input, true);
}

NodeResult SGNode_Vector3Distance::DoOperation()
{
	CommonUtilities::Vector3<float> inVectOne;
	CommonUtilities::Vector3<float> inVectTwo;

	if (GetPinData("FirstVector", inVectOne) && GetPinData("SecondVector", inVectTwo))
	{
		SetPinData("Result", (inVectOne-inVectTwo).Length());
		return NoExec();
	}

	return Error("Something went wrong Vector Break!");
}