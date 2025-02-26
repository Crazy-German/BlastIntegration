#include "pch.h"
#include "SGNode_Tick.h"

IMPLEMENT_GRAPH_NODE(SGNode_Tick, ScriptGraphEventNode);

SGNode_Tick::SGNode_Tick()
{
	CreateDataPin<float>("Delta Time", PinDirection::Output);
}
