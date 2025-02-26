#pragma once
#include <vector>
#include <map>
#include "GameState.h"

class State;

class StateStack
{
public:

	~StateStack();
	void PushState(GameState aID);
	void AddState(GameState aID, State* aState);
	State* GetCurrentState();
	int Size();
	void Pop();
	void RenderStateAtIndex(int aIndex);
	void Clear();
	State* GetState(int aValue);
private:
	std::vector<State*> myStates;
	std::map<GameState, State*> myCachedStates;
};