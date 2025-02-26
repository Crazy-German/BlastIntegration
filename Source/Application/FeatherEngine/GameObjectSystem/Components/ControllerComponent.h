#pragma once
#include "Component.h"

#include "CommonUtilities\Matrix.hpp"
#include "CommonUtilities\InputObserver.h"

#define CONTROLLER_COMPONENT 5
class ControllerComponent : public Component, public InputObserver
{
public:
	ControllerComponent();
	~ControllerComponent();

	void Awake() override;
	void AddAction(ActionEventID aGameAction);

	void Update(const float& aDeltaTime) override;

	void RecieveEvent(const ActionEvent& anEvent) override;

	void AddSpeed(float aSpeedValue);
	float GetSpeed() const { return mySpeed; }

	//please don't override setgameobject

	//Returns the world position the mouse left clicks on
	CU::Vector3f GetMouseWorldPosition();
	CU::Vector3f GetNavMeshPosition();

	//Returns the game object the mouse left clicks on
	GameObject* GetMouseGameObject();
private:
	void Move(const float& aDeltaTime);
	
private:
	bool myIsRotating = false;
	CU::Vector4f myDirection = { 0.0f, 0.0f, 0.0f , 0.0f};
	float mySpeed;
	float myRotationSpeed;
	CU::Vector3f myRotation = { 0,0,0 };
};

