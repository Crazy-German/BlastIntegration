#include "FeatherEngine.pch.h"
#include "InterfaceMenu.h"
#include <CommonUtilities/Input.h>
#include <MainSingleton.h>

InterfaceMenu::InterfaceMenu()
{
	myRemainingInputCooldown = 0.0f;
	mySelectionIndex = 0;
	myUsedMouseInput = false;
}

InterfaceMenu::~InterfaceMenu()
{

}

void InterfaceMenu::SetName(const std::string& aName)
{
	myName = aName;
}

void InterfaceMenu::Update(float aDeltaTime)
{
	myDeltaTime = aDeltaTime;
	if (myRemainingInputCooldown >= 0.0f)
	{
		std::cout << myRemainingInputCooldown;
		myRemainingInputCooldown -= aDeltaTime;
	}
	for (int i = 0; i < myComponents.size(); i++)
	{
		myComponents[i]->Update(aDeltaTime);
	}
}

bool InterfaceMenu::Input(CU::Vector2f aMousePosition)
{
	if (!(myLastMousePosition.x == 0 && myLastMousePosition.y == 0))
	{
		if (myLastMousePosition != aMousePosition)
		{
			myUsedMouseInput = true;
		}
	}
	
	myLastMousePosition = aMousePosition;

	for (int i = 0; i < myInteractables.size(); i++)
	{
		if (myInteractables[i]->GetInteractionState() != InteractionState::Clicked)
		{
			myInteractables[i]->SetInteractionState(InteractionState::None);
		}
	}

	if (!myInteractables.empty())
	{
		
		if (myInteractables[mySelectionIndex]->GetInteractionState() != InteractionState::Clicked && !myUsedMouseInput)
		{
			myInteractables[mySelectionIndex]->SetInteractionState(InteractionState::Selected);
		}

		if (CU::Input::GetKeyDown(CU::Keys::W) || CU::Input::myXInput.GetButtonDown(XINPUT_GAMEPAD_DPAD_UP))
		{
			MoveSelectionUp();
			SwitchInputType(true);
		}

		if (CU::Input::GetKeyDown(CU::Keys::S) || CU::Input::myXInput.GetButtonDown(XINPUT_GAMEPAD_DPAD_DOWN))
		{
			MoveSelectionDown();
			SwitchInputType(true);
		}

		if (CU::Input::GetKeyHeld(CU::Keys::A))
		{
			myInteractables[mySelectionIndex]->HorizontalMoveActionContinuous(-1.0f * myDeltaTime);
			SwitchInputType(true);
		}

		if (CU::Input::GetKeyHeld(CU::Keys::D))
		{
			myInteractables[mySelectionIndex]->HorizontalMoveActionContinuous(1.0f * myDeltaTime);
			SwitchInputType(true);
		}

		float joystickInput = CU::Input::myXInput.GetLeftStick().x;
		if (abs(joystickInput) > 0.1f)
		{
			myInteractables[mySelectionIndex]->HorizontalMoveActionContinuous(joystickInput * myDeltaTime);
			SwitchInputType(true);
		}
		
		if (CU::Input::GetKeyDown(CU::Keys::A) || CU::Input::myXInput.GetButtonDown(XINPUT_GAMEPAD_DPAD_LEFT))
		{
			myInteractables[mySelectionIndex]->HorizontalMoveActionStep(-1);
			SwitchInputType(true);
		}

		if (CU::Input::GetKeyDown(CU::Keys::D) || CU::Input::myXInput.GetButtonDown(XINPUT_GAMEPAD_DPAD_RIGHT))
		{
			myInteractables[mySelectionIndex]->HorizontalMoveActionStep(1);
			SwitchInputType(true);
		}

		if (CU::Input::GetKeyDown(CU::Keys::SPACE) || CU::Input::myXInput.GetButtonDown(XINPUT_GAMEPAD_A))
		{
			myInteractables[mySelectionIndex]->ClickAction();
			SwitchInputType(true);
		}
	}

	

	if (myUsedMouseInput)
	{
		for (int i = static_cast<int>(myComponents.size()) - 1; i >= 0; i--)
		{
			if (myComponents[i]->IsHovered(aMousePosition, myUsedMouseInput))
			{
				myComponents[i]->HoverAction();
				//do thing with component?
				if (CU::Input::GetMouseButtonPressed(CU::MouseButtons::Left))
				{
					if (myRemainingInputCooldown <= 0.0f)
					{
						myComponents[i]->ClickAction(aMousePosition);
					}
				}
				return true;
			}
		}
		return false;
	}
	return false;
}

void InterfaceMenu::AddInterfaceComponent(std::shared_ptr<InterfaceComponent> aComponent, bool aInterfaceable)
{
	myComponents.push_back(aComponent);
	if (aInterfaceable)
	{
		myInteractables.push_back(aComponent);
	}
}

void InterfaceMenu::Draw()
{
	for (int i = 0; i < myComponents.size(); i++)
	{
		myComponents[i]->Draw();
	}
}

void InterfaceMenu::StartCooldown()
{
	myRemainingInputCooldown = myInputCooldown;
}

void InterfaceMenu::Reset()
{
	if (!myComponents.empty())
	{
		for (int i = 0; i < myComponents.size(); i++)
		{
			myComponents[i] = nullptr;
		}
		myComponents.clear();
	}
	
}

const std::string& InterfaceMenu::GetName() const
{
	return myName;
}

void InterfaceMenu::MoveSelectionDown()
{
	mySelectionIndex++;
	if (mySelectionIndex >= myInteractables.size())
	{
		mySelectionIndex = 0;
	}
}

void InterfaceMenu::MoveSelectionUp()
{
	mySelectionIndex--;
	if (mySelectionIndex < 0)
	{
		mySelectionIndex = static_cast<int>(myInteractables.size() - 1);
	}
}

void InterfaceMenu::SwitchInputType(bool aMouse)
{
	aMouse;
	if (myUsedMouseInput)
	{
		mySelectionIndex = 0;
	}
	myUsedMouseInput = false;
}
