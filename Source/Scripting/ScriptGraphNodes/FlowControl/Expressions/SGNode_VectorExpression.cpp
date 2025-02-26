#include "pch.h"
#include "SGNode_VectorExpression.h"

#include "CommonUtilities/Vector3.hpp"

IMPLEMENT_GRAPH_NODE(SGNode_VectorEquals, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(SGNode_VectorNotEquals, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(SGNode_VectorLessThan, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(SGNode_VectorMoreThan, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(SGNode_VectorLessEqualThan, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(SGNode_VectorMoreEqualThan, ScriptGraphNode);


SGNode_VectorEquals::SGNode_VectorEquals()
{
	AddFlag(ScriptGraphNodeFlag_Compact);
	CreateDataPin<CommonUtilities::Vector3<float>>("Value##1", PinDirection::Input, true);
	CreateDataPin<CommonUtilities::Vector3<float>>("Value##2", PinDirection::Input, true);
	CreateDataPin<bool>("Output", PinDirection::Output, true);
}

NodeResult SGNode_VectorEquals::DoOperation()
{
	CommonUtilities::Vector3<float> value1;
	CommonUtilities::Vector3<float> value2;
	SetPinData("Output", false);
	if(GetPinData("Value##1", value1) && GetPinData("Value##2", value2))
	{
		if(value1.x == value2.x || value1.y == value2.y || value1.z == value2.z)
		{
			SetPinData("Output", true);
		}
	}
	return NoExec();
}

SGNode_VectorNotEquals::SGNode_VectorNotEquals()
{
	AddFlag(ScriptGraphNodeFlag_Compact);

	CreateDataPin<CommonUtilities::Vector3<float>>("Value##1", PinDirection::Input, true);
	CreateDataPin<CommonUtilities::Vector3<float>>("Value##2", PinDirection::Input, true);
	CreateDataPin<bool>("Output", PinDirection::Output, true);
}

NodeResult SGNode_VectorNotEquals::DoOperation()
{
	CommonUtilities::Vector3<float> value1;
	CommonUtilities::Vector3<float> value2;
	SetPinData("Output", false);
	if(GetPinData("Value##1", value1) && GetPinData("Value##2", value2))
	{
		if(value1.x != value2.x || value1.y != value2.y || value1.z != value2.z)
		{
			SetPinData("Output", true);
		}
	}
	return NoExec();
}

SGNode_VectorLessThan::SGNode_VectorLessThan()
{
	AddFlag(ScriptGraphNodeFlag_Compact);
	CreateDataPin<CommonUtilities::Vector3<float>>("Value##1", PinDirection::Input, true);
	CreateDataPin<CommonUtilities::Vector3<float>>("Value##2", PinDirection::Input, true);
	CreateDataPin<bool>("Output", PinDirection::Output, true);
}

NodeResult SGNode_VectorLessThan::DoOperation()
{
	CommonUtilities::Vector3<float> value1;
	CommonUtilities::Vector3<float> value2;
	SetPinData("Output", false);
	if(GetPinData("Value##1", value1) && GetPinData("Value##2", value2))
	{
		if(value1.LengthSqr() < value2.LengthSqr())
		{
			SetPinData("Output", true);
		}
	}
	return NoExec();
}

SGNode_VectorMoreThan::SGNode_VectorMoreThan()
{
	AddFlag(ScriptGraphNodeFlag_Compact);
	CreateDataPin<CommonUtilities::Vector3<float>>("Value##1", PinDirection::Input, true);
	CreateDataPin<CommonUtilities::Vector3<float>>("Value##2", PinDirection::Input, true);
	CreateDataPin<bool>("Output", PinDirection::Output, true);
}

NodeResult SGNode_VectorMoreThan::DoOperation()
{
	CommonUtilities::Vector3<float> value1;
	CommonUtilities::Vector3<float> value2;
	SetPinData("Output", false);
	if(GetPinData("Value##1", value1) && GetPinData("Value##2", value2))
	{
		if(value1.LengthSqr() > value2.LengthSqr())
		{
			SetPinData("Output", true);
		}
	}
	return NoExec();
}

SGNode_VectorLessEqualThan::SGNode_VectorLessEqualThan()
{
	AddFlag(ScriptGraphNodeFlag_Compact);
	CreateDataPin<CommonUtilities::Vector3<float>>("Value##1", PinDirection::Input, true);
	CreateDataPin<CommonUtilities::Vector3<float>>("Value##2", PinDirection::Input, true);
	CreateDataPin<bool>("Output", PinDirection::Output, true);
}

NodeResult SGNode_VectorLessEqualThan::DoOperation()
{
	CommonUtilities::Vector3<float> value1;
	CommonUtilities::Vector3<float> value2;
	SetPinData("Output", false);
	if(GetPinData("Value##1", value1) && GetPinData("Value##2", value2))
	{
		if(value1.LengthSqr() <= value2.LengthSqr())
		{
			SetPinData("Output", true);
		}
	}
	return NoExec();
}

SGNode_VectorMoreEqualThan::SGNode_VectorMoreEqualThan()
{
	AddFlag(ScriptGraphNodeFlag_Compact);
	CreateDataPin<CommonUtilities::Vector3<float>>("Value##1", PinDirection::Input, true);
	CreateDataPin<CommonUtilities::Vector3<float>>("Value##2", PinDirection::Input, true);
	CreateDataPin<bool>("Output", PinDirection::Output, true);
}

NodeResult SGNode_VectorMoreEqualThan::DoOperation()
{
	CommonUtilities::Vector3<float> value1;
	CommonUtilities::Vector3<float> value2;
	SetPinData("Output", false);
	if(GetPinData("Value##1", value1) && GetPinData("Value##2", value2))
	{
		if(value1.LengthSqr() >= value2.LengthSqr())
		{
			SetPinData("Output", true);
		}
	}
	return NoExec();
}
