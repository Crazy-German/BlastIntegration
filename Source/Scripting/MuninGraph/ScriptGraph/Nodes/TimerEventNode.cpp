#include "TimerEventNode.h"
#include "ScriptGraph/ScriptGraphEdge.h"
#include "ScriptGraph/ScriptGraphSchema.h"
ScriptGraphTimerEventNode::ScriptGraphTimerEventNode()
{
	CreateExecPin("Out", PinDirection::Output, false);
	myEntryHandle = "TimerEventNode";
}

NodeResult ScriptGraphTimerEventNode::DoOperation()
{
	for(const auto& val : GetPins() | std::views::values)
	{
		if(val.GetPinDirection() == PinDirection::Input && !val.IsConnected())
		{
			printf("Timer Event node not Connected to id, please connect the timerEvent\n");
			return NoExec();
		}
	}
	return ExecPin("Out");
}


const std::vector<std::type_index>& ScriptGraphTimerEventNode::GetSupportedSchemas() const
{
	static std::vector<std::type_index> supportedSchemas =
	{
		typeid(ScriptGraphSchema)
	};

	return supportedSchemas;
}

