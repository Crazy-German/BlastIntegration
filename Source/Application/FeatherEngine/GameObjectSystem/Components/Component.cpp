#include "FeatherEngine.pch.h"
#include "Component.h"
#include "GameObjectSystem/ComponentSystem.h"
#include "Logger/Logger.h"

Component::Component()
{
	myIsEnabled = false;
	myIsMarkedForDestroy = false;
	myId = -1;
}

void Component::Awake()
{
}

void Component::Update(const float& aDeltaTime)
{
	aDeltaTime;
}

void Component::Render()
{
}

void Component::DebugRender()
{
}

void Component::Destroy()
{
	myIsMarkedForDestroy = true;
	myIsEnabled = false;
}

bool Component::GetMarkedDestroy() const
{
	return myIsMarkedForDestroy;
}

void Component::Put(Letter /*aLetter*/)
{
}

GameObject* Component::GetGameObject() const
{
	return myGameObject;
}

void Component::SetGameObject(std::shared_ptr<GameObject> aGameObject)
{
	myGameObject = aGameObject.get();
}

bool Component::GetEnabled() const
{
	return myIsEnabled;
}

void Component::SetEnabled(const bool& aIsEnabled)
{
	myIsEnabled = aIsEnabled;
}

void Component::RecieveInternal(InternalMSG aEvent)
{
	switch (aEvent.myEvent)
	{
	case InternalEvent::eHit:
		break;
	case InternalEvent::eDamage:
		break;
	default:
		break;
	}
}

void Component::SetShouldDebugRender(const bool aShouldDebugRender)
{
	myShouldDebugRender = aShouldDebugRender;
	myGameObject->UpdatedebugRenderState();
}

bool& Component::ShouldDebugRender()
{
	return myShouldDebugRender;
}

int Component::GetId() const
{
	return myId;
}

void Component::SetId(const int& aId)
{
	myId = aId;
}

int Component::GetTypeId()
{
	return myTypeID;
}

void Component::SetTypeId(const int& aComponentId)
{
	myTypeID = aComponentId;
}

Component*& Component::GetLink(void* ComponentPointer)
{
	return myGameObject->FindComponentLink(ComponentPointer);
}
