//#pragma once
//#include "MuninGraphImpl.h"
//#include "ScriptGraph.h"
//#include "ScriptGraphCommon.h"
//#include "ScriptGraphSchema.h"
//#include "NodeGraph/NodeGraphSchema.h"
//#include "Internal/MuninGraphCommon.h"
//
//struct FunctionScriptGraphSignature;
//class FunctionScriptGraph;
//class TypedDataContainer;
//class ScriptGraphPin;
//class ScriptGraph;
//class ScriptGraphNode;
//struct ScriptGraphEdge;
//
//class ScriptGraphSchema : public ScriptGraphSchema
//{
//public:
//
//	ScriptGraphSchema(ScriptGraph* aGraph);
//
//	FORCEINLINE const std::unordered_map<std::string, size_t, string_hash, std::equal_to<>>& GetEntryPoints() const { return GetGraph()->myEntryPoints; }
//
//	//~ Begin Functions
//	std::shared_ptr<FunctionScriptGraph> CreateFunction(std::string_view aName) const;
//	void UpdateFunctionSignature(std::string_view aName, FunctionScriptGraphSignature& aSignature);
//	//~ End Functions
//
//private:
//
//	void DisconnectFunctionNodeIfNeeded(const ScriptGraphBase* aGraph, const std::shared_ptr<FunctionScriptGraph>& aFunctionGraph, const FunctionScriptGraphSignature& aSignature);
//};
//
//class FunctionScriptGraphSchema : public ScriptGraphSchema
//{
//public:
//	FunctionScriptGraphSchema(FunctionScriptGraph* aGraph);
//
//	//~ Begin Functions
//	void UpdateFunctionSignature(std::string_view aName, FunctionScriptGraphSignature& aSignature) const;
//	//~ End Functions
//};