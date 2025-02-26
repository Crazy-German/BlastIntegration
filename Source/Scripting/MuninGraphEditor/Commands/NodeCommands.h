#pragma once
#include "Command.h"
#include "../../../Utilities/CommonUtilities/Vector3.hpp"

struct MoveNodeCommand : public Command
{
	MoveNodeCommand(std::shared_ptr<ScriptGraphNode> aNode, float PrevX, float PrevY, float PrevZ, float NewX, float NewY, float NewZ)
		:PrevPos(PrevX, PrevY, PrevZ), NewPos(NewX, NewY, NewZ), node(aNode) 
	{
		
	}
	std::shared_ptr<ScriptGraphNode> node;
	CommonUtilities::Vector3<float> PrevPos;
	CommonUtilities::Vector3<float> NewPos;
	bool Undo() override
	{
		node->SetDirectPosition(PrevPos.x, PrevPos.y, PrevPos.z);
		return true;
	};
	bool Redo() override
	{
		node->SetDirectPosition(NewPos.x, NewPos.y, NewPos.z);
		return true;
	};
	bool Clean() override{return false;};
};

struct CreateNodeCommand : public Command
{
	CreateNodeCommand(const RegisteredNodeClass aType, size_t aID, ScriptGraphSchema* aSchema)
		:myType(aType)
	{
		aId = aID;
		myShema = aSchema;
		XPos = YPos = ZPos = 0;
	}
	const RegisteredNodeClass myType;
	ScriptGraphSchema* myShema;
	float XPos, YPos, ZPos;
	bool Undo() override
	{
		myShema->GetNodes().at(aId)->GetNodePosition(XPos, YPos, ZPos);
		myShema->RemoveNode(aId);
		return true;
	};
	bool Redo() override
	{
		auto node = myShema->CreateNode(myType);
		node->SetNodePosition(XPos, YPos, ZPos);
		aId = aId = AsObjectUIDSharedPtr(node)->GetUID();
		return true;
	};
	bool Clean() override{return !myShema->GetNodes().contains(aId);};
};

struct RemoveNodeCommand : public  Command
{
	RemoveNodeCommand(const RegisteredNodeClass aType, size_t aID,ScriptGraphSchema* aSchema, float aX, float aY, float aZ)
		:myType(aType), XPos(aX), YPos(aY), ZPos(aZ)
	{
		aId = aID;
		myShema = aSchema;
	}
	const RegisteredNodeClass myType;
	ScriptGraphSchema* myShema;
	float XPos, YPos, ZPos;
	bool Redo() override
	{
		myShema->GetNodes().at(aId)->GetNodePosition(XPos, YPos, ZPos);
		myShema->RemoveNode(aId);
		return true;
	};
	bool Undo() override
	{
		auto node = myShema->CreateNode(myType);
		aId = AsObjectUIDSharedPtr(node)->GetUID();
		
		node->SetNodePosition(XPos, YPos, ZPos);
		return true;
	};
	bool Clean() override{ return false;};
};