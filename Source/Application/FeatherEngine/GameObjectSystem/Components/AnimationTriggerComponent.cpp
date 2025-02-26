#include "FeatherEngine.pch.h"
#include "AnimationTriggerComponent.h"

#include "MainSingleton.h"
#include "Animations/Animator.h"
#include "GameObjectSystem/PostMaster/GameObjectPostMaster.h"
#include "AudioSourceComponent.h"

AnimationTriggerComponent::AnimationTriggerComponent()
{
	myHasTriggered = false;
	myIsPlayingFirstBridgeAnimation = false;
	myNeedsToLoop = false;
}

void AnimationTriggerComponent::Update(const float& aDeltaTime)
{
	aDeltaTime;
	if (myIsPlayingFirstBridgeAnimation == true)
	{
		if (myNeedsToLoop)
		{
			myNeedsToLoop = false;
		}
		else
		{
			myGameObject->GetComponent<Animator>()->StopCheck();
			if (myGameObject->GetComponent<Animator>()->GetAnimatorState() == Animator::AnimatorState::Stopped)
			{
				myGameObject->GetComponent<Animator>()->PlayAnimation("Magic Bridge Idle");
				myIsPlayingFirstBridgeAnimation = false;
				myIsEnabled = false;
			}
		}
	}
}

void AnimationTriggerComponent::Awake()
{
	GameObjectPostMaster::Get()->Register(eLetterRegister::ScriptedEvent, this);
}

void AnimationTriggerComponent::RecieveInternal(InternalMSG aEvent)
{
	if ((aEvent.myEvent == InternalEvent::eDamage || aEvent.myEvent == InternalEvent::eHit) && myHasTriggered == false)
	{
		myHasTriggered = true;
		myGameObject->GetComponent<Animator>()->PlayAnimation("Breaking vase");
		GetGameObject()->GetComponent<AudioSourceComponent>()->MakeSound("VaseBreak_SFX3D");
	}

}

void AnimationTriggerComponent::Put(Letter anEvent)
{
	anEvent;
	if (myHasTriggered == false)
	{
		myGameObject->GetComponent<Animator>()->PlayAnimation("Magic Bridge Separate");
		GetGameObject()->GetComponent<AudioSourceComponent>()->MakeSound("MagicBridge_SFX");
		myHasTriggered = true;
		myNeedsToLoop = true;
		myIsPlayingFirstBridgeAnimation = true;
	}
}
