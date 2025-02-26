#pragma once
#include "AnimationPlayer.h"

#include <memory>
#include <unordered_map>
#include <CommonUtilities/Matrix.hpp>

class Mesh;
struct Animation;
struct AnimationBuffer;

class AnimationPlayerLayered : public AnimationPlayer
{
public:
	struct AnimationData
	{
		std::shared_ptr<Animation> Animation;
		float Timer;
		float FrameTime;
		unsigned CurrentFrame;
	};

	AnimationPlayerLayered(const std::unordered_map<unsigned, std::shared_ptr<Animation>>& aJointToAnimation, 
		std::shared_ptr<Mesh>& aMesh, std::shared_ptr<AnimationBuffer>& anAnimationBuffer);
	~AnimationPlayerLayered() override = default;

	void Update(float aDeltaTime, bool aUpdateOnlyFrame = false) override;
	void Stop() override;
	float GetDuration() override;
private:
	void UpdateAnimation(unsigned aJointIdx, const CU::Matrix4x4f& aParentJointTransform, std::shared_ptr<AnimationBuffer>& anAnimationBuffer, AnimationData& someAnimationData);
	std::unordered_map<unsigned, AnimationData> myJointToAnimation;
};

