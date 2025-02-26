#pragma once
#include "Command.h"

struct AddEdgeCommand : public Command
{
	AddEdgeCommand(size_t aStartPin, size_t aEndPin, size_t aID,ScriptGraphSchema* aSchema)
		:startPin(aStartPin), endPin(aEndPin), mySchema(aSchema)
	{
		aId = aID;
	}
	size_t startPin, endPin;
	ScriptGraphSchema* mySchema;
	bool Undo() override
	{
		if(mySchema->GetEdges().contains(aId))
		{
			mySchema->RemoveEdge(aId);
			return true;
		}
		else
		{
			Redo();
			return false;
		}
	}
	bool Redo() override
	{
		aId = mySchema->CreateEdge(startPin, endPin);
		return true;
	}
	bool Clean() override
	{
		return !mySchema->GetPins().contains(startPin) || !mySchema->GetPins().contains(endPin);
	}
};

struct RemoveEdgeCommand : public Command
{
	RemoveEdgeCommand(size_t aStartPin, size_t aEndPin, size_t aID,ScriptGraphSchema* aSchema)
		:startPin(aStartPin), endPin(aEndPin), mySchema(aSchema)
	{
		aId = aID;
	}
	size_t startPin, endPin;
	ScriptGraphSchema* mySchema;
	bool Undo() override
	{
		aId = mySchema->CreateEdge(startPin, endPin);
		return true;
	}
	bool Redo() override
	{
		mySchema->RemoveEdge(aId);
		return true;
	}
	bool Clean() override
	{
		return !mySchema->GetPins().contains(startPin) || !mySchema->GetPins().contains(endPin);
	}
};