#include "pch.h"
#include "SGNode_AND.h"



IMPLEMENT_GRAPH_NODE(SGNode_AND, ScriptGraphNode);

SGNode_AND::SGNode_AND()
{
	AddFlag(ScriptGraphNodeFlag_Compact);
	
		CreateDataPin<bool>("Condition##1", PinDirection::Input, true);
	CreateDataPin<bool>("Condition##2", PinDirection::Input, true);
	CreateDataPin<bool>("Output", PinDirection::Output, true);
}

NodeResult SGNode_AND::DoOperation()
{
	bool condition = false;
	bool condition2= false;
	SetPinData("Output", false);
	if(GetPinData("Condition##1", condition) && GetPinData("Condition##2", condition2))
	{
		if(condition && condition2)
		{
			SetPinData("Output", true);
		}
	}
	return NoExec();
}
