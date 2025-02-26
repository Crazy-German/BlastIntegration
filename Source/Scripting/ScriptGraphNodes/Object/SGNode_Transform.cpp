#include "pch.h"
#include "SGNode_Transform.h"

#include "../../../Application/FeatherEngine/GameObjectSystem/GameObject.h"
#include "../../../Application/FeatherEngine/GameObjectSystem/Components/Transform.h"
#include "../../../Application/FeatherEngine/GameObjectSystem/Components/Component.h"

#include "CommonUtilities/Vector3.hpp"

#define IMPLEMENT_ACTOR_NODE(NAME) \
IMPLEMENT_GRAPH_NODE(SGNode_##NAME, ScriptGraphNode) \
SGNode_##NAME::SGNode_##NAME()

#define IMPLEMENT_ACTOR_OPERATION(NAME) \
NodeResult SGNode_##NAME::DoOperation() 

IMPLEMENT_ACTOR_NODE(GetPosition)
{
	AddFlag(ScriptGraphNodeFlag_Compact); 
	CreateDataPin<GameObject*>("In", PinDirection::Input, true);
	CreateDataPin<CommonUtilities::Vector3f>("Out", PinDirection::Output, true);
}
IMPLEMENT_ACTOR_OPERATION(GetPosition)
{
	GameObject* in;
	if (GetPinData("In", in))
	{
		if(in == nullptr)
		{
			in = static_cast<GameObject*>(reinterpret_cast<ScriptGraph*>(GetOwner())->GetOwner());;
		}
		const CommonUtilities::Vector3f out = in->GetTransform()->GetPosition();
		SetPinData("Out", out);
		return NoExec();
	}
	return Error("GameObject was null");
}

IMPLEMENT_ACTOR_NODE(SetPosition)
{
	CreateExecPin("In", PinDirection::Input, false);
	CreateDataPin<GameObject*>("Actor", PinDirection::Input, true);
	CreateDataPin<CommonUtilities::Vector3f>("Vector", PinDirection::Input, true);

	CreateExecPin("Out", PinDirection::Output, false);
}
IMPLEMENT_ACTOR_OPERATION(SetPosition)
{
	GameObject* actor;
	CommonUtilities::Vector3f vector;

	if (GetPinData("Actor", actor) && GetPinData("Vector", vector))
	{
		if(actor == nullptr)
		{
			actor = static_cast<GameObject*>(reinterpret_cast<ScriptGraph*>(GetOwner())->GetOwner());;
		}
		actor->GetTransform()->SetPosition(vector);
		return ExecPin("Out");
	}
	return Error("GameObject was null");
}

IMPLEMENT_ACTOR_NODE(AddPosition)
{
	CreateExecPin("In", PinDirection::Input, false);
	CreateDataPin<GameObject*>("Actor", PinDirection::Input, true);
	CreateDataPin<CommonUtilities::Vector3f>("Vector", PinDirection::Input, true);

	CreateExecPin("Out", PinDirection::Output, false);
}
IMPLEMENT_ACTOR_OPERATION(AddPosition)
{
	GameObject* actor;
	CommonUtilities::Vector3f vector;

	if (GetPinData("Actor", actor) && GetPinData("Vector", vector))
	{
		if(actor == nullptr)
		{
			actor = static_cast<GameObject*>(reinterpret_cast<ScriptGraph*>(GetOwner())->GetOwner());;
		}
		actor->GetTransform()->AddPosition(vector);
		return ExecPin("Out");
	}
	return Error("GameObject was null");
}
#pragma endregion
#pragma region Rotation
IMPLEMENT_ACTOR_NODE(GetRotation)
{
	AddFlag(ScriptGraphNodeFlag_Compact);
	CreateDataPin<GameObject*>("In", PinDirection::Input, true);
	CreateDataPin<CommonUtilities::Vector3f>("Out", PinDirection::Output, true);
}
IMPLEMENT_ACTOR_OPERATION(GetRotation)
{
	GameObject* in;
	if (GetPinData("In", in))
	{
		if(in == nullptr)
		{
			in = static_cast<GameObject*>(reinterpret_cast<ScriptGraph*>(GetOwner())->GetOwner());;
		}
		const CommonUtilities::Vector3f out = in->GetTransform()->GetRotationEuler();
		SetPinData("Out", out);
		return NoExec();
	}
	return Error("GameObject was null");
}

IMPLEMENT_ACTOR_NODE(SetRotation)
{
	CreateExecPin("In", PinDirection::Input, false);
	CreateDataPin<GameObject*>("Actor", PinDirection::Input, true);
	CreateDataPin<float>("Rotation_X", PinDirection::Input, true);
	CreateDataPin<float>("Rotation_Y", PinDirection::Input, true);
	CreateDataPin<float>("Rotation_Z", PinDirection::Input, true);

	CreateExecPin("Out", PinDirection::Output, false);
}
IMPLEMENT_ACTOR_OPERATION(SetRotation)
{
	GameObject* actor;
	CommonUtilities::Vector3f rotation;

	if (GetPinData("Actor", actor) && GetPinData("Rotation_X", rotation.x)&& GetPinData("Rotation_Y", rotation.y)&& GetPinData("Rotation_Z", rotation.z))
	{
		if(actor == nullptr)
		{
			actor = static_cast<GameObject*>(reinterpret_cast<ScriptGraph*>(GetOwner())->GetOwner());;
		}
		actor->GetTransform()->SetRotation(rotation);
		return ExecPin("Out");
	}
	return Error("GameObject was null");
}

IMPLEMENT_ACTOR_NODE(AddRotation)
{
	CreateExecPin("In", PinDirection::Input, false);
	CreateDataPin<GameObject*>("Actor", PinDirection::Input, true);
	CreateDataPin<float>("Rotation_X", PinDirection::Input, true);
	CreateDataPin<float>("Rotation_Y", PinDirection::Input, true);
	CreateDataPin<float>("Rotation_Z", PinDirection::Input, true);

	CreateExecPin("Out", PinDirection::Output, false);
}
IMPLEMENT_ACTOR_OPERATION(AddRotation)
{
	GameObject* actor;
	CommonUtilities::Vector3f rotation;

	if (GetPinData("Actor", actor) && GetPinData("Rotation_X", rotation.x)&& GetPinData("Rotation_Y", rotation.y)&& GetPinData("Rotation_Z", rotation.z))
	{
		if(actor == nullptr)
		{
			actor = static_cast<GameObject*>(reinterpret_cast<ScriptGraph*>(GetOwner())->GetOwner());;
		}
		actor->GetTransform()->AddRotation(rotation);
		return ExecPin("Out");
	}
	return Error("GameObject was null");
}
#pragma endregion