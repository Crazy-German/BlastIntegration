#include "Input.h"
#include <iostream>
#include <Windowsx.h>

namespace CommonUtilities
{
	XInputWrapper Input::myXInput;

	std::bitset<256> Input::myKeysDown{};
	std::bitset<256> Input::myKeysLastFrameDown{};
	std::bitset<256> Input::myKeys{};
	std::bitset<256> Input::myKeysPrev{};
	std::bitset<256> Input::myKeysPrevPast{};
	
	POINT Input::myMousePosition;
	POINT Input::myMousePositionPrev;
	POINT Input::myMouseDelta;
	int Input::myMouseWheelDelta;
	POINT Input::myMouseAbsolutPosition;

	POINT Input::myMouseLockPosition;
	bool Input::myMouseIsCenterLocked;

	std::string Input::myStringInputBuffer;
	std::string Input::myStringInputBufferPrev;

	void Input::Awake()
	{
		myXInput.Initialize();
		for (int i = 0; i < 256; i++)
		{
			myKeys[i] = false;
			myKeysDown[i] = false;
			myKeysLastFrameDown[i] = false;
			myKeysPrev[i] = false;
			myKeysPrevPast[i] = false;
		}
		myMousePosition = { 0, 0 };
		myMousePositionPrev = { 0, 0 };
		myMouseDelta = { 0, 0 };
		myMouseWheelDelta = 0;
		myMouseAbsolutPosition = { 0, 0 };
	}

	bool Input::HandleEvents(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_KEYDOWN:
		{
			WORD keyFlags = HIWORD(lParam);
			if (!(keyFlags & KF_REPEAT))
			{
				myKeysDown[wParam] = true;
				InternalStringInputHandler(message, wParam, lParam);
			}
			myKeys[wParam] = true;
			break;
		}
		case WM_KEYUP:
			myKeys[wParam] = false;
			break;
		case WM_MOUSEMOVE:
			myMousePosition.x = GET_X_LPARAM(lParam);
			myMousePosition.y = GET_Y_LPARAM(lParam);
			break;
		case WM_LBUTTONDOWN:
			myKeysDown[VK_LBUTTON] = true;
			myKeys[VK_LBUTTON] = true;
			break;
		case WM_LBUTTONUP:
			myKeys[VK_LBUTTON] = false;
			break;
		case WM_RBUTTONDOWN:
			myKeysDown[VK_RBUTTON] = true;
			myKeys[VK_RBUTTON] = true;
			break;
		case WM_RBUTTONUP:
			myKeys[VK_RBUTTON] = false;
			break;
		case WM_MBUTTONDOWN:
			myKeysDown[VK_MBUTTON] = true;
			myKeys[VK_MBUTTON] = true;
			break;
		case WM_MBUTTONUP:
			myKeys[VK_MBUTTON] = false;
			break;
		case WM_MOUSEWHEEL:
			myMouseWheelDelta += GET_WHEEL_DELTA_WPARAM(wParam);
			break;
		default:
			return false;
		}
		return true;
	}

	LRESULT Input::BuiltInWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (Input::HandleEvents(message, wParam, lParam))
			return 0;

		lParam;
		wParam;
		hWnd;
		switch (message)
		{
			// this message is read when the window is closed
			case WM_DESTROY:
			{
				// close the application entirely
				PostQuitMessage(0);
				return 0;
			}
			default:
				break;
		}

		return 0;
	}

	void Input::Update()
	{
		myXInput.Update();
		for (int i = 0; i < 256; i++)
		{
			myKeysPrevPast[i] = myKeysPrev[i];
			myKeysPrev[i] = myKeys[i];
			myKeysLastFrameDown[i] = myKeysDown[i];
			myKeysDown[i] = false;
		}
		myMouseWheelDelta = 0;

		if (myMouseIsCenterLocked)
		{
			POINT mousePos;
			GetCursorPos(&mousePos);
			SetCursorPos(myMouseLockPosition.x, myMouseLockPosition.y);
			myMouseDelta.x = myMouseLockPosition.x - mousePos.x;
			myMouseDelta.y = myMouseLockPosition.y - mousePos.y;
		}
		else
		{
			myMouseDelta.x = myMousePosition.x - myMousePositionPrev.x;
			myMouseDelta.y = myMousePosition.y - myMousePositionPrev.y;
		}

		myMousePositionPrev = myMousePosition;
		myStringInputBufferPrev = myStringInputBuffer;
		myStringInputBuffer.clear();
	}

	bool Input::GetKeyDown(const int& aKeyCode)
	{
		return myKeysLastFrameDown[aKeyCode];
	}

	bool Input::GetKeyDown(const Keys& aKeyCode)
	{
		return myKeysLastFrameDown[static_cast<int>(aKeyCode)];
	}

	bool Input::GetKeyHeld(const int& aKeyCode)
	{
		return myKeys[aKeyCode];
	}

	bool Input::GetKeyHeld(const Keys& aKeyCode)
	{
		return myKeys[static_cast<int>(aKeyCode)];
	}

	bool Input::GetKeyUp(const int& aKeyCode)
	{
		return !myKeysPrev[aKeyCode] && myKeysPrevPast[aKeyCode];
	}

	bool Input::GetKeyUp(const Keys& key)
	{
		return !myKeysPrev[static_cast<int>(key)] && myKeysPrevPast[static_cast<int>(key)];
	}

	bool Input::GetMouseButtonDown(const int& aButton)
	{
		return myKeysLastFrameDown[aButton];
	}

	bool Input::GetMouseButtonDown(const MouseButtons& aButton)
	{
		return myKeysLastFrameDown[static_cast<int>(aButton)];
	}

	bool Input::GetMouseButtonPressed(const int& aButton)
	{
		return myKeys[aButton];
	}

	bool Input::GetMouseButtonPressed(const MouseButtons& aButton)
	{
		return myKeys[static_cast<int>(aButton)];
	}

	bool Input::GetMouseButtonUp(const int& aButton)
	{
		return !myKeysPrev[aButton] && myKeysPrevPast[aButton];
	}

	bool Input::GetMouseButtonUp(const MouseButtons& aButton)
	{
		return !myKeysPrev[static_cast<int>(aButton)] && myKeysPrevPast[static_cast<int>(aButton)];
	}

	POINT Input::GetMousePosition()
	{
		return myMousePosition;
	}

	POINT Input::GetMousePositionDelta()
	{
		return myMouseDelta;
	}

	POINT Input::GetMouseAbsolutePosition()
	{
		GetCursorPos(&myMouseAbsolutPosition);
		return myMouseAbsolutPosition;
	}

	int Input::GetMouseWheelDelta()
	{
		return myMouseWheelDelta;
	}

	void Input::LockMouseToWindow(const bool& aShouldLock, const HWND& aWindowHandle)
	{
		if (aShouldLock)
		{
			RECT rect;
			GetClientRect(aWindowHandle, &rect);

			POINT ul;
			ul.x = rect.left;
			ul.y = rect.top;

			POINT lr;
			lr.x = rect.right;
			lr.y = rect.bottom;
			MapWindowPoints(aWindowHandle, nullptr, &ul, 1);
			MapWindowPoints(aWindowHandle, nullptr, &lr, 1);

			rect.left = ul.x;
			rect.top = ul.y;

			rect.right = lr.x;
			rect.bottom = lr.y;

			ClipCursor(&rect);
		}
		else
		{
			ClipCursor(nullptr);
		}
	}

	void Input::SetMousePosition(const POINT& aPosition)
	{
		SetCursorPos(aPosition.x, aPosition.y);
	}

	void Input::LockMouseToCenter(const bool& aShouldLock, const HWND& aWindowHandle)
	{
		if (aShouldLock)
		{
			RECT rect;
			GetClientRect(aWindowHandle, &rect);
			POINT ul;
			ul.x = rect.left;
			ul.y = rect.top;
			POINT lr;
			lr.x = rect.right;
			lr.y = rect.bottom;
			MapWindowPoints(aWindowHandle, nullptr, &ul, 1);
			MapWindowPoints(aWindowHandle, nullptr, &lr, 1);
			rect.left = ul.x;
			rect.top = ul.y;
			rect.right = lr.x;
			rect.bottom = lr.y;
			POINT center;
			center.x = (rect.left + rect.right) / 2;
			center.y = (rect.top + rect.bottom) / 2;
			myMouseLockPosition = center;
			SetCursorPos(center.x, center.y);

			myMouseIsCenterLocked = true;
		}
		else
		{
			myMouseIsCenterLocked = false;
		}
	}

	std::string Input::GetStringInputBuffer()
	{
		return myStringInputBufferPrev;
	}


	const char* Input::KeyCodeToString(const Keys& aKeyCode)
	{

	    switch (aKeyCode)
	    {
	    case Keys::MOUSELBUTTON:
	        return "MOUSELBUTTON";
	        break;
	    case Keys::MOUSERBUTTON:
	        return "MOUSERBUTTON";
	        break;
	    case Keys::CANCEL:
	        return "CANCEL";
	        break;
	    case Keys::MOUSEMBUTTON:
	        return "MBUTTON";
	        break;
	    case Keys::MOUSEXBUTTON1:
	        return "MOUSEXBUTTON1";
	        break;
	    case Keys::MOUSEXBUTTON2:
	        return "MOUSEXBUTTON2";
	        break;
	    case Keys::BACK:
	        return "BACK";
	        break;
	    case Keys::TAB:
	        return "TAB";
	        break;
	    case Keys::CLEAR:
	        return "CLEAR";
	        break;
	    case Keys::RETURN:
	        return "RETURN";
	        break;
	    case Keys::SHIFT:
	        return "SHIFT";
	        break;
	    case Keys::CONTROL:
	        return "CONTROL";
	        break;
	    case Keys::MENU:
	        return "MENU";
	        break;
	    case Keys::PAUSE:
	        return "PAUSE";
	        break;
	    case Keys::CAPITAL:
	        return "CAPITAL";
	        break;
	    case Keys::KANA:
	        return "KANA";
	        break;
	    case Keys::JUNJA:
	        return "JUNJA";
	        break;
	    case Keys::FINAL:
	        return "FINAL";
	        break;
	    case Keys::HANJA:
	        return "HANJA";
	        break;
	    case Keys::ESCAPE:
	        return "ESCAPE";
	        break;
	    case Keys::CONVERT:
	        return "CONVERT";
	        break;
	    case Keys::NONCONVERT:
	        return "NONCONVERT";
	        break;
	    case Keys::ACCEPT:
	        return "ACCEPT";
	        break;
	    case Keys::MODECHANGE:
	        return "MODECHANGE";
	        break;
	    case Keys::SPACE:
	        return "SPACE";
	        break;
	    case Keys::PRIOR:
	        return "PRIOR";
	        break;
	    case Keys::NEXT:
	        return "NEXT";
	        break;
	    case Keys::END:
	        return "END";
	        break;
	    case Keys::HOME:
	        return "HOME";
	        break;
	    case Keys::LEFT:
	        return "LEFT";
	        break;
	    case Keys::UP:
	        return "UP";
	        break;
	    case Keys::RIGHT:
	        return "RIGHT";
	        break;
	    case Keys::DOWN:
	        return "DOWN";
	        break;
	    case Keys::SELECT:
	        return "SELECT";
	        break;
	    case Keys::PRINT:
	        return "PRINT";
	        break;
	    case Keys::EXECUTE:
	        return "EXECUTE";
	        break;
	    case Keys::SNAPSHOT:
	        return "SNAPSHOT";
	        break;
	    case Keys::INSERT:
	        return "INSERT";
	        break;
	    case Keys::DELETE_BUTTON:
	        return "DELETE_BUTTON";
	        break;
	    case Keys::HELP:
	        return "HELP";
	        break;
	    case Keys::A:
	        return "A";
	        break;
	    case Keys::B:
	        return "B";
	        break;
	    case Keys::C:
	        return "C";
	        break;
	    case Keys::D:
	        return "D";
	        break;
	    case Keys::E:
	        return "E";
	        break;
	    case Keys::F:
	        return "F";
	        break;
	    case Keys::G:
	        return "G";
	        break;
	    case Keys::H:
	        return "H";
	        break;
	    case Keys::I:
	        return "I";
	        break;
	    case Keys::J:
	        return "J";
	        break;
	    case Keys::K:
	        return "K";
	        break;
	    case Keys::L:
	        return "L";
	        break;
	    case Keys::M:
	        return "M";
	        break;
	    case Keys::N:
	        return "N";
	        break;
	    case Keys::O:
	        return "O";
	        break;
	    case Keys::P:
	        return "P";
	        break;
	    case Keys::Q:
	        return "Q";
	        break;
	    case Keys::R:
	        return "R";
	        break;
	    case Keys::S:
	        return "S";
	        break;
	    case Keys::T:
	        return "T";
	        break;
	    case Keys::U:
	        return "U";
	        break;
	    case Keys::V:
	        return "V";
	        break;
	    case Keys::W:
	        return "W";
	        break;
	    case Keys::X:
	        return "X";
	        break;
	    case Keys::Y:
	        return "Y";
	        break;
	    case Keys::Z:
	        return "Z";
	        break;
	    case Keys::LWIN:
	        return "LWIN";
	        break;
	    case Keys::RWIN:
	        return "RWIN";
	        break;
	    case Keys::APPS:
	        return "APPS";
	        break;
	    case Keys::SLEEP:
	        return "SLEEP";
	        break;
	    case Keys::NUMPAD0:
	        return "NUMPAD0";
	        break;
	    case Keys::NUMPAD1:
	        return "NUMPAD1";
	        break;
	    case Keys::NUMPAD2:
	        return "NUMPAD2";
	        break;
	    case Keys::NUMPAD3:
	        return "NUMPAD3";
	        break;
	    case Keys::NUMPAD4:
	        return "NUMPAD4";
	        break;
	    case Keys::NUMPAD5:
	        return "NUMPAD5";
	        break;
	    case Keys::NUMPAD6:
	        return "NUMPAD6";
	        break;
	    case Keys::NUMPAD7:
	        return "NUMPAD7";
	        break;
	    case Keys::NUMPAD8:
	        return "NUMPAD8";
	        break;
	    case Keys::NUMPAD9:
	        return "NUMPAD9";
	        break;
	    case Keys::MULTIPLY:
	        return "MULTIPLY";
	        break;
	    case Keys::ADD:
	        return "ADD";
	        break;
	    case Keys::SEPARATOR:
	        return "SEPARATOR";
	        break;
	    case Keys::SUBTRACT:
	        return "SUBTRACT";
	        break;
	    case Keys::DECIMAL:
	        return "DECIMAL";
	        break;
	    case Keys::DIVIDE:
	        return "DIVIDE";
	        break;
	    case Keys::F1:
	        return "F1";
	        break;
	    case Keys::F2:
	        return "F2";
	        break;
	    case Keys::F3:
	        return "F3";
	        break;
	    case Keys::F4:
	        return "F4";
	        break;
	    case Keys::F5:
	        return "F5";
	        break;
	    case Keys::F6:
	        return "F6";
	        break;
	    case Keys::F7:
	        return "F7";
	        break;
	    case Keys::F8:
	        return "F8";
	        break;
	    case Keys::F9:
	        return "F9";
	        break;
	    case Keys::F10:
	        return "F10";
	        break;
	    case Keys::F11:
	        return "F11";
	        break;
	    case Keys::F12:
	        return "F12";
	        break;
	    case Keys::F13:
	        return "F13";
	        break;
	    case Keys::F14:
	        return "F14";
	        break;
	    case Keys::F15:
	        return "F15";
	        break;
	    case Keys::F16:
	        return "F16";
	        break;
	    case Keys::F17:
	        return "F17";
	        break;
	    case Keys::F18:
	        return "F18";
	        break;
	    case Keys::F19:
	        return "F19";
	        break;
	    case Keys::F20:
	        return "F20";
	        break;
	    case Keys::F21:
	        return "F21";
	        break;
	    case Keys::F22:
	        return "F22";
	        break;
	    case Keys::F23:
	        return "F23";
	        break;
	    case Keys::F24:
	        return "F24";
	        break;
	    case Keys::NUMLOCK:
	        return "NUMLOCK";
	        break;
	    case Keys::SCROLL:
	        return "SCROLL";
	        break;
	    case Keys::OEM_NEC_EQUAL:
	        return "OEM_NEC_EQUAL";
	        break;
	    case Keys::OEM_FJ_MASSHOU:
	        return "OEM_FJ_MASSHOU";
	        break;
	    case Keys::OEM_FJ_TOUROKU:
	        return "OEM_FJ_TOUROKU";
	        break;
	    case Keys::OEM_FJ_LOYA:
	        return "OEM_FJ_TOUROKU";
	        break;
	    case Keys::OEM_FJ_ROYA:
	        return "OEM_FJ_ROYA";
	        break;
	    case Keys::LSHIFT:
	        return "LSHIFT";
	        break;
	    case Keys::RSHIFT:
	        return "RSHIFT";
	        break;
	    case Keys::LCONTROL:
	        return "LCONTROL";
	        break;
	    case Keys::RCONTROL:
	        return "RCONTROL";
	        break;
	    case Keys::LMENU:
	        return "LMENU";
	        break;
	    case Keys::RMENU:
	        return "RMENU";
	        break;
	    case Keys::BROWSER_BACK:
	        return "BROWSER_BACK";
	        break;
	    case Keys::BROWSER_FORWARD:
	        return "BROWSER_FORWARD";
	        break;
	    case Keys::BROWSER_REFRESH:
	        return "BROWSER_REFRESH";
	        break;
	    case Keys::BROWSER_STOP:
	        return "BROWSER_STOP";
	        break;
	    case Keys::BROWSER_SEARCH:
	        return "BROWSER_SEARCH";
	        break;
	    case Keys::BROWSER_FAVORITES:
	        return "BROWSER_FAVORITES";
	        break;
	    case Keys::BROWSER_HOME:
	        return "BROWSER_HOME";
	        break;
	    case Keys::VOLUME_MUTE:
	        return "VOLUME_MUTE";
	        break;
	    case Keys::VOLUME_DOWN:
	        return "VOLUME_DOWN";
	        break;
	    case Keys::VOLUME_UP:
	        return "VOLUME_UP";
	        break;
	    case Keys::MEDIA_NEXT_TRACK:
	        return "MEDIA_NEXT_TRACK";
	        break;
	    case Keys::MEDIA_PREV_TRACK:
	        return "MEDIA_PREV_TRACK";
	        break;
	    case Keys::MEDIA_STOP:
	        return "MEDIA_STOP";
	        break;
	    case Keys::MEDIA_PLAY_PAUSE:
	        return "MEDIA_PLAY_PAUSE";
	        break;
	    case Keys::LAUNCH_MAIL:
	        return "LAUNCH_MAIL";
	        break;
	    case Keys::LAUNCH_MEDIA_SELECT:
	        return "LAUNCH_MEDIA_SELECT";
	        break;
	    case Keys::LAUNCH_APP1:
	        return "LAUNCH_APP1";
	        break;
	    case Keys::LAUNCH_APP2:
	        return "LAUNCH_APP2";
	        break;
	    case Keys::OEM_1:
	        return "OEM_1";
	        break;
	    case Keys::OEM_PLUS:
	        return "OEM_PLUS";
	        break;
	    case Keys::OEM_COMMA:
	        return "OEM_COMMA";
	        break;
	    case Keys::OEM_MINUS:
	        return "OEM_MINUS";
	        break;
	    case Keys::OEM_PERIOD:
	        return "OEM_PERIOD";
	        break;
	    case Keys::OEM_2:
	        return "OEM_2";
	        break;
	    case Keys::OEM_3:
	        return "OEM_3";
	        break;
	    case Keys::GAMEPAD_A:
	        return "GAMEPAD_A";
	        break;
	    case Keys::GAMEPAD_B:
	        return "GAMEPAD_B";
	        break;
	    case Keys::GAMEPAD_X:
	        return "GAMEPAD_X";
	        break;
	    case Keys::GAMEPAD_Y:
	        return "GAMEPAD_Y";
	        break;
	    case Keys::GAMEPAD_RIGHT_SHOULDER:
	        return "GAMEPAD_RIGHT_SHOULDER";
	        break;
	    case Keys::GAMEPAD_LEFT_SHOULDER:
	        return "GAMEPAD_LEFT_SHOULDER";
	        break;
	    case Keys::GAMEPAD_LEFT_TRIGGER:
	        return "GAMEPAD_LEFT_TRIGGER";
	        break;
	    case Keys::GAMEPAD_RIGHT_TRIGGER:
	        return "GAMEPAD_RIGHT_TRIGGER";
	        break;
	    case Keys::GAMEPAD_DPAD_UP:
	        return "GAMEPAD_DPAD_UP";
	        break;
	    case Keys::GAMEPAD_DPAD_DOWN:
	        return "GAMEPAD_DPAD_DOWN";
	        break;
	    case Keys::GAMEPAD_DPAD_LEFT:
	        return "GAMEPAD_DPAD_LEFT";
	        break;
	    case Keys::GAMEPAD_DPAD_RIGHT:
	        return "GAMEPAD_DPAD_RIGHT";
	        break;
	    case Keys::GAMEPAD_MENU:
	        return "GAMEPAD_MENU";
	        break;
	    case Keys::GAMEPAD_VIEW:
	        return "GAMEPAD_VIEW";
	        break;
	    case Keys::GAMEPAD_LEFT_THUMBSTICK_BUTTON:
	        return "GAMEPAD_LEFT_THUMBSTICK_BUTTON";
	        break;
	    case Keys::GAMEPAD_RIGHT_THUMBSTICK_BUTTON:
	        return "GAMEPAD_RIGHT_THUMBSTICK_BUTTON";
	        break;
	    case Keys::GAMEPAD_LEFT_THUMBSTICK_UP:
	        return "GAMEPAD_LEFT_THUMBSTICK_UP";
	        break;
	    case Keys::GAMEPAD_LEFT_THUMBSTICK_DOWN:
	        return "GAMEPAD_LEFT_THUMBSTICK_DOWN";
	        break;
	    case Keys::GAMEPAD_LEFT_THUMBSTICK_RIGHT:
	        return "GAMEPAD_LEFT_THUMBSTICK_RIGHT";
	        break;
	    case Keys::GAMEPAD_LEFT_THUMBSTICK_LEFT:
	        return "GAMEPAD_LEFT_THUMBSTICK_LEFT";
	        break;
	    case Keys::GAMEPAD_RIGHT_THUMBSTICK_UP:
	        return "GAMEPAD_RIGHT_THUMBSTICK_UP";
	        break;
	    case Keys::GAMEPAD_RIGHT_THUMBSTICK_DOWN:
	        return "GAMEPAD_RIGHT_THUMBSTICK_DOWN";
	        break;
	    case Keys::GAMEPAD_RIGHT_THUMBSTICK_RIGHT:
	        return "GAMEPAD_RIGHT_THUMBSTICK_RIGHT";
	        break;
	    case Keys::GAMEPAD_RIGHT_THUMBSTICK_LEFT:
	        return "GAMEPAD_RIGHT_THUMBSTICK_LEFT";
	        break;
	    case Keys::OEM_4:
	        return "OEM_4";
	        break;
	    case Keys::OEM_5:
	        return "OEM_5";
	        break;
	    case Keys::OEM_6:
	        return "OEM_6";
	        break;
	    case Keys::OEM_7:
	        return "OEM_7";
	        break;
	    case Keys::OEM_8:
	        return "OEM_8";
	        break;
	    case Keys::OEM_AX:
	        return "OEM_AX";
	        break;
	    case Keys::OEM_102:
	        return "OEM_102";
	        break;
	    case Keys::ICO_HELP:
	        return "ICO_HELP";
	        break;
	    case Keys::ICO_00:
	        return "ICO_00";
	        break;
	    case Keys::PROCESSKEY:
	        return "PROCESSKEY";
	        break;
	    case Keys::ICO_CLEAR:
	        return "ICO_CLEAR";
	        break;
	    case Keys::PACKET:
	        return "PACKET";
	        break;
	    case Keys::OEM_RESET:
	        return "OEM_RESET";
	        break;
	    case Keys::OEM_JUMP:
	        return "OEM_JUMP";
	        break;
	    case Keys::OEM_PA1:
	        return "OEM_PA1";
	        break;
	    case Keys::OEM_PA2:
	        return "OEM_PA2";
	        break;
	    case Keys::OEM_PA3:
	        return "OEM_PA3";
	        break;
	    case Keys::OEM_WSCTRL:
	        return "OEM_WSCTRL";
	        break;
	    case Keys::OEM_CUSEL:
	        return "OEM_CUSEL";
	        break;
	    case Keys::OEM_ATTN:
	        return "OEM_ATTN";
	        break;
	    case Keys::OEM_FINISH:
	        return "OEM_FINISH";
	        break;
	    case Keys::OEM_COPY:
	        return "OEM_COPY";
	        break;
	    case Keys::OEM_AUTO:
	        return "OEM_AUTO";
	        break;
	    case Keys::OEM_ENLW:
	        return "OEM_ENLW";
	        break;
	    case Keys::OEM_BACKTAB:
	        return "OEM_BACKTAB";
	        break;
	    case Keys::ATTN:
	        return "ATTN";
	        break;
	    case Keys::CRSEL:
	        return "CRSEL";
	        break;
	    case Keys::EXSEL:
	        return "EXSEL";
	        break;
	    case Keys::EREOF:
	        return "EREOF";
	        break;
	    case Keys::PLAY:
	        return "PLAY";
	        break;
	    case Keys::ZOOM:
	        return "ZOOM";
	        break;
	    case Keys::NONAME:
	        return "NO_NAME";
	        break;
	    case Keys::PA1:
	        return "PA1";
	        break;
	    case Keys::OEM_CLEAR:
	        return "OEM_CLEAR";
	        break;
        case Keys::N0:
			return "N0";
	        break;
        case Keys::N1:
			return "N1";
	        break;
        case Keys::N2:
			return "N2";
	        break;
        case Keys::N3:
			return "N3";
	        break;
        case Keys::N4:
	        return "N4";
	        break;
        case Keys::N5:
	        return "N5";
	        break;
        case Keys::N6:
	        return "N6";
	        break;
        case Keys::N7:
	        return "N7";
	        break;
        case Keys::N8:
	        return "N8";
	        break;
        case Keys::N9:
	        return "N9";
	        break;
	    default:
	        return "nullptr";
	        break;
	    }
	}

	void Input::InternalStringInputHandler(const UINT& aMessage, const WPARAM& aWParam, const LPARAM& aLParam)
	{
		aMessage;
		aLParam;
		//Filter wParams for letters and numbers include space-bar
		if (aWParam >= 0x41 && aWParam <= 0x5A)
		{
			//Check shift
			if (myKeys[VK_SHIFT])
			{
				myStringInputBuffer += static_cast<char>(aWParam);
			}
			else
			{
				myStringInputBuffer += static_cast<char>(aWParam + 0x20);
			}
		}
		else if (aWParam == 0x20)
		{
			myStringInputBuffer += " ";
		}
		else if (aWParam >= 0x30 && aWParam <= 0x40)
		{
			myStringInputBuffer += static_cast<char>(aWParam);
		}
	}

}

