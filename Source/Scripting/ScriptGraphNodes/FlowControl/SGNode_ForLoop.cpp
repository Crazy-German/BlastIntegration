#include "pch.h"
#include "SGNode_ForLoop.h"

IMPLEMENT_GRAPH_NODE(SGNode_ForLoop, ScriptGraphNode)

SGNode_ForLoop::SGNode_ForLoop()
{
	CreateExecPin("In", PinDirection::Input, false);

	CreateDataPin<int>("First Index", PinDirection::Input);
	CreateDataPin<int>("Last Index", PinDirection::Input);

	CreateExecPin("Loop Body", PinDirection::Output, true);
	CreateDataPin<int>("Index", PinDirection::Output);
	CreateExecPin("Completed", PinDirection::Output, true);
}

NodeResult SGNode_ForLoop::Enter(size_t anEntryPinId, NodeEntryType aEntryType)
{
	if(aEntryType == NodeEntryType::New)
	{
		if(!GetPinData("First Index", myIndex))
		{
			printf("Failed to reset index\n");
		}
	}
	else if(aEntryType == NodeEntryType::InProgress)
	{
		myIndex++;
	}

	return ScriptGraphNode::Enter(anEntryPinId, aEntryType);
}

NodeResult SGNode_ForLoop::DoOperation()
{
	int lastIndex;
	if(GetPinData("Last Index", lastIndex))
	{
		SetPinData("Index", myIndex);
		if(myIndex<= lastIndex)
		{
			return ExecPin("Loop Body", NodeResultState::InProgress);
		}
		return ExecPin("Completed");
	}
	return NoExec();

}
