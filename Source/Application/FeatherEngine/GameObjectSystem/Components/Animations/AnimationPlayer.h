#pragma once
#include <memory>
#include <Graphics/GraphicsEngine/AnimationBuffer.h>

#include "GraphicsEngine/Animation.h"

class Mesh;
struct Animation;

class AnimationPlayer
{
public:
	enum class AnimationState
	{
		Playing,
		Paused,
		Stopped
	};

	AnimationPlayer() = default;
	virtual ~AnimationPlayer() = default;

	virtual void Play() { myState = AnimationState::Playing; }
	void Pause() { myState = AnimationState::Paused; }
	virtual void Stop() { myState = AnimationState::Stopped; }
	virtual float GetDuration(){return -1.0f;};
	virtual void Update(float aDeltaTime, bool aUpdateOnlyFrame = false) { aDeltaTime;aUpdateOnlyFrame; }
	AnimationState GetState() const;
	CU::Matrix4x4f* GetJointRefrence(int aJoint) const{return &myAnimationBuffer->JointTransforms[aJoint];};

protected:
	std::shared_ptr<Mesh> myMesh;
	std::shared_ptr<AnimationBuffer> myAnimationBuffer;
	AnimationState myState;
};

inline AnimationPlayer::AnimationState AnimationPlayer::GetState() const
{
	return myState;
}

