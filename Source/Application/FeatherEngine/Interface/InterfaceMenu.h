#pragma once
#include "GraphicsEngine/Sprite2D.h"
#include <array>
#include <AssetManagement/AssetTypes/Sprite.h>
#include "InterfaceComponent.h"

class InterfaceMenu
{
public:
	InterfaceMenu();
	~InterfaceMenu();

	void SetName(const std::string& aName);

	void Update(float aDeltaTime);

	//Returns false if mouse click was not made on a sprite contained in the current menu
	bool Input(CU::Vector2f aMousePosition);

	void AddInterfaceComponent(std::shared_ptr<InterfaceComponent> aComponent, bool aInterfaceable = false);

	void Draw();
	void StartCooldown();
	void Reset();

	const std::string& GetName() const;

private:

	const float myInputCooldown = 0.2f;
	float myRemainingInputCooldown = 0.2f;
	std::string myName;
	std::vector<std::shared_ptr<InterfaceComponent>> myComponents;
	std::vector < std::shared_ptr<InterfaceComponent>> myInteractables;

	void MoveSelectionDown();
	void MoveSelectionUp();

	bool myUsedMouseInput;

	int mySelectionIndex;
	CU::Vector2f myLastMousePosition;
	void SwitchInputType(bool aMouse);
	float myDeltaTime; //EVIL!

};