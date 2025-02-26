#pragma once
#include <ranges>
#include "MuninGraphImpl.h"
#include "ScriptGraph.h"
#include "ScriptGraphCommon.h"
#include "NodeGraph/NodeGraphSchema.h"
#include "Internal/MuninGraphCommon.h"

struct FunctionScriptGraphSignature;
class FunctionScriptGraph;
class TypedDataContainer;
class ScriptGraphPin;
class ScriptGraph;
class ScriptGraphNode;
struct ScriptGraphEdge;

class ScriptGraphSchema : public NodeGraphSchema<ScriptGraphSchema, ScriptGraphBase, ScriptGraphNode, ScriptGraphEdge, ScriptGraphPin>
{
public:
	ScriptGraphSchema(ScriptGraphBase* aGraph)
		: NodeGraphSchema(aGraph)
	{  }

	//~ Begin NodeGraphSchema Interface
	std::shared_ptr<ScriptGraphNode> CreateNode(const RegisteredNodeClass& aClass) override;
	bool RemoveNode(size_t aNodeUID) override;
	bool CanCreateEdge(size_t aSourcePinId, size_t aTargetPinId, std::string& outMessage) const override;
	int CreateEdge(size_t aSourcePinId, size_t aTargetPinId) override;
	bool DisconnectPin(size_t aPinId) override;
	bool RemoveEdge(size_t aEdgeId) override;
	//~ End NodeGraphSchema Interface

	bool RegisterNode(const std::shared_ptr<ScriptGraphNode>& aNode) override;

	template<typename T>
	std::shared_ptr<ScriptGraphNode> AddNode()
	{
		const std::type_index nodeType = typeid(T);
		const RegisteredNodeClass& nodeClass = MuninGraph::Get().GetNodeClass(nodeType);
		return CreateNode(nodeClass);
	}

	//~ Begin Dynamic Pins
	void MarkDynamicPinForDelete(size_t aPinId);
	void CommitDynamicPinDeletes();
	size_t CreateDynamicExecPin(size_t aNodeId, std::string_view aLabel, PinDirection aDirection);
	size_t CreateDynamicExecPin(ScriptGraphNode* aNode, std::string_view aLabel, PinDirection aDirection);
	size_t CreateDynamicDataPin(size_t aNodeId, std::string_view aLabel, PinDirection aDirection, std::type_index aType);
	size_t CreateDynamicDataPin(ScriptGraphNode* aNode, std::string_view aLabel, PinDirection aDirection, std::type_index aType);

	template<typename T>
	void CreateDynamicDataPin(size_t aNodeId, std::string_view aLabel, PinDirection aDirection)
	{
		CreateDynamicDataPin(aNodeId, aLabel, aDirection, typeid(T));
	}

	template<typename T>
	void CreateDynamicDataPin(ScriptGraphNode* aNode, std::string_view aLabel, PinDirection aDirection)
	{
		CreateDynamicDataPin(aNode, aLabel, aDirection, typeid(T));
	}

	void RemoveDynamicPin(size_t aNodeId, size_t aPinID);
	void RemoveDynamicPin(ScriptGraphNode* aNode, size_t aPinID);
	//~ End Dynamic Pins

	//~ Begin Variables
	std::shared_ptr<ScriptGraphNode> AddNode(std::type_index aType);
	bool AddVariable(std::string_view aVariableName, const TypedDataContainer& aDefaultValue, int aFlags = ScriptGraphVariableFlag_None);
	void RemoveVariable(const std::string& aVariableName);
	void SetNodeVariable(size_t aNodeUID, std::string_view aVariableName);
	void SetNodeVariable(ScriptGraphNode* aNode, std::string_view aVariableName);
	//~ End Variables
	FORCEINLINE const std::unordered_map<std::string, ScriptGraphVariable, string_hash, std::equal_to<>>& GetVariables() const { return GetGraph()->myVariables; }

	//~ Begin Functions
	std::shared_ptr<ScriptGraphNode> CreateFunctionNode(const std::shared_ptr<FunctionScriptGraph>& aFunctionGraph);
	std::shared_ptr<FunctionScriptGraph> CreateFunction(std::string_view aName) const;
	void UpdateFunctionSignature(std::string_view aName, FunctionScriptGraphSignature& aSignature);
	//~ End Functions
	FORCEINLINE const std::unordered_map<std::string, size_t, string_hash, std::equal_to<>>& GetEntryPoints() const { return GetGraph()->myEntryPoints; }
	void RecalculateEntryPoints() const;
private:
	void CheckCyclicLink(const ScriptGraphNode* aNode, const ScriptGraphNode* aBaseNode, bool& outResult) const;

	ScriptGraphPin& GetMutablePin(size_t aPinId);
	int CreateEdgeInternal(ScriptGraphPin& aSourcePin, ScriptGraphPin& aTargetPin) const;

	void DisconnectFunctionNodeIfNeeded(const ScriptGraphBase* aGraph, const std::shared_ptr<FunctionScriptGraph>& aFunctionGraph, const FunctionScriptGraphSignature& aSignature);

	std::vector<size_t> myPinsToDelete;
};