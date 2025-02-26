#include "pch.h"
#include "SGNode_Timer.h"

#include "TimerManager.h"
#include "CommonUtilities/Timer.h"

IMPLEMENT_GRAPH_NODE(SGNode_Timer, ScriptGraphNode)

SGNode_Timer::SGNode_Timer()
{

	CreateExecPin("Out", PinDirection::Output, false);
	CreateExecPin("In", PinDirection::Input, false);
	CreateDataPin<float>("Duration", PinDirection::Input);
	CreateDataPin<bool>("Looping", PinDirection::Input);
	CreateDataPin<unsigned>("ID", PinDirection::Output);
	wasAdded = false;
}

NodeResult SGNode_Timer::Enter(size_t anEntryPinId, NodeEntryType aEntryType)
{
	if(aEntryType == NodeEntryType::New)
	{
		float duration;
		bool loop;
		if(GetPinData("Duration", duration) && GetPinData("Looping", loop))
		{
			myId = TimerManager::Get()->AddTimer([this]()->std::string {return this->Completed();}, duration, static_cast<ScriptGraph*>(GetOwner()),loop);
			SetPinData("ID", myId);
			//wasAdded = true;
		}
		else
		{
			printf("Hey timer error");
		}
	}
	return ScriptGraphNode::Enter(anEntryPinId, aEntryType);
}

NodeResult SGNode_Timer::DoOperation()
{
	/*myTimer+= CommonUtilities::MainSingleton::GetInstance()->GetTimer().GetDeltaTime();
	
	float delay = 0;*/
	/*if(GetPinData("Duration", delay))
	{
		SetPinData("Current", myTimer);
		if(myTimer>= delay)
		{
			return ExecPin("On Timer");
		}
	}*/
	
	return ExecPin("Out", NodeResultState::Finished);
}

std::string SGNode_Timer::Completed()
{
	return "Timer"+std::to_string(myId);
}
