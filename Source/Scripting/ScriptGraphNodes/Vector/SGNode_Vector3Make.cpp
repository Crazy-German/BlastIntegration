#include "pch.h"
#include "SGNode_Vector3Make.h"

#include "CommonUtilities/Vector3.hpp"

IMPLEMENT_GRAPH_NODE(SGNode_Vector3Make, ScriptGraphNode);

SGNode_Vector3Make::SGNode_Vector3Make()
{
	AddFlag(ScriptGraphNodeFlag_Compact);

	CreateDataPin<CommonUtilities::Vector3<float>>("Result", PinDirection::Output, true);
	CreateDataPin<float>("X", PinDirection::Input, true);
	CreateDataPin<float>("Y", PinDirection::Input, true);
	CreateDataPin<float>("Z", PinDirection::Input, true);
}

NodeResult SGNode_Vector3Make::DoOperation()
{
	CommonUtilities::Vector3<float> outVect;
	float inX = 0;
	float inY = 0;
	float inZ = 0;

	if (GetPinData("X", inX) && GetPinData("Y", inY) && GetPinData("Z", inZ))
	{
		outVect = {inX,inY,inZ};
		SetPinData("Result", outVect);
		return NoExec();
	}

	return Error("Something went wrong Vector Break!");
}
