#include "pch.h"
#include "SGNode_Vector3ScalarMul.h"
#include "CommonUtilities/Vector3.hpp"

IMPLEMENT_GRAPH_NODE(SGNode_Vector3ScalarMul, ScriptGraphNode);

SGNode_Vector3ScalarMul::SGNode_Vector3ScalarMul()
{
	AddFlag(ScriptGraphNodeFlag_Compact);

	CreateDataPin<CommonUtilities::Vector3<float>>("Vector", PinDirection::Input, true);
	CreateDataPin<float>("Scalar", PinDirection::Input, true);
	CreateDataPin<CommonUtilities::Vector3<float>>("Result", PinDirection::Output, true);
}

NodeResult SGNode_Vector3ScalarMul::DoOperation()
{
	CommonUtilities::Vector3<float> inVect;
	float inScalar;

	if (GetPinData("Vector", inVect) && GetPinData("Scalar", inScalar))
	{
		inVect*=inScalar;
		SetPinData("Result", inVect);
		return NoExec();
	}

	return Error("Something went wrong Vector Break!");
}
