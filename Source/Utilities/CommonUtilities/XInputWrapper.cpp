#include "Input.h"
#include <iostream>
#include <Windowsx.h>
#include "XInputWrapper.h"
#include "Xinput.h"
#pragma comment(lib, "XInput.lib")
#pragma comment(lib, "Xinput9_1_0.lib")


void CommonUtilities::XInputWrapper::Initialize()
{
	myDeadzone.x = 0.1f;
	myDeadzone.y = 0.1f;
}

const bool CommonUtilities::XInputWrapper::Update()
{
	if (myControllerID == -1)
		CheckConnection(); 

	if (myControllerID != -1)
	{
		memcpy_s(&myPreviousGamepadState, sizeof(XINPUT_STATE), &myGamepadState, sizeof(XINPUT_STATE));
		ZeroMemory(&myGamepadState, sizeof(XINPUT_STATE));
		if (XInputGetState(myControllerID, &myGamepadState) != ERROR_SUCCESS)
		{
			myControllerID = -1;
			return false;
		}

		float normLX = fmaxf(-1, (float)myGamepadState.Gamepad.sThumbLX / 32767);
		float normLY = fmaxf(-1, (float)myGamepadState.Gamepad.sThumbLY / 32767);

		myLeftStick.x = (fabs(normLX) < myDeadzone.x ? 0 : (fabs(normLX) - myDeadzone.x) * (normLX / fabs(normLX)));
		myLeftStick.y = (fabs(normLY) < myDeadzone.y ? 0 : (fabs(normLY) - myDeadzone.y) * (normLY / fabs(normLY)));

		if (myDeadzone.x > 0) myLeftStick.x *= 1 / (1 - myDeadzone.x);
		if (myDeadzone.y > 0) myLeftStick.y *= 1 / (1 - myDeadzone.x);

		float normRX = fmaxf(-1, (float)myGamepadState.Gamepad.sThumbRX / 32767);
		float normRY = fmaxf(-1, (float)myGamepadState.Gamepad.sThumbRY / 32767);

		myRightStick.x = (fabs(normRX) < myDeadzone.x ? 0 : (fabs(normRX) - myDeadzone.x) * (normRX / fabs(normRX)));
		myRightStick.y = (fabs(normRY) < myDeadzone.y ? 0 : (fabs(normRY) - myDeadzone.y) * (normRY / fabs(normRY)));

		if (myDeadzone.x > 0) myRightStick.x *= 1 / (1 - myDeadzone.x);
		if (myDeadzone.y > 0) myRightStick.y *= 1 / (1 - myDeadzone.y);

		myLeftTrigger = (float)myGamepadState.Gamepad.bLeftTrigger / 255;
		myRightTrigger = (float)myGamepadState.Gamepad.bRightTrigger / 255;

		return true;
	}
	return false;
}

const bool CommonUtilities::XInputWrapper::CheckConnection()
{
	return false;
}

const bool CommonUtilities::XInputWrapper::CheckIsConnected(int aIndex)
{
	aIndex;
	return false;
}

const bool CommonUtilities::XInputWrapper::GetButtonDown(const WORD aButton)
{
	return (((myGamepadState.Gamepad.wButtons & aButton) != 0) && ((myPreviousGamepadState.Gamepad.wButtons & aButton) == 0));
}

const bool CommonUtilities::XInputWrapper::GetButtonPressed(const WORD aButton)
{
	return (myGamepadState.Gamepad.wButtons & aButton) != 0;
}

const bool CommonUtilities::XInputWrapper::GetButtonUp(const WORD aButton)
{
	return (((myPreviousGamepadState.Gamepad.wButtons & aButton) != 0) && ((myGamepadState.Gamepad.wButtons & aButton) == 0));
}

const CU::Vector2f& CommonUtilities::XInputWrapper::GetLeftStick()
{
	return myLeftStick;
}

const CU::Vector2f& CommonUtilities::XInputWrapper::GetRightStick()
{
	return myRightStick;
}



