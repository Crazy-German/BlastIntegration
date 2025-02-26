#include "pch.h"
#include "SGNode_OnTouch.h"

#include "GameObjectSystem/GameObject.h"

IMPLEMENT_GRAPH_NODE(SGNode_OnTouchFound, ScriptGraphEventNode);
SGNode_OnTouchFound::SGNode_OnTouchFound()
{
	CreateDataPin<GameObject*>("OtherActor", PinDirection::Output);
}

IMPLEMENT_GRAPH_NODE(SGNode_OnTouchLost, ScriptGraphEventNode);
SGNode_OnTouchLost::SGNode_OnTouchLost()
{
	CreateDataPin<GameObject*>("OtherActor", PinDirection::Output);
}
