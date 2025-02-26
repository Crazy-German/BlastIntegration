#pragma once

#include "GameObjectSystem/Components/Component.h"


#include "GraphicsEngine/Animation.h"
#include "GraphicsEngine/AnimationBuffer.h"

#include <memory>
#include <string>
#include <unordered_map>


class AnimationPlayer;
struct Animation;
class Mesh;
class AnimationEventListener;


#define ANIMATOR 600
class Animator : public Component
{
public:
	struct SingleAnimationData
	{
		std::string Name;
		std::string AnimationAssetName;
	};

	struct LayeredAnimationData
	{
		std::string Name;
		std::string rootAnimation;
		std::vector<std::pair<std::string, std::string>> JointsToLayeredAnimations;
	};

	enum class AnimatorState
	{
		Playing,
		Paused,
		Stopped
	};

	Animator();
	~Animator() override = default;

	void Init();

	void Awake() override;
	void Update(const float& aDeltaTime) override;

	void PlayAnimation(const std::string& aName, const float& aBlendTime = 0);
	void PauseAnimation();
	void StopAnimation();
	AnimatorState GetAnimatorState() const;
	void AddAnimation(std::string aAnimationName, std::string aAnimationPath = "");
	void AddSingleAnimation(const SingleAnimationData& someAnimationData, const bool& aIsLooping = false);
	void AddLayeredAnimation(const LayeredAnimationData& someAnimationData);

	void RemoveAnimation(std::string aAnimation);
	float GetAnimationDuration(std::string aAnimation);

	void ForceResetAnimation();

	void UpdateAnimationBuffer() const;

	void StopCheck();

	void SubscribeToEvent(const std::string& anEventName, AnimationEventListener* aListener);

	void UnSubscribeFromEvent(const std::string& anEventName, AnimationEventListener* aListener);

	void NotifyListeners(const std::string& anEventName);

	std::shared_ptr<AnimationBuffer> GetAnimationBuffer();
	Animation::Frame& GetAnimationFrame(uint32_t aAnimationIndex) const;
	void UpdateAnimation(Animation::Frame& aFrame, Animation::Frame& aPrevFrame);
private:
	void UpdateInternalAnimation(unsigned aJointIndex, CommonUtilities::Matrix4x4f aParentTransform);
	void AddEvent(const std::string& anEventName);

private:
	std::shared_ptr<Mesh> myMesh;
	std::unordered_map<std::string, std::shared_ptr<AnimationPlayer>> myAnimations;

	std::string myCurrentAnimation;
	std::string myPrevAnimation;
	std::unordered_map<std::string, uint32_t> myAnimationNameToIndex;
	std::vector<std::shared_ptr<Animation>> myAnimation;
	std::shared_ptr<Animation> myActiveAnimation;
	Animation::Frame aPose;

	float myBlendingDelay;
	float myBlendingTimer;
	AnimatorState myCurrentAnimatorState;
	std::shared_ptr<AnimationBuffer> myAnimationBuffer;

	std::unordered_map<std::string, std::vector<AnimationEventListener*>> myEventListeners;
};

