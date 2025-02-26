#include "FeatherEngine.pch.h"
#include "Animator.h"

#include <stdexcept>

#include "AnimationPlayerLayered.h"
#include "AnimationPlayerSingle.h"
#include "MainSingleton.h"
#include "GraphicsEngine/RenderData.h"

#include "AssetManagement/AssetManager.h"
#include "AssetManagement/AssetTypes/AnimationAsset.h"

#include "GraphicsEngine/GfxCmd_SetAnimationBuffer.h"
#include "GraphicsEngine/Mesh.h"

#include "GameObjectSystem/Components/Animations/AnimationEventListener.h"
#include "GameObjectSystem/Components/MeshComponent.h"
#include "GameObjectSystem\Components\Animations\AnimationPlayerSingle.h"
#include <execution>

Animator::Animator()
{
	myCurrentAnimation = "";
	myCurrentAnimatorState = AnimatorState::Stopped;
}

void Animator::Init()
{
	myMesh = GetGameObject()->GetComponent<MeshComponent>()->GetMesh();
	myAnimationBuffer = std::make_shared<AnimationBuffer>();
}


void Animator::Awake()
{/*
	myMesh = GetGameObject()->GetComponent<MeshComponent>()->GetMesh();
	myAnimationBuffer = std::make_shared<AnimationBuffer>();*/
}

void Animator::Update(const float& aDeltaTime)
{
	if(myAnimations[myCurrentAnimation] == nullptr)
	{
		return;
	}
	if (myCurrentAnimatorState == AnimatorState::Playing)
	{
		myBlendingTimer+=aDeltaTime;
		//myAnimations[myCurrentAnimation]->Update(aDeltaTime, myGameObject->GetIsVisible());

		if(myGameObject->GetIsVisible())
		{
			if(myAnimationNameToIndex[myCurrentAnimation] != myAnimationNameToIndex[myPrevAnimation])
			{
			}
			myAnimation[myAnimationNameToIndex[myCurrentAnimation]]->myTime+=aDeltaTime;
			if(myAnimationNameToIndex[myCurrentAnimation] != myAnimationNameToIndex[myPrevAnimation])
			{
				myAnimation[myAnimationNameToIndex[myPrevAnimation]]->myTime+=aDeltaTime;
			}
			Animation::Frame& prevAnimationFrame = GetAnimationFrame(myAnimationNameToIndex[myPrevAnimation]);
			Animation::Frame& currentAnimationFrame = GetAnimationFrame(myAnimationNameToIndex[myCurrentAnimation]);
			UpdateAnimation(currentAnimationFrame, prevAnimationFrame);
			if (currentAnimationFrame.TriggeredEvents.size() > 0)
			{
				for (auto& eventIndex : currentAnimationFrame.TriggeredEvents)
				{
					NotifyListeners(myAnimation[myAnimationNameToIndex[myCurrentAnimation]]->EventNames[eventIndex]);
				}
			}
		}


	}
	if(myAnimations[myCurrentAnimation]->GetState() != AnimationPlayer::AnimationState::Playing)
	{
		myCurrentAnimatorState = AnimatorState::Stopped;
	}
}

void Animator::PlayAnimation(const std::string& aName, const float& aBlendTime)
{
	if (myAnimations.contains(aName) && aName != myCurrentAnimation)
	{
		std::cout << aName << "\n";
		if(myCurrentAnimation.empty())
		{
			myPrevAnimation = aName;
		}
		else
		{
			myPrevAnimation = myCurrentAnimation;
		}
		myCurrentAnimation = aName;
		myAnimations[myCurrentAnimation]->Play();
		myAnimation[myAnimationNameToIndex[aName]]->myTime = 0;
		myCurrentAnimatorState = AnimatorState::Playing;
		myBlendingDelay = aBlendTime;
		myBlendingTimer = 0;
	}
}

void Animator::PauseAnimation()
{
	if (myAnimations.contains(myCurrentAnimation))
	{
		myAnimations[myCurrentAnimation]->Pause();
		myCurrentAnimatorState = AnimatorState::Paused;
	}
}

void Animator::StopAnimation()
{
	if (myAnimations.contains(myCurrentAnimation))
	{
		myAnimations[myCurrentAnimation]->Stop();
		myCurrentAnimatorState = AnimatorState::Stopped;
	}
}

Animator::AnimatorState Animator::GetAnimatorState() const
{
	return myCurrentAnimatorState;
}

void Animator::AddAnimation(std::string aAnimationName, std::string aAnimationPath)
{
	myAnimationNameToIndex.emplace(aAnimationName, (uint32_t)myAnimation.size());
	myAnimation.push_back(AssetManager::Get().GetAsset<AnimationAsset>(aAnimationPath.empty()?aAnimationName:aAnimationPath)->myAnimation);
	for (auto& eventName : myAnimation.back()->EventNames)
	{
		AddEvent(eventName);
	}
}

void Animator::AddSingleAnimation(const SingleAnimationData& someAnimationData, const bool& aIsLooping)
{
	if (AssetManager::Get().GetAsset<AnimationAsset>(someAnimationData.AnimationAssetName).get() == nullptr)
	{
		return;
	}
	std::shared_ptr<Animation> animation = AssetManager::Get().GetAsset<AnimationAsset>(someAnimationData.AnimationAssetName)->myAnimation;
	animation->myIsLooping = aIsLooping;
	myAnimationNameToIndex.emplace(someAnimationData.Name, (uint32_t)myAnimation.size());
	myAnimation.emplace_back(animation);
	myAnimations[someAnimationData.Name] = std::make_shared<AnimationPlayerSingle>(animation, myMesh, myAnimationBuffer);

	for (auto& eventName : myAnimation.back()->EventNames)
	{
		AddEvent(eventName);
	}
}

void Animator::AddLayeredAnimation(const LayeredAnimationData& someAnimationData)
{
	std::unordered_map<unsigned, std::shared_ptr<Animation>> jointToAnimation;

	jointToAnimation[0] = AssetManager::Get().GetAsset<AnimationAsset>(someAnimationData.rootAnimation)->myAnimation;
	for (const auto& pair : someAnimationData.JointsToLayeredAnimations)
	{
		jointToAnimation[myMesh->GetSkeleton().JointNameToIndex.at(pair.first)] =
			AssetManager::Get().GetAsset<AnimationAsset>(pair.second)->myAnimation;
	}

	myAnimations[someAnimationData.Name] = std::make_shared<AnimationPlayerLayered>(jointToAnimation, myMesh, myAnimationBuffer);

	for (auto& eventName : myAnimation.back()->EventNames)
	{
		AddEvent(eventName);
	}
}

void Animator::RemoveAnimation(std::string aAnimation)
{
	myAnimations.erase(aAnimation);
}

float Animator::GetAnimationDuration(std::string aAnimation)
{
	if(!myAnimations.contains(aAnimation))
	{
		return -1.0f;
	}
	return myAnimations.at(aAnimation)->GetDuration();
}

void Animator::ForceResetAnimation()
{
	//myAnimations[myCurrentAnimation]->
	reinterpret_cast<AnimationPlayerSingle*>(myAnimations[myCurrentAnimation].get())->Stop();
}

void Animator::UpdateAnimationBuffer() const
{
	//AnimationBufferData data;
	//data.type = GraphicsCommandType::SetAnimationBuffer;
	//data.animationBuffer = *myAnimationBuffer;
	MainSingleton::Get().GetRenderer().Enqueue<GfxCmd_SetAnimationBuffer>(myAnimationBuffer);
}

void Animator::StopCheck()
{
	if (reinterpret_cast<AnimationPlayerSingle*>(myAnimations[myCurrentAnimation].get())->GetTimer() >= reinterpret_cast<AnimationPlayerSingle*>(myAnimations[myCurrentAnimation].get())->GetDuration())
	{
		StopAnimation();
	}
	
}

void Animator::SubscribeToEvent(const std::string& anEventName, AnimationEventListener* aListener)
{
	if (!myEventListeners.contains(anEventName))
	{
		return;
	}
	myEventListeners.at(anEventName).push_back(aListener);
}

void Animator::UnSubscribeFromEvent(const std::string& anEventName, AnimationEventListener* aListener)
{
	if (!myEventListeners.contains(anEventName))
	{
		return;
	}
	int deleteIndex = -1;
	for (int index = 0; index < myEventListeners.at(anEventName).size(); index++)
	{
		if (myEventListeners.at(anEventName)[index] == aListener)
		{
			deleteIndex = index;
			myEventListeners.at(anEventName)[index] = nullptr;
		}
	}

	if (deleteIndex >= 0)
	{
		myEventListeners.at(anEventName).erase(myEventListeners.at(anEventName).begin() + deleteIndex);
	}
}

void Animator::NotifyListeners(const std::string& anEventName)
{
	if (!myEventListeners.contains(anEventName))
	{
		return;
	}
	for (auto& listener : myEventListeners.at(anEventName))
	{
		listener->OnAnimationEventTrigger(anEventName);
	}
}

std::shared_ptr<AnimationBuffer> Animator::GetAnimationBuffer()
{
	return myAnimationBuffer;
}

Animation::Frame& Animator::GetAnimationFrame(uint32_t aAnimationIndex) const
{
	float& time = myAnimation[aAnimationIndex]->myTime;
	if (time >= myAnimation[aAnimationIndex]->Duration)
	{
		if (myAnimation[aAnimationIndex]->myIsLooping)
		{
			while (time >= myAnimation[aAnimationIndex]->Duration)
				time -= myAnimation[aAnimationIndex]->Duration;
		}
		else
		{
			time = myAnimation[aAnimationIndex]->Duration;
		}
	}

	float result = time*myAnimation[aAnimationIndex]->FramesPerSecond;
	size_t frameIndex = (size_t)std::floor(result);
	return myAnimation[aAnimationIndex]->Frames[frameIndex];
}

void Animator::UpdateAnimation(Animation::Frame& aFrame, Animation::Frame& aPrevFrame)
{
	auto& skeleton = myMesh->GetSkeleton();
	
	for(size_t i = 2; i<skeleton.Joints.size(); i++)
	{
		const auto& currentFrameTransform = aFrame.RealTransforms[skeleton.Joints[i].Name];
		const auto& prevFrameTransform = aPrevFrame.RealTransforms[skeleton.Joints[i].Name];
		const float t = 1 - myBlendingDelay/myBlendingTimer;
		CommonUtilities::Vector3f pos = CommonUtilities::Vector3f::Lerp(prevFrameTransform.GetPosition(), currentFrameTransform.GetPosition(), t);
		CommonUtilities::Quaternionf quat = CommonUtilities::Quaternionf::Slerp(prevFrameTransform.GetRotationQuaternion(), currentFrameTransform.GetRotationQuaternion(), t);
		CommonUtilities::Vector3f scale = CommonUtilities::Vector3f::Lerp(prevFrameTransform.GetScale(), currentFrameTransform.GetScale(), t);

		aPose.RealTransforms[skeleton.Joints[i].Name].SetPosition(pos);
		aPose.RealTransforms[skeleton.Joints[i].Name].SetScale(scale);
		aPose.RealTransforms[skeleton.Joints[i].Name].SetRotation(quat);
		aPose.RealTransforms[skeleton.Joints[i].Name].RecalculateCache();
	}
	
	UpdateInternalAnimation(2, skeleton.Joints[1].BindPoseInverse);
}

void Animator::UpdateInternalAnimation(unsigned aJointIndex, CommonUtilities::Matrix4x4f aParentTransform)
{
	auto joint = myMesh->GetSkeleton().Joints[aJointIndex];
	auto currentFrame = aPose;
	auto transform = currentFrame.RealTransforms[joint.Name].GetMatrixScale();
	
	transform *= aParentTransform;
	myAnimationBuffer->JointTransforms[aJointIndex] = joint.BindPoseInverse * transform;
	for (auto child : joint.Children)
	{
		UpdateInternalAnimation(child, transform);
	}

}


void Animator::AddEvent(const std::string& anEventName)
{
	if (myEventListeners.contains(anEventName))
	{
		return;
	}
	std::vector<AnimationEventListener*> listeners;
	myEventListeners.emplace(std::pair(std::string(anEventName), listeners));
}

