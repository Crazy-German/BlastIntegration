#include "InputMapper.h"

#include <iostream>

#include "Vector2.hpp"


CommonUtilities::InputMapper::InputMapper()
{
	for (int i = 0; i < 0xff; i++)
	{
		myPopperMappedInputs.insert(std::make_pair(static_cast<Keys>(i), ActionEventID::Count));
	}
}

CommonUtilities::InputMapper::~InputMapper()
{
	myObservers->clear();
}

void CommonUtilities::InputMapper::Refresh()
{
#ifndef _RETAIL
	TranslateToEvent(GameInput::Rotation, 1.0f);
	if (CommonUtilities::Input::GetKeyHeld(Keys::A) || CommonUtilities::Input::GetKeyHeld(Keys::GAMEPAD_LEFT_THUMBSTICK_LEFT))
	{
		TranslateToEvent(GameInput::MoveLeft, 1.0f);
	}
	if (CommonUtilities::Input::GetKeyHeld(Keys::W) || CommonUtilities::Input::GetKeyHeld(Keys::GAMEPAD_LEFT_THUMBSTICK_UP))
	{
		TranslateToEvent(GameInput::MoveForward, 1.0f);
	}
	if (CommonUtilities::Input::GetKeyHeld(Keys::S) || CommonUtilities::Input::GetKeyHeld(Keys::GAMEPAD_LEFT_THUMBSTICK_DOWN))
	{
		TranslateToEvent(GameInput::MoveBackwards, 1.0f);
	}
	if (CommonUtilities::Input::GetKeyHeld(Keys::D) || CommonUtilities::Input::GetKeyHeld(Keys::GAMEPAD_LEFT_THUMBSTICK_RIGHT))
	{
		TranslateToEvent(GameInput::MoveRight, 1.0f);
	}
	if (CommonUtilities::Input::GetKeyHeld(Keys::E))
	{
		TranslateToEvent(GameInput::FlyUp, 1.0f);
	}
	if (CommonUtilities::Input::GetKeyHeld(Keys::Q))
	{
		TranslateToEvent(GameInput::FlyDown, 1.0f);
	}

	if (CommonUtilities::Input::GetKeyUp(Keys::N))
	{
		TranslateToEvent(GameInput::ToggleLockMouse, 1.0f);
	}
	if (CommonUtilities::Input::GetKeyUp(Keys::M))
	{
		TranslateToEvent(GameInput::ToggleMoveCamera, 1.0f);
	}
#endif
	if (CommonUtilities::Input::GetKeyHeld(Keys::MOUSELBUTTON))
	{
		TranslateToEvent(GameInput::Left_Click, 1.0f);
	}
	if (CommonUtilities::Input::GetKeyHeld(Keys::MOUSELBUTTON) || CommonUtilities::Input::GetKeyHeld(Keys::SPACE) || CommonUtilities::Input::GetKeyHeld(Keys::GAMEPAD_A))
	{
		TranslateToEvent(GameInput::Jump, 1.0f);
	}
	if (CommonUtilities::Input::GetKeyHeld(Keys::MOUSEMBUTTON))
	{
		TranslateToEvent(GameInput::Middle_Mouse, 1.0f);
	}
	if (CommonUtilities::Input::GetKeyDown(Keys::MOUSERBUTTON))
	{
		TranslateToEvent(GameInput::Right_Click, 1.0f);
	}
	if (CommonUtilities::Input::GetKeyHeld(Keys::MOUSERBUTTON) || CommonUtilities::Input::GetKeyHeld(Keys::E) || CommonUtilities::Input::GetKeyHeld(Keys::GAMEPAD_B))
	{
		TranslateToEvent(GameInput::Scream, 1.0f);
	}
	if (CommonUtilities::Input::GetKeyHeld(Keys::MOUSEMBUTTON) || CommonUtilities::Input::GetKeyHeld(Keys::CONTROL) || CommonUtilities::Input::GetKeyHeld(Keys::GAMEPAD_X))
	{
		TranslateToEvent(GameInput::Charge, 1.0f);
	}

	if (CommonUtilities::Input::GetKeyDown(Keys::F1))
	{
		TranslateToEvent(GameInput::Toggle_DirLight, 1.0f);
	}
	if (CommonUtilities::Input::GetKeyDown(Keys::F2))
	{
		TranslateToEvent(GameInput::Toggle_PointLight, 1.0f);
	}
	if (CommonUtilities::Input::GetKeyDown(Keys::B))
	{
		TranslateToEvent(GameInput::ToggleCamLookAt, 1.0f);
	}
	if (CommonUtilities::Input::GetKeyDown(Keys::F3))
	{
		TranslateToEvent(GameInput::Toggle_SpotLight, 1.0f);
	}
	for (int i = 0; i < 0xff; i++)
	{
		if (CommonUtilities::Input::GetKeyDown(static_cast<Keys>(i)) || CommonUtilities::Input::GetKeyHeld(static_cast<Keys>(i)))
		{
			TranslateToEvent(static_cast<Keys>(i), 1.0f);
		}
	}

	Vector2f deltaRotation = { static_cast<float>(CommonUtilities::Input::GetMousePositionDelta().x), static_cast<float>(CommonUtilities::Input::GetMousePositionDelta().y) };
	if (deltaRotation.LengthSqr() > 0.01f)
	{
		TranslateToEvent(GameInput::RotationX, deltaRotation.x);
		TranslateToEvent(GameInput::RotationY, deltaRotation.y);
	}

	if (Input::GetKeyDown(Keys::F6))
	{
		TranslateToEvent(GameInput::Change_DebugPass, 1.0f);
	}
}

CommonUtilities::InputMapper& CommonUtilities::InputMapper::GetInputMapper()
{
	static CommonUtilities::InputMapper localInputMapper;
	return localInputMapper;
}


void CommonUtilities::InputMapper::PostEvent(const ActionEvent& anEvent)
{
	if (myObservers[static_cast<int>(anEvent.Id)].size() <= 0)
	{
		return;
	}
	for (auto& observer : myObservers[static_cast<int>(anEvent.Id)])
	{
		if (observer != nullptr)
		{
			observer->RecieveEvent(anEvent);
		}
	}
}

void CommonUtilities::InputMapper::Subscribe(const ActionEventID& anEventType, InputObserver* anObserver)
{
	myObservers[static_cast<int>(anEventType)].push_back(anObserver);
}

void CommonUtilities::InputMapper::UnRegister(const ActionEventID& anEventType, InputObserver* anObserver)
{
	auto list = myObservers[static_cast<int>(anEventType)];
	for (size_t index = 0; index < list.size(); ++index)
	{
		if (list[index] == anObserver)
		{
			list[index] = list[list.size() - 1];
			list.pop_back();
			return;
		}
	}
}

void CommonUtilities::InputMapper::UnRegisterFromAll(InputObserver* anObserver)
{
	for (auto& subscriberVector : myObservers)
	{
		for (auto& nameObserverPair : subscriberVector)
		{
			auto& observer = nameObserverPair;

			if (observer == anObserver)
			{
				std::swap(nameObserverPair, subscriberVector.back());
				subscriberVector.pop_back();
				continue;
			}
		}
	}
}

void CommonUtilities::InputMapper::BindEvent(const GameInput& anInput, const ActionEventID& anAction)
{
	myMappedInputs.insert(std::make_pair(anInput, anAction));
}

void CommonUtilities::InputMapper::BindKey(const Keys& anInput, const ActionEventID& anAction)
{
	myPopperMappedInputs.at(anInput) = anAction;
}

void CommonUtilities::InputMapper::TranslateToEvent(const GameInput& anInput, const float& aWeight)
{
	if (myMappedInputs.find(anInput) != myMappedInputs.end())
	{
		ActionEvent event = {};
		event.Id = myMappedInputs.at(anInput);
		event.Weight = aWeight;
		PostEvent(event);
	}
}

void CommonUtilities::InputMapper::TranslateToEvent(const Keys& anInput, const float& aWeight)
{
	if (myPopperMappedInputs.at(anInput) != ActionEventID::Count)
	{
		ActionEvent event = {};
		event.Id = myPopperMappedInputs.at(anInput);
		event.Weight = aWeight;
		PostEvent(event);
	}
}
