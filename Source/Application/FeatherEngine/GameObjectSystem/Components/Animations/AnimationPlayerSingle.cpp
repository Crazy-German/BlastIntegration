#include "FeatherEngine.pch.h"
#include "AnimationPlayerSingle.h"

#include "GraphicsEngine/Animation.h"
#include "GraphicsEngine/AnimationBuffer.h"
#include "GraphicsEngine/Mesh.h"

AnimationPlayerSingle::AnimationPlayerSingle(const std::shared_ptr<Animation>& aAnimation, std::shared_ptr<Mesh>& aMesh, 
	std::shared_ptr<AnimationBuffer>& anAnimationBuffer)
{
	myMesh= aMesh;
	myAnimationBuffer = anAnimationBuffer;

	myAnimation = aAnimation;
	myTimer = 0;
	myFrameTime = 1 / myAnimation->FramesPerSecond;
	myCurrentFrame = 0;
	myState = AnimationState::Paused;
}

void AnimationPlayerSingle::Play()
{
	myState = AnimationState::Playing;
	if(!myAnimation->myIsLooping)
	{
		myTimer = 0;
		myCurrentFrame = 0;
	}
}

void AnimationPlayerSingle::Update(float aDeltaTime, bool aUpdateOnlyFrame)
{
	if (myState == AnimationState::Playing)
	{
		myTimer += aDeltaTime;
		if (myTimer >= myFrameTime)
		{
			if (myCurrentFrame >= myAnimation->Frames.size())
			{
				if(myAnimation->myIsLooping)
				{
					myCurrentFrame = 0;
				}
				else
				{
					myState = AnimationState::Stopped;
					return;
				}
			}
			CU::Matrix4x4f parentJointTransform = {};
			if(!aUpdateOnlyFrame)
			{
				myTimer = 0;
				myCurrentFrame++;
				return;
			}
			UpdateAnimation(0, parentJointTransform, myAnimationBuffer);
			myTimer = 0;
			myCurrentFrame++;
		}
		return;
	}

	if (myState == AnimationState::Stopped)
	{
		myCurrentFrame = 0;
		myTimer = 0;
		//myState = AnimationState::Paused;
	}
}

float AnimationPlayerSingle::GetDuration()
{
	return myAnimation->Duration;
}

void AnimationPlayerSingle::Stop()
{
	myState = AnimationState::Stopped;
	myCurrentFrame = 0;
	myTimer = 0;
}

float AnimationPlayerSingle::GetTimer()
{
	return myFrameTime * myCurrentFrame;
}

Animation::Frame AnimationPlayerSingle::GetFrame(unsigned aFrame)
{
	if(aFrame<myAnimation->Frames.size()-1)
	{
		return myAnimation->Frames[aFrame];
	}
	return Animation::Frame();
}

void AnimationPlayerSingle::UpdateAnimation(unsigned aJointIdx, const CU::Matrix4x4f& aParentJointTransform, std::shared_ptr<AnimationBuffer>& anAnimationBuffer)
{
	auto joint = myMesh->GetSkeleton().Joints[aJointIdx];
	auto currentFrame = myAnimation->Frames[myCurrentFrame];
	auto transform = currentFrame.Transforms[joint.Name];
	auto transform2 = currentFrame.RealTransforms[joint.Name].GetMatrixScale();
	//auto quaternion = CommonUtilities::Quaternionf::Slerp(currentFrame.RealTransforms[joint.Name].GetRotationQuaternion(), currentFrame.RealTransforms[joint.Name].GetRotationQuaternion(), 0.5f);
	transform *= aParentJointTransform;
	anAnimationBuffer->JointTransforms[aJointIdx] = joint.BindPoseInverse * transform;
	for (auto child : joint.Children)
	{
		UpdateAnimation(child, transform, anAnimationBuffer);
	}
}
