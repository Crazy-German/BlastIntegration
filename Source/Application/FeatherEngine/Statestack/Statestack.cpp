#include "FeatherEngine.pch.h"
#include "StateStack.h"
#include "State.h"

StateStack::~StateStack()
{
	for (auto iterator : myCachedStates)
	{
		delete iterator.second;
	}
}

void StateStack::PushState(GameState aID)
{
	myStates.push_back(myCachedStates[aID]);
}

void StateStack::AddState(GameState aID, State* aState)
{
	myCachedStates[aID] = aState;
}

State* StateStack::GetCurrentState()
{
	return myStates[myStates.size() - 1];
}

int StateStack::Size()
{
	return static_cast<int>(myStates.size());
}

void StateStack::Pop()
{
	myStates.pop_back();
}

void StateStack::RenderStateAtIndex(int aIndex)
{
	if (aIndex < 0) 
	{
		return;
	}

	if (myStates[aIndex]->LetThroughRender() == true) 
	{
		RenderStateAtIndex(aIndex - 1);
	}

	myStates[aIndex]->Render();
}

void StateStack::Clear()
{
	myStates.clear();
}

State* StateStack::GetState(int aValue)
{
	return myStates[aValue];
}
