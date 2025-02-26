#pragma once
#include <memory>
#include <CommonUtilities/Matrix.hpp>
#include "AnimationPlayer.h"

class Mesh;
struct Animation;
struct AnimationBuffer;

class AnimationPlayerSingle : public AnimationPlayer
{
public:
	AnimationPlayerSingle(const std::shared_ptr<Animation>& aAnimation, std::shared_ptr<Mesh>& aMesh,
		std::shared_ptr<AnimationBuffer>& anAnimationBuffer);
	~AnimationPlayerSingle() override = default;
	void Play() override;
	void Update(float aDeltaTime, bool aUpdateOnlyFrame = false) override;
	float GetDuration() override;
	void Stop() override;
	float GetTimer();
	Animation::Frame GetFrame(unsigned aFrame);
	
private:
	void UpdateAnimation(unsigned aJointIdx, const CU::Matrix4x4f& aParentJointTransform, std::shared_ptr<AnimationBuffer>& anAnimationBuffer);

	std::shared_ptr<Animation> myAnimation;
	float myTimer;
	float myFrameTime;
	size_t myCurrentFrame;
};