#pragma once
#include "Vector3.hpp"
enum class ActionEventID
{
	MoveLeft,
	MoveRight,
	MoveForward,
	MoveBackwards,
	MoveStick,
	Left_Click,
	Walk,
	Attack,
	FlyUp,
	FlyDown,
	Rotation,
	CameraMovedX,
	CameraMovedY,
	Toggle_DirLight,
	Toggle_PointLight,
	Toggle_SpotLight,
	Jump,
	Scream,
	Charge,
	Change_DebugPass,
	EnableColliderDebug,
	RenderNavMesh,
	MoveCamLeft,
	MoveCamRight,
	MoveCamForward,
	MoveCamBackwards,
	Count,
};

struct ActionEvent
{
	ActionEventID Id;
	float Weight;
};
