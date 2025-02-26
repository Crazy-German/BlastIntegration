#pragma once
#include "ScriptGraphBase.h"
#include "ScriptGraphCommon.h"
#include "ScriptGraphPayload.h"
#include "Internal/MuninGraphCommon.h"
#include "NodeGraph/NodeGraph.h"
#include "NodeGraph/NodeGraphCommon.h"

class FunctionScriptGraph;
class ScriptGraph;
class ScriptGraphPayload;
struct ScriptGraphVariable;
class ScriptGraphSchema;
struct ScriptGraphEdge;
class ScriptGraphNode;
class ScriptGraphPin;

class ScriptGraph : public ScriptGraphBase
{
	friend class ScriptGraphSchema;

public:

	ScriptGraph(void* aOwner = nullptr);

	bool Execute(std::string_view aEntryPointHandle);
	bool ExecuteWithPayload(std::string_view aEntryPointHandle, const ScriptGraphPayload& aPayload);
	bool ExecuteInput();
	void Tick(float aDeltaTime);

	void Stop();

	bool Serialize(std::vector<uint8_t>& outResult) override;
	bool Deserialize(const std::vector<uint8_t>& inData) override;
		
	FORCEINLINE size_t GetNumActiveFunctions() const { return myNumActiveFunctions; }
	FORCEINLINE void* GetOwner() const { return myOwner; }
	const std::unordered_map<size_t, std::shared_ptr<ScriptGraphNode>>& GetNodes() const;
	const std::unordered_map<size_t, ScriptGraphEdge>& GetLinks() const;
protected:
	void Reset() override;

private:

	void SetupScriptGraph();

	size_t myNumActiveFunctions;
	void* myOwner;

	std::unordered_map<std::string, std::shared_ptr<FunctionScriptGraph>, string_hash, std::equal_to<>> myFunctions;
};

