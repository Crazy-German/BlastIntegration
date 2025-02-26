#include "pch.h"
#include "SGNode_Switch.h"

IMPLEMENT_GRAPH_NODE(SGNode_IntSwitch, ScriptGraphNode);
//IMPLEMENT_GRAPH_NODE(SGNode_EnumSwitch, ScriptGraphNode);

SGNode_IntSwitch::SGNode_IntSwitch()
{
	AddFlag(GraphNodeFlag_DynamicOutputPins);
	AddFlag(GraphNodeFlag_EditableOutputPins);

	CreateExecPin("In", PinDirection::Input, false);
	CreateDataPin<int>("Selection", PinDirection::Input);
	CreateExecPin("Default", PinDirection::Output);
	caseCount = 0;
}

NodeResult SGNode_IntSwitch::DoOperation()
{
	int caseVal;
	GetPinData("Selection", caseVal);
	for(auto& pin: GetPins() | std::views::values)
	{
		size_t pos = pin.GetLabel().find_first_of("0123456789");
		if(pos == std::string::npos)
		{
			continue;
		}
		int pinVal = atoi(std::string(pin.GetLabel().substr(pos)).c_str());
		if(pinVal == caseVal)
		{
			return ExecPin(pin.GetLabel());
		}
	}
	return ExecPin("Default");
}

void SGNode_IntSwitch::OnUserAddedPin(ScriptGraphSchema* aSchema, size_t aPinId)
{
	aSchema;
	RenamePin(aPinId, ("Case "+std::to_string(caseCount)));
	caseCount++;
}

void SGNode_IntSwitch::OnUserRemovedPin(ScriptGraphSchema* aSchema, size_t aPinId)
{
	ScriptGraphNode::OnUserRemovedPin(aSchema, aPinId);
}
//
//SGNode_EnumSwitch::SGNode_EnumSwitch()
//{
//	AddFlag(GraphNodeFlag_DynamicOutputPins);
//	AddFlag(GraphNodeFlag_EditableOutputPins);
//
//	CreateExecPin("In", PinDirection::Input, false);
//	CreateDataPin<int>("Selection", PinDirection::Input);
//	CreateExecPin("Default", PinDirection::Output);
//}
//
//NodeResult SGNode_EnumSwitch::DoOperation()
//{
//	return ExecPin("Default");
//}
//
//void SGNode_EnumSwitch::OnUserAddedPin(ScriptGraphSchema* aSchema, size_t aPinId)
//{
//	RenamePin(aPinId, ("Case "+std::to_string(caseCount)));
//	caseCount++;
//}
//
//void SGNode_EnumSwitch::OnUserRemovedPin(ScriptGraphSchema* aSchema, size_t aPinId)
//{
//	ScriptGraphNode::OnUserRemovedPin(aSchema, aPinId);
//}
