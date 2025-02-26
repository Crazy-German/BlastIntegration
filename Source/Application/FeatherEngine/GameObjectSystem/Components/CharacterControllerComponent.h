#pragma once
#include "Collision/ColliderComponentBase.h"
#include "Physcics/CharacterController.h"

#define CHARACTER_CONTROLLER_COMPONENT 500

class CharacterControllerComponent : public Squish::CharacterController ,public ColliderComponentBase
{
public:
	CharacterControllerComponent();
	~CharacterControllerComponent() override;
	void Destroy() override;
	void Awake() override;
	void Update(const float& aDeltaTime) override;
	const Squish::ActorUserData* GetUserData() const override;
};

