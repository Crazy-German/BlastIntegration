#include "pch.h"
#include "SGNode_Sequence.h"

IMPLEMENT_GRAPH_NODE(SGNode_Sequence, ScriptGraphNode);

SGNode_Sequence::SGNode_Sequence()
{
	AddFlag(GraphNodeFlag_DynamicOutputPins);
	AddFlag(GraphNodeFlag_EditableOutputPins);
	
	CreateExecPin("In", PinDirection::Input, false);
	myNodeCount = 0;
	CreateExecPin(("Then ")+std::to_string(myNodeCount++), PinDirection::Output);
}

NodeResult SGNode_Sequence::Enter(size_t anEntryPinId, NodeEntryType aEntryType)
{
	myIndex++;
	if(aEntryType == NodeEntryType::New)
	{
		myIndex = 0;
	}

	return ScriptGraphNode::Enter(anEntryPinId, aEntryType);
}

NodeResult SGNode_Sequence::DoOperation()
{
	if(myIndex == myNodeCount-1)
	{
			return ExecPin(("Then "+std::to_string(myIndex)));
	}
	return ExecPin(("Then "+std::to_string(myIndex)),NodeResultState::InProgress);
}
#ifndef NOEDITOR

void SGNode_Sequence::OnUserAddedPin(ScriptGraphSchema* aSchema, size_t aPinId)
{
	aSchema;
	RenamePin(aPinId, ("Then "+std::to_string(myNodeCount)));
	myNodeCount++;
}

void SGNode_Sequence::OnUserRemovedPin(ScriptGraphSchema* aSchema, size_t aPinId)
{
	aSchema;
	aPinId;
	/*int nextIndexoffset = 0;
	std::string label;
	for(int i = 0; i<myNodeCount; i++)
	{
		label = GetPin(aPinId).GetLabel();
		if(label.ends_with(std::to_string(i)))
		{
			nextIndexoffset = -1;
			continue;
		}
		RenamePin("Then "+std::to_string(i),"Then "+std::to_string(i+nextIndexoffset));
	}
	myNodeCount--;*/

}
#endif