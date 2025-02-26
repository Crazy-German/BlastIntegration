#include "ScriptGraphBase.h"
#include "ScriptGraphPin.h"
#include "ScriptGraphNode.h"
#include "ScriptGraphEdge.h"
#include "ScriptGraphPayload.h"
#include "ScriptGraphSchema.h"
#include "ScriptGraphVariable.h"

void ScriptGraphBase::ClearLastExecutedPath()
{
	myLastExecutedPath.clear();
}

void ScriptGraphBase::ResetEntryPoints()
{
	myEntryPoints.clear();
}

void ScriptGraphBase::Reset()
{
	NodeGraph::Reset();
	myVariables.clear();
	myEntryPoints.clear();
	myNodeIdToEntryPoint.clear();
}

ScriptGraphBase::ScriptGraphBase(ScriptGraphType aType)
	: myShouldStop(false), myGraphType(aType)
{
}

ScriptGraphBase::~ScriptGraphBase() = default;

void ScriptGraphBase::BindErrorHandler(ScriptGraphErrorHandlerSignature&& aErrorHandler)
{
	myErrorDelegate = std::move(aErrorHandler);
}

void ScriptGraphBase::UnbindErrorHandler()
{
	myErrorDelegate = nullptr;
}

bool ScriptGraphBase::ExecuteInternal(ScriptGraphNode* aNode, size_t aPinId)
{
	assert(aNode);

	ScriptGraphNode* currentNode = aNode;
	size_t currentNodeEntryPin = aPinId;

	bool railFinished = false;

	NodeResult result;
	do
	{
		// Execute the node.
		result = currentNode->Enter(currentNodeEntryPin, NodeEntryType::New);

		// If the node is In Progress, just loop and let it do its thing.
		// This is used in i.e. WHILE, FOR and FOREACH loops or even SEQUENCE nodes.
		while (result.State() == NodeResultState::InProgress && myShouldStop == false)
		{
			const ScriptGraphPin& exitPin = GetPinFromId(result.Data());
			if (exitPin.IsConnected())
			{
				const size_t exitEdgeId = exitPin.GetEdges()[0];
				const NodeGraphEdge& exitEdge = GetEdgeFromId(exitEdgeId);
				const ScriptGraphPin& entryPin = GetPinFromId(exitEdge.ToId);

				myLastExecutedPath.push_back(exitEdgeId);

				// Start a new Exec for the provided pin.
				if (!ExecuteInternal(entryPin.GetOwner(), entryPin.GetUID()))
				{
					// If something went wrong, stop executing.
					return false;
				}
			}

			// Then tell the node to execute again.
			result = currentNode->Enter(aPinId, NodeEntryType::InProgress);
		}

		// Result was Finished, go to next node if we have one.
		if (result.State() == NodeResultState::Finished)
		{
			if(result.Data() > 0)
			{
				const ScriptGraphPin& exitPin = GetPinFromId(result.Data());
				railFinished = !exitPin.IsConnected();
				if (!railFinished)
				{
					const size_t exitEdgeId = exitPin.GetEdges()[0];
					const NodeGraphEdge& exitEdge = GetEdgeFromId(exitEdgeId);
					const ScriptGraphPin& entryPin = GetPinFromId(exitEdge.ToId);

					myLastExecutedPath.push_back(exitEdgeId);

					currentNode = entryPin.GetOwner();
					currentNodeEntryPin = entryPin.GetUID();
				}
			}
		}

	} while (!railFinished && result.State() != NodeResultState::Error && myShouldStop == false);

	if(result.State() == NodeResultState::Error)
	{
		const auto nodeUIDPtr = AsObjectUIDPtr(currentNode);
		ReportError(nodeUIDPtr->GetUID(), currentNode->GetError());
		return false;
	}

	assert(result.State() == NodeResultState::Finished);

	return true;
}

void ScriptGraphBase::ReportError(size_t aNodeId, std::string_view aErrorMessage) const
{
	if(myErrorDelegate)
	{
		myErrorDelegate(this, aNodeId, aErrorMessage);
	}
}