#include "pch.h"
#include "SGNode_Timeline.h"

#include "TimerManager.h"
#include "CommonUtilities/AABB3D.hpp"
#include "CommonUtilities/Timer.h"

IMPLEMENT_GRAPH_NODE(SGNode_Timeline, ScriptGraphTimerEventNode);

SGNode_Timeline::SGNode_Timeline()
{
	RenamePin("Out", "Start");
	GetMutablePin("Start").SetLabelVisible(true);
	CreateExecPin("Play", PinDirection::Input);
	CreateExecPin("Update", PinDirection::Output);
	CreateExecPin("Finished", PinDirection::Output);
	CreateDataPin<float>("Time A", PinDirection::Input);
	CreateDataPin<float>("Time B", PinDirection::Input);
	CreateDataPin<float>("CurrentTime", PinDirection::Output);
	CreateDataPin<float>("Alpha", PinDirection::Output);
}

NodeResult SGNode_Timeline::Enter(size_t anEntryPinId, NodeEntryType aEntryType)
{
	if(GetPinData("Time A", myCurrentTime))
	{
		SetPinData("CurrentTime", myCurrentTime);
		float timeb;
		if(GetPinData("Time B", timeb))
		{
			SetPinData("Alpha", 0);
		}

	}
	return ScriptGraphNode::Enter(anEntryPinId, aEntryType);
}

std::string SGNode_Timeline::GetEntryHandle() const
{
	return "Timeline" + std::to_string(myDoneUpdateID);
}

NodeResult SGNode_Timeline::DoOperation()
{
	float duration;
	if(GetPinData("Time B", duration))
	{
		myTimerUpdateID = TimerManager::Get()->AddTimer([this]()->std::string {return this->Update();}, 0, static_cast<ScriptGraph*>(GetOwner()),true);
		myDoneUpdateID = TimerManager::Get()->AddTimer([this]()->std::string {return this->Complete();}, duration, static_cast<ScriptGraph*>(GetOwner()),false);
	}
	return ExecPin("Start");
}

std::string SGNode_Timeline::Update()
{
	myCurrentTime+= CommonUtilities::Timer::Get().GetDeltaTime();
	float endtime = 0;
	float startTime = 0;
	SetPinData("CurrentTime", myCurrentTime);
	if(GetPinData("Time B", endtime) && GetPinData("Time A", startTime))
	{
		SetPinData("Alpha", (myCurrentTime)/(endtime-startTime));
		if(endtime < myCurrentTime)
		{
			TimerManager::Get()->RemoveTimer(myTimerUpdateID);
			return "NoExec";
		}
	}
	if(myCurrentTime>=startTime)
	{
		size_t exitEdgeID = GetPin("Update").GetEdges()[0];
		const NodeGraphEdge& exitEdge = GetOwner()->GetEdgeFromId(exitEdgeID);
		const ScriptGraphPin& entryPin = GetOwner()->GetPinFromId(exitEdge.ToId);
		if (!GetOwner()->ExecuteInternal(entryPin.GetOwner(), entryPin.GetUID()))
		{
			// If something went wrong, stop executing.
			return "Error";
		}
	}
	return "Success";
}

std::string SGNode_Timeline::Complete()
{
	size_t exitEdgeID = GetPin("Finished").GetEdges()[0];
	const NodeGraphEdge& exitEdge = GetOwner()->GetEdgeFromId(exitEdgeID);
	const ScriptGraphPin& entryPin = GetOwner()->GetPinFromId(exitEdge.ToId);
	if (!GetOwner()->ExecuteInternal(entryPin.GetOwner(), entryPin.GetUID()))
	{
		// If something went wrong, stop executing.
		return "Error";
	}
	return "NoExec";
}
