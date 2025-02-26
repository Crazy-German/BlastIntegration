#include "EventNodes.h"
#include "ScriptGraph/ScriptGraphEdge.h"
#include "ScriptGraph/ScriptGraphSchema.h"

ScriptGraphEventNode::ScriptGraphEventNode()
{
	AddFlag(GraphNodeFlag_Unique);
	CreateExecPin("Out", PinDirection::Output, true);
}

NodeResult ScriptGraphEventNode::DoOperation()
{
	return ExecPin("Out", NodeResultState::Finished);
}

const std::vector<std::type_index>& ScriptGraphEventNode::GetSupportedSchemas() const
{
	static std::vector<std::type_index> supportedSchemas =
	{
		typeid(ScriptGraphSchema)
	};

	return supportedSchemas;
}