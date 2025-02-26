#pragma once
#include <array>
#include <filesystem>

#include "CommonUtilities/Intersection.hpp"
#include "CommonUtilities/Input.h"
#include "CommonUtilities/InputObserver.h"
#include "GameObjectSystem/Components/Component.h"
#include "GameObjectSystem\Components\SphereColliderComponent.h"
#include "GameObjectSystem\Components\OBBColliderComponent.h"
#include <GameObjectSystem\Components\CharacterControllerComponent.h>
#include "Squish/Physcics/PhysicsMovement.h"

class RigidBodyComponent;

enum class ePlayerAnimationState : uint8_t
{
	Idle,
	Walking,
	WalkingUpSlope,
	WalkStop,
	Charging,
	JumpStart,
	Falling,
	Landing,
	Gliding,
	ChargingShout,
	ShoutingLight,
	ShoutingHeavy,
	Slide,
	GettingTape,
	count
};

enum class PlayerSoundID
{
	ComboAttack1,
	ComboAttack2,
	ComboAttack3,
	HeavyAttack,
	JumpAttack,
	WarCry,
	ProjectileAttack,
	FootStep,
	Hurt,
	Hurt2,
	Hurt3,
	Death,
	Death2,
	Death3,
	Healing,
	AbilityUnlock
};

#define PLAYER_COMPONENT 5
class PlayerComponent : public Component, public InputObserver
{
public:
	PlayerComponent();
	~PlayerComponent() override;

	void Awake() override;
	void Destroy() override;
	void Update(const float& aDeltaTime) override;
	void Render() override;
	void Put(Letter aLetter) override;
	void Prepare();
	
	CU::Vector3f GetPosition();

	void SetCamera(GameObject& aCamera);

	void SetLocalRotation(const CU::Vector3f& aRotation);
	void AddLocalRotation(const CU::Vector3f& aDeltaRotation);

	void Init();
	void RecieveEvent(const ActionEvent& anEvent) override;
	void LoadSettings(const std::string& aPath);
	void RecieveInternal(InternalMSG aEvent) override;

	// for misty purposes
	void TurnAround();

private:
	void Jump();
	void Scream();
	void AnimationFromState();
	void StopAnimation();
	void InitSounds();
	void UpdateSounds();

	void UpdateRRSound(PlayerSoundID anID);

	bool myIsJumpHeld;
	bool myIsCharging;
	bool myIsChargingScream;
	bool myIsCharged;
	bool myIsGrounded;
	bool myIsBonked;

	bool myIsGlidingUnlocked;
	bool myIsSuperJumpUnlocked;

	float myMovementSpeed;
	float myMaxSpeed;
	float myMaxGlideSpeed;
	float myJumpLift;
	float myJumpStartImpulse;
	float mySuperJumpImpulse;
	float myJumpLiftDuration;
	float myCurrentChargeTime;

	float myScreamReach;

	//timers
	float myCurrentJumpTime;
	float myChargeTimeRequired;

	float myScreamChargeTimer;
	float myMaxScreamChargeTime;
	float myScreamTimer;
	float myScreamDuration;
	float myMaxScreamDuration;

	ePlayerAnimationState myAnimationState;

	CU::Vector3f myMovementVector;
	bool myIsMoving;
	bool myWasWalkingLastFrame;
	bool myIsScreaming;
	bool myIsScreamReleased;

	GameObject* myCamera;
	CharacterControllerComponent* myRigidBody;

	Squish::PhysicsData myPhysicsData;

	std::unordered_map<PlayerSoundID, std::filesystem::path> mySoundIDToPath;
	PlayerSoundID myCurrentHurtSound = PlayerSoundID::Hurt;
	PlayerSoundID myCurrentDeathSound = PlayerSoundID::Death;

	CU::Vector3f myRotation;
};