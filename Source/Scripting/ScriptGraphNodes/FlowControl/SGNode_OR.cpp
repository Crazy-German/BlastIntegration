#include "pch.h"
#include "SGNode_OR.h"

IMPLEMENT_GRAPH_NODE(SGNode_OR, ScriptGraphNode);

SGNode_OR::SGNode_OR()
{
	AddFlag(ScriptGraphNodeFlag_Compact);
	
		CreateDataPin<bool>("Condition##1", PinDirection::Input, true);
	CreateDataPin<bool>("Condition##2", PinDirection::Input, true);
	CreateDataPin<bool>("Output", PinDirection::Output, true);
}

NodeResult SGNode_OR::DoOperation()
{
	bool condition = false;
	bool condition2= false;
	SetPinData("Output", false);
	if(GetPinData("Condition##1", condition) && GetPinData("Condition##2", condition2))
	{
		if(condition || condition2)
		{
			SetPinData("Output", true);
		}
	}
	return NoExec();
}
