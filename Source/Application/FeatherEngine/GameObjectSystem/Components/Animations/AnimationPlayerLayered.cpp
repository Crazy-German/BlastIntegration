#include "FeatherEngine.pch.h"
#include "AnimationPlayerLayered.h"

#include "GraphicsEngine/Animation.h"
#include "GraphicsEngine/AnimationBuffer.h"
#include "GraphicsEngine/Mesh.h"

AnimationPlayerLayered::AnimationPlayerLayered(const std::unordered_map<unsigned, std::shared_ptr<Animation>>& aJointToAnimation,
	std::shared_ptr<Mesh>& aMesh, std::shared_ptr<AnimationBuffer>& anAnimationBuffer)
{
	myMesh = aMesh;
	myAnimationBuffer = anAnimationBuffer;

	for (const auto& animation : aJointToAnimation)
	{
		myJointToAnimation[animation.first] =
		{
			animation.second,
			0,
			1 / animation.second->FramesPerSecond,
			0
		};
	}

	myState = AnimationState::Paused;
}

void AnimationPlayerLayered::Update(float aDeltaTime, bool aUpdateOnlyFrame)
{
	if (myState == AnimationState::Playing)
	{
		for (auto& animation : myJointToAnimation)
		{
			animation.second.Timer += aDeltaTime;
			if (animation.second.Timer >= animation.second.FrameTime)
			{
				animation.second.CurrentFrame =
					animation.second.CurrentFrame < animation.second.Animation->Frames.size() - 1 ?
					animation.second.CurrentFrame + 1 : 0;
				animation.second.Timer = 0;
			}
		}
		if(!aUpdateOnlyFrame)
		{
			return;
		}
		CU::Matrix4x4f parentJointTransform = {};
		UpdateAnimation(0, parentJointTransform, myAnimationBuffer, myJointToAnimation[0]);
		return;
	}

	if (myState == AnimationState::Stopped)
	{
		for (auto& animation : myJointToAnimation)
		{
			animation.second.CurrentFrame = 0;
			animation.second.Timer = 0;
		}
		myState = AnimationState::Paused;
	}
}

void AnimationPlayerLayered::Stop()
{
	for (auto& animation : myJointToAnimation)
		{
			animation.second.CurrentFrame = 0;
			animation.second.Timer = 0;
		}
		myState = AnimationState::Stopped;
}

float AnimationPlayerLayered::GetDuration()
{
	return myJointToAnimation.at(0).Animation->Duration;
}

void AnimationPlayerLayered::UpdateAnimation(unsigned aJointIdx, const CU::Matrix4x4f& aParentJointTransform,
                                             std::shared_ptr<AnimationBuffer>& anAnimationBuffer, AnimationData& someAnimationData)
{
	auto joint = myMesh->GetSkeleton().Joints[aJointIdx];
	auto currentFrame = someAnimationData.Animation->Frames[someAnimationData.CurrentFrame];
	auto transform = currentFrame.Transforms[joint.Name];
	transform *= aParentJointTransform;
	anAnimationBuffer->JointTransforms[aJointIdx] = joint.BindPoseInverse * transform;
	auto quaternion = CommonUtilities::Quaternionf::Slerp(currentFrame.RealTransforms[joint.Name].GetRotationQuaternion(), currentFrame.RealTransforms[joint.Name].GetRotationQuaternion(), 0.5f);
	for (auto child : joint.Children)
	{
		if (myJointToAnimation.contains(child))
		{
			UpdateAnimation(child, transform, anAnimationBuffer, myJointToAnimation[child]);
		}
		else
		{
			UpdateAnimation(child, transform, anAnimationBuffer, someAnimationData);
		}
	}
}

