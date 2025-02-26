//#include "ScriptGraphSchema.h"
//
//#include <ranges>
//
//#include "FunctionScriptGraph.h"
//#include "MuninGraph.h"
//#include "ScriptGraph.h"
//#include "ScriptGraphNode.h"
//#include "ScriptGraphEdge.h"
//#include "ScriptGraphPin.h"
//#include "NodeGraph/NodeGraphSchema.h"
//#include "Nodes/EventNodes.h"
//#include "Nodes/FunctionNodes.h"
//#include "Nodes/VariableNodes.h"
//
//ScriptGraphSchema::ScriptGraphSchema(ScriptGraph* aGraph)
//	: ScriptGraphSchema(aGraph)
//{
//}
//
//std::shared_ptr<FunctionScriptGraph> ScriptGraphSchema::CreateFunction(std::string_view aName) const
//{
//	ScriptGraph* const scriptGraph = dynamic_cast<ScriptGraph*>(GetMutableGraph());
//	std::string lowerName(aName);
//	std::ranges::transform(lowerName, lowerName.begin(), tolower);
//	if(scriptGraph->myFunctions.contains(lowerName))
//	{
//		return {};
//	}
//
//	std::shared_ptr<FunctionScriptGraph> functionGraph = std::make_shared<FunctionScriptGraph>(scriptGraph);
//	scriptGraph->myFunctions.emplace(lowerName, functionGraph);
//	return functionGraph;
//}
//
//void ScriptGraphSchema::UpdateFunctionSignature(std::string_view aName, FunctionScriptGraphSignature& aSignature)
//{
//	ScriptGraph* const scriptGraph = dynamic_cast<ScriptGraph*>(GetMutableGraph());
//	std::string lowerName(aName);
//	std::ranges::transform(lowerName, lowerName.begin(), tolower);
//	if(scriptGraph->myFunctions.contains(lowerName))
//	{
//		throw std::runtime_error("That graph could not be found!");
//	}
//
//	const std::shared_ptr<FunctionScriptGraph>& functionGraph = scriptGraph->myFunctions.at(lowerName);
//	// Scan for all function nodes that use this function in all graphs that we own.
//	// TODO: MuninGraph itself can be used as a global repository of all nodes of each type
//	// so that we can ask it for nodes and edit them. Maybe a master MuninGraph schema? xP
//
//	DisconnectFunctionNodeIfNeeded(scriptGraph, functionGraph, aSignature);
//	for(const auto& funcGraph : scriptGraph->myFunctions | std::views::values)
//	{
//		DisconnectFunctionNodeIfNeeded(funcGraph.get(), functionGraph, aSignature);
//	}
//
//	functionGraph->UpdateSignature(aSignature);
//}
//
//void ScriptGraphSchema::DisconnectFunctionNodeIfNeeded(const ScriptGraphBase* aGraph,
//                                                       const std::shared_ptr<FunctionScriptGraph>& aFunctionGraph, const FunctionScriptGraphSignature& aSignature)
//{
//	for(const auto& node : aGraph->GetNodes() | std::views::values)
//	{
//		if(const auto& funcNode = std::dynamic_pointer_cast<SGNode_FunctionCall>(node))
//		{
//			if(funcNode->myFunction.lock() == aFunctionGraph)
//			{
//				for(const auto& [pinId, pin] : funcNode->GetPins())
//				{
//					if(pin.GetPinType() == ScriptGraphPinType::Data)
//					{
//						if(pin.GetPinDirection() == PinDirection::Input && !aSignature.ReturnValues.contains(pin.GetLabel()))
//						{
//							DisconnectPin(pinId);
//						}
//						else if(pin.GetPinDirection() == PinDirection::Output && !aSignature.Parameters.contains(pin.GetLabel()))
//						{
//							DisconnectPin(pinId);
//						}
//					}
//				}
//			}
//		}
//	}
//}
//
//FunctionScriptGraphSchema::FunctionScriptGraphSchema(FunctionScriptGraph* aGraph)
//	: ScriptGraphSchema(aGraph)
//{  }
//
