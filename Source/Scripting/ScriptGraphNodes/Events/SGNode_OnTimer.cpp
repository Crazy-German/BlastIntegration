#include "pch.h"
#include "SGNode_OnTimer.h"
IMPLEMENT_GRAPH_NODE(SGNode_OnTimer, ScriptGraphTimerEventNode)

SGNode_OnTimer::SGNode_OnTimer()
{
	CreateDataPin<unsigned>("aID", PinDirection::Input);
	CreateDataPin<unsigned>("ID", PinDirection::Output);
}

std::string_view SGNode_OnTimer::GetEntryHandle()
{
	
	unsigned aId;
	if(GetPinData("aID", aId))
	{
		myEntryHandle = std::string("Timer"+std::to_string(aId));
		return myEntryHandle;
	}
	return "SGNode_OnTimer";
}
