#include "pch.h"
#include "SGNode_FloatExpressions.h"

IMPLEMENT_GRAPH_NODE(SGNode_FloatEquals, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(SGNode_FloatNotEquals, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(SGNode_FloatLessThan, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(SGNode_FloatMoreThan, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(SGNode_FloatLessEqualThan, ScriptGraphNode);
IMPLEMENT_GRAPH_NODE(SGNode_FloatMoreEqualThan, ScriptGraphNode);


SGNode_FloatEquals::SGNode_FloatEquals()
{
	AddFlag(ScriptGraphNodeFlag_Compact);
	CreateDataPin<float>("Value##1", PinDirection::Input, true);
	CreateDataPin<float>("Value##2", PinDirection::Input, true);
	CreateDataPin<bool>("Output", PinDirection::Output, true);
}

NodeResult SGNode_FloatEquals::DoOperation()
{
	float value1;
	float value2;
	SetPinData("Output", false);
	if(GetPinData("Value##1", value1) && GetPinData("Value##2", value2))
	{
		if(value1 == value2)
		{
			SetPinData("Output", true);
		}
	}
	return NoExec();
}

SGNode_FloatNotEquals::SGNode_FloatNotEquals()
{
	AddFlag(ScriptGraphNodeFlag_Compact);

	CreateDataPin<float>("Value##1", PinDirection::Input, true);
	CreateDataPin<float>("Value##2", PinDirection::Input, true);
	CreateDataPin<bool>("Output", PinDirection::Output, true);
}

NodeResult SGNode_FloatNotEquals::DoOperation()
{
	float value1;
	float value2;
	SetPinData("Output", false);
	if(GetPinData("Value##1", value1) && GetPinData("Value##2", value2))
	{
		if(value1 != value2)
		{
			SetPinData("Output", true);
		}
	}
	return NoExec();
}

SGNode_FloatLessThan::SGNode_FloatLessThan()
{
	AddFlag(ScriptGraphNodeFlag_Compact);
	CreateDataPin<float>("Value##1", PinDirection::Input, true);
	CreateDataPin<float>("Value##2", PinDirection::Input, true);
	CreateDataPin<bool>("Output", PinDirection::Output, true);
}

NodeResult SGNode_FloatLessThan::DoOperation()
{
	float value1;
	float value2;
	SetPinData("Output", false);
	if(GetPinData("Value##1", value1) && GetPinData("Value##2", value2))
	{
		if(value1 < value2)
		{
			SetPinData("Output", true);
		}
	}
	return NoExec();
}

SGNode_FloatMoreThan::SGNode_FloatMoreThan()
{
	AddFlag(ScriptGraphNodeFlag_Compact);
	CreateDataPin<float>("Value##1", PinDirection::Input, true);
	CreateDataPin<float>("Value##2", PinDirection::Input, true);
	CreateDataPin<bool>("Output", PinDirection::Output, true);
}

NodeResult SGNode_FloatMoreThan::DoOperation()
{
	float value1;
	float value2;
	SetPinData("Output", false);
	if(GetPinData("Value##1", value1) && GetPinData("Value##2", value2))
	{
		if(value1 > value2)
		{
			SetPinData("Output", true);
		}
	}
	return NoExec();
}

SGNode_FloatLessEqualThan::SGNode_FloatLessEqualThan()
{
	AddFlag(ScriptGraphNodeFlag_Compact);
	CreateDataPin<float>("Value##1", PinDirection::Input, true);
	CreateDataPin<float>("Value##2", PinDirection::Input, true);
	CreateDataPin<bool>("Output", PinDirection::Output, true);
}

NodeResult SGNode_FloatLessEqualThan::DoOperation()
{
	float value1;
	float value2;
	SetPinData("Output", false);
	if(GetPinData("Value##1", value1) && GetPinData("Value##2", value2))
	{
		if(value1 <= value2)
		{
			SetPinData("Output", true);
		}
	}
	return NoExec();
}

SGNode_FloatMoreEqualThan::SGNode_FloatMoreEqualThan()
{
	AddFlag(ScriptGraphNodeFlag_Compact);
	CreateDataPin<float>("Value##1", PinDirection::Input, true);
	CreateDataPin<float>("Value##2", PinDirection::Input, true);
	CreateDataPin<bool>("Output", PinDirection::Output, true);
}

NodeResult SGNode_FloatMoreEqualThan::DoOperation()
{
	float value1;
	float value2;
	SetPinData("Output", false);
	if(GetPinData("Value##1", value1) && GetPinData("Value##2", value2))
	{
		if(value1 >= value2)
		{
			SetPinData("Output", true);
		}
	}
	return NoExec();
}
