#pragma once
#include <Windows.h>
#include <bitset>
#include <Xinput.h>
#include "Vector2.hpp"

namespace CommonUtilities
{


	class XInputWrapper
	{
	public:
		XInputWrapper() = default;


		void Initialize();

		const bool Update();
		
		const bool CheckConnection();
		const bool CheckIsConnected(int aIndex);
		
		const bool GetButtonDown(const WORD aButton);
		const bool GetButtonPressed(const WORD aButton);
		const bool GetButtonUp(const WORD aButton);

		const CU::Vector2f& GetLeftStick();
		const CU::Vector2f& GetRightStick();


	private:

		int myControllerID;

		XINPUT_STATE myGamepadState;
		XINPUT_STATE myPreviousGamepadState;

		_XINPUT_VIBRATION myVibration;
		CU::Vector2f myDeadzone;

		CU::Vector2f myLeftStick;
		CU::Vector2f myOldLeftStick;
		CU::Vector2f myRightStick;
		CU::Vector2f myOldRightStick;

		float myLeftTrigger;
		float myOldLeftTrigger;

		float myRightTrigger;
		float myOldRightTrigger;

	};
}