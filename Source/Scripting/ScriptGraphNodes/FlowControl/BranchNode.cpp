#include "pch.h"
#include "BranchNode.h"


IMPLEMENT_GRAPH_NODE(SGNode_BranchNode, ScriptGraphNode);

SGNode_BranchNode::SGNode_BranchNode()
{
	CreateExecPin("In", PinDirection::Input, false);
	CreateExecPin("True", PinDirection::Output, true);
	CreateExecPin("False", PinDirection::Output, true);
	
	CreateDataPin<bool>("Condition", PinDirection::Input);
}

NodeResult SGNode_BranchNode::DoOperation()
{
	bool condition = false;
	if(GetPinData("Condition", condition))
	{
		if(condition)
		{
			return ExecPin("True");
		}
		return ExecPin("False");
	}
	return NoExec();
}
