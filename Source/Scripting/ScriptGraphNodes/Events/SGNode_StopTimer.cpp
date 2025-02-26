#include "pch.h"
#include "SGNode_StopTimer.h"

#include "TimerManager.h"

IMPLEMENT_GRAPH_NODE(SGNode_StopTimer, ScriptGraphNode)

SGNode_StopTimer::SGNode_StopTimer()
{
	CreateExecPin("In", PinDirection::Input, false);
	CreateExecPin("Out", PinDirection::Output, false);
	CreateDataPin<unsigned>("Timer ID",PinDirection::Input);
}

NodeResult SGNode_StopTimer::DoOperation()
{
	unsigned aID = (unsigned)-1;
	if(GetPinData("Timer ID", aID))
	{
		if(aID> 0)
		{
			TimerManager::Get()->RemoveTimer(aID);
			return ExecPin("Out");
		}
	}
	return NoExec();
}
