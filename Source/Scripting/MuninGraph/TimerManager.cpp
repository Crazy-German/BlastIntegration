#include "TimerManager.h"

#include <ranges>
#include <utility>

#include "ScriptGraph/ScriptGraph.h"
#include "ScriptGraph/ScriptGraphEdge.h"
#include "ScriptGraph/ScriptGraphNode.h"
#include "ScriptGraph/ScriptGraphPin.h"
#include "ScriptGraph/ScriptGraphSchema.h"

TimerManager* TimerManager::myInstance = nullptr;

TimerManager* TimerManager::Get()
{
	if(myInstance == nullptr)
	{
		myInstance = new TimerManager();
	}
	return myInstance;
}


void TimerManager::Update(float aDeltaTime)
{
	for(auto& [id,timer] : myInstance->myTimers)
	{
		if(!myInstance->myTimers.contains(id) || myInstance->myToRemove.contains(id))
		{
			continue;
		}
		timer.myTimeElapsed+=aDeltaTime;
		if(timer.myTimeElapsed >= timer.myDuration)
		{
			timer.myGraph->Execute(timer.myFunc());
			if(timer.myLoop)
			{
				timer.myTimeElapsed = 0;
			}
			else
			{
				myInstance->myToRemove.emplace(id, false);
			}
		}
	}
	
	RemoveInternal();
}

unsigned TimerManager::AddTimer(TimerCallBack aCallBack, float aDuration, ScriptGraph* aGraph, bool aLoop)
{
	unsigned id = GetId();
	
	myInstance->myTimers.emplace(std::pair<unsigned, Timer>(id, Timer()));
	myInstance->myTimers.at(id).myFunc = std::move(aCallBack);
	myInstance->myTimers.at(id).myDuration = aDuration;
	myInstance->myTimers.at(id).myTimeElapsed = 0;
	myInstance->myTimers.at(id).myLoop = aLoop;
	myInstance->myTimers.at(id).myGraph = aGraph;
	
	return myInstance->TimerID-1;
}

void TimerManager::RemoveTimer(unsigned aTimerId)
{
	if(myInstance->myTimers.contains(aTimerId))
	{
		myInstance->myToRemove.emplace(aTimerId, false);
	}
}

void TimerManager::ClearTimers()
{
	myTimers.clear();
}

TimerManager::TimerManager()
{
		TimerID = 0;
}

TimerManager::~TimerManager()
{
	delete myInstance;
}

void TimerManager::RemoveInternal()
{
	for(auto& id : myInstance->myToRemove | std::views::keys)
	{
		myInstance->myTimers.erase(id);
		myInstance->myToRemove.at(id) = true;
	}
}

unsigned TimerManager::GetId()
{
	if(myInstance->myTimers.empty())
	{
		myInstance->TimerID = 0;
		myInstance->myToRemove.clear();
	}
	for(auto& [id,free] : myInstance->myToRemove)
	{
		if(free)
		{
			return id;
		}
	}
	return myInstance->TimerID++;
}
