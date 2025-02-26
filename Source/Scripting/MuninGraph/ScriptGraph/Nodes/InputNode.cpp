#include "InputNode.h"
#include "ScriptGraph/ScriptGraphEdge.h"
#include "ScriptGraph/ScriptGraphSchema.h"
#include "../../Utilities/CommonUtilities/Input.h"

ScriptGraphInputNode::ScriptGraphInputNode()
{
	CreateExecPin("No Input", PinDirection::Output);
	CreateExecPin("Pressed", PinDirection::Output);
	CreateExecPin("Held", PinDirection::Output);
	CreateExecPin("Released", PinDirection::Output);
}

NodeResult ScriptGraphInputNode::DoOperation()
{
	
	if(CommonUtilities::Input::GetKeyDown(myKey))
	{
		return ExecPin("Pressed");
	}
	if(CommonUtilities::Input::GetKeyHeld(myKey))
	{
		return ExecPin("Held");
	}
	if(CommonUtilities::Input::GetKeyUp(myKey))
	{
		return ExecPin("Released");
	}
	return ExecPin("No Input");
}

const std::vector<std::type_index>& ScriptGraphInputNode::GetSupportedSchemas() const
{
	static std::vector<std::type_index> supportedSchemas =
	{
		typeid(ScriptGraphSchema)
	};

	return supportedSchemas;
}
