#pragma once
#include "../FeatherEngine/GameObjectSystem/Components/Component.h"
#include "../FeatherEngine/GameObjectSystem/Components/Animations/AnimationEventListener.h"

#include "CommonUtilities/Matrix4x4.hpp"

#define AMOUNT_OF_FOOTSTEPS 4
#define FOOTSTEP_VISIBILITY_TIME 5.0f

#define FOOTSTEPS_COMPONENT 5

#define LEFT_FOOT_EVENT_NAME "stomp_event"

class GameObject;
class Animator;

enum class Foot
{
	Left,
	Right
};

struct FootTransform
{
	CU::Matrix4x4f* JoinTransform;
	CU::Matrix4x4f BindPose;
};

class FootStepsComponent : public Component, public AnimationEventListener
{
public:
	FootStepsComponent() = default;
	~FootStepsComponent() = default;

	void Awake() override { ; }
	void Init(Animator* anAnimator);
	void SetLeftBone(CU::Matrix4x4f* aJointTransform, const CU::Matrix4x4f& aBindPose);
	void SetRightBone(CU::Matrix4x4f* aJointTransform, const CU::Matrix4x4f& aBindPose);
	void Destroy() override;
	void Update(const float& aDeltaTime) override;
	void Render() override;
	void PlaceStep(const Foot aFoot);

	void OnAnimationEventTrigger(std::string_view anEventName);

private:
	std::array<std::shared_ptr<GameObject>, AMOUNT_OF_FOOTSTEPS> myLeftSteps;
	std::array<std::shared_ptr<GameObject>, AMOUNT_OF_FOOTSTEPS> myRightSteps;
	FootTransform myLeftFoot;
	FootTransform myRightFoot;
	Animator* myAnimator;
	int myCurrentLeftIndex = 0;
	int myCurrentRightIndex = 0;
};

