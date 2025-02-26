#pragma once
#include "ScriptGraphCommon.h"
#include "ScriptGraphPayload.h"
#include "Internal/MuninGraphCommon.h"
#include "NodeGraph/NodeGraph.h"
#include "NodeGraph/NodeGraphCommon.h"

#include <functional>

class ScriptGraphSchema;
class ScriptGraphPayload;
struct ScriptGraphVariable;
class ScriptGraphSchema;
struct ScriptGraphEdge;
class ScriptGraphNode;
class ScriptGraphPin;

// Barebones base for Script Graphs. Used for both the master ScriptGraph class and the FunctionGraph.
class ScriptGraphBase : public NodeGraph<ScriptGraphNode, ScriptGraphPin, ScriptGraphEdge, ScriptGraphSchema>
{
	friend class ScriptGraphSchema;
	friend class ScriptGraphSchema;

public:
	~ScriptGraphBase() override;

	using ScriptGraphErrorHandlerSignature = std::function<void(const ScriptGraphBase*, size_t, std::string_view)>;
	void BindErrorHandler(ScriptGraphErrorHandlerSignature&& aErrorHandler);
	void UnbindErrorHandler();

	FORCEINLINE ScriptGraphType GetScriptGraphType() const { return myGraphType; }
	FORCEINLINE const std::vector<size_t>& GetLastExecutedPath() const { return myLastExecutedPath; }

	void ClearLastExecutedPath();

	void ResetEntryPoints();
	bool ExecuteInternal(ScriptGraphNode* aNode, size_t aPinId);
protected:
	ScriptGraphBase(ScriptGraphType aType);

	void Reset() override;

	void ReportError(size_t aNodeId, std::string_view aErrorMessage) const;

	bool myShouldStop;
	std::vector<size_t> myLastExecutedPath;

	std::unordered_map<std::string, size_t, string_hash, std::equal_to<>> myEntryPoints;
	std::unordered_map<size_t, std::string> myNodeIdToEntryPoint;

	std::unordered_map<std::string, ScriptGraphVariable, string_hash, std::equal_to<>> myVariables;
	
private:

	ScriptGraphType myGraphType;
	ScriptGraphErrorHandlerSignature myErrorDelegate;
};
