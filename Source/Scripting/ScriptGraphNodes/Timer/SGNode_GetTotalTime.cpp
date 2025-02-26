#include "pch.h"
#include "SGNode_GetTotalTime.h"

#include "CommonUtilities/Camera.hpp"
#include "CommonUtilities/Timer.h"

IMPLEMENT_GRAPH_NODE(SGNode_GetTotalTime, ScriptGraphNode);

SGNode_GetTotalTime::SGNode_GetTotalTime()
{
	AddFlag(ScriptGraphNodeFlag_Compact);

	CreateDataPin<float>("TotalTime",PinDirection::Output, true);
}

NodeResult SGNode_GetTotalTime::DoOperation()
{
	float temp = static_cast<float>(CommonUtilities::Timer::Get().GetTotalTime());
	SetPinData("TotalTime", temp);
	return NoExec();
}
