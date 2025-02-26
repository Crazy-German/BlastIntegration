#pragma once
#include <functional>
#include <string>

class ScriptGraphNode;
class ScriptGraphPin;
struct ScriptGraphEdge;
class ScriptGraphSchema;
class ScriptGraph;
//class ScriptGraph;
typedef std::function<std::string()> TimerCallBack;
class TimerManager
{
public:
	static TimerManager* Get();
	
	void Update(float aDeltaTime);
	unsigned AddTimer(TimerCallBack aCallBack, float aDuration, ScriptGraph* aGraph, bool aLoop = true);
	void RemoveTimer(unsigned aTimerId);
	void ClearTimers();
	~TimerManager();
private:
	static void RemoveInternal();
	static unsigned GetId();
	unsigned TimerID;
	struct Timer
	{
		TimerCallBack myFunc;
		float myTimeElapsed;
		float myDuration;
		bool myLoop;
		ScriptGraph* myGraph;
	};
	std::unordered_map<unsigned, Timer> myTimers;
	std::unordered_map<unsigned, bool> myToRemove;

	TimerManager();
	
	static TimerManager* myInstance;
};
