#pragma once

enum class GameInput
{
	MoveLeft,
	MoveForward,
	MoveBackwards,
	MoveRight,
	MoveStick,
	Jump,
	Scream,
	Charge,
	FlyUp,
	FlyDown,
	Rotation, // Don't know what this one does, so I made two new ones for the camera
	RotationX,
	RotationY,
	Shift_Plus_Left,
	Middle_Mouse,
	Left_Click,
	Right_Click,
	One,
	Two,
	Three,
	Toggle_DirLight,
	Toggle_PointLight,
	Toggle_SpotLight,
	Change_DebugPass,

	ToggleCamLookAt,
	ToggleMoveCamera,
	ToggleLockMouse
};