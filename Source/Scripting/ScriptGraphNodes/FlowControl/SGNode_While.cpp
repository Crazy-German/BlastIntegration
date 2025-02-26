#include "pch.h"
#include "SGNode_While.h"

IMPLEMENT_GRAPH_NODE(SGNode_While, ScriptGraphNode);

SGNode_While::SGNode_While()
{
	CreateExecPin("In", PinDirection::Input, false);

	CreateExecPin("Loop Body", PinDirection::Output, true);
	CreateExecPin("Completed", PinDirection::Output, true);
	
	CreateDataPin<bool>("Condition", PinDirection::Input);
}

NodeResult SGNode_While::DoOperation()
{
	bool condition = false;
	if(GetPinData("Condition", condition))
	{
		if(condition)
		{
			return ExecPin("Loop Body", NodeResultState::InProgress);
		}
		return ExecPin("Completed");
	}
	return NoExec();
}
