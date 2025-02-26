#include "FeatherEngine.pch.h"
#include "GameObject.h"
#include "memory"

GameObject::GameObject()
{
	myIsEnabled = true;
	myEnemyType = eEnemyType::None;
	myEnemyGroup = -1;
}

void GameObject::Awake()
{
	for (int componentIndex = 0; componentIndex < myComponents.size(); componentIndex++)
	{
		myComponents[componentIndex]->Awake();
	}
}

void GameObject::Update(const float& aDeltaTime)
{
	if (this == nullptr || !myIsEnabled)
	{
		return;
	}
	for (int componentIndex = 0; componentIndex < myComponents.size(); componentIndex++)
	{
		if (myComponents[componentIndex]->GetEnabled())
		{
			myComponents[componentIndex]->Update(aDeltaTime);
		}
	}
}

void GameObject::Render()
{
	if (!myIsVisible || this == nullptr || !myIsEnabled)
	{
		return;
	}
	for (int componentIndex = 0; componentIndex < myComponents.size(); componentIndex++)
	{
		myComponents[componentIndex]->Render();
	}
}

void GameObject::DebugRender()
{
#ifdef _RETAIL
	return;
#else

	if (!myShouldDebugRender)
	{
		return;
	}
	for (auto& component : myComponents)
	{
		if(component->ShouldDebugRender())
		{
			component->DebugRender();
		}
	}
#endif
}

Transform* GameObject::GetTransform()
{
	return GetComponent<Transform>();
}

Component* GameObject::AddComponent(Component* aComponent)
{
	if (aComponent->GetGameObject() == nullptr)
	{
		myComponents.push_back(aComponent);
		return aComponent;
	}
	return nullptr;
}

Component*& GameObject::FindComponentLink(void* aComponentPointer)
{
	for (auto& component : myComponents)
	{
		if (component == aComponentPointer)
		{
			return component;
		}
	}
	throw;
	//Component* DANGERPOINTER = nullptr;
	//return DANGERPOINTER;
}

void GameObject::Destroy()
{
	for (int componentIndex = 0; componentIndex < myComponents.size(); componentIndex++)
	{
		myComponents[componentIndex]->Destroy();
	}
}

bool GameObject::GetEnabled() const
{
	return myIsEnabled;
}

void GameObject::SetEnabled(const bool& aIsEnabled)
{
	myIsEnabled = aIsEnabled;
	for (int componentId = 0; componentId < myComponents.size(); componentId++)
	{
		myComponents[componentId]->SetEnabled(aIsEnabled);
	}
}

std::string& GameObject::GetName()
{
	return myName;
}

void GameObject::SetName(const std::string& aName)
{
	myName = aName;
}

void GameObject::SendInternal(InternalMSG aMsg) const
{
	for (int componentId = 0; componentId < myComponents.size(); componentId++)
	{
		myComponents[componentId]->RecieveInternal(aMsg);
	}
}

void GameObject::SetIsVisible(const bool aIsVisible)
{
	myIsVisible = aIsVisible;
}

bool GameObject::GetIsVisible() const
{
	return myIsVisible;
}

void GameObject::SetShouldDebugRender(const bool aShouldDebugRender)
{
	myShouldDebugRender = aShouldDebugRender;
	for (int componentId = 0; componentId < myComponents.size(); componentId++)
	{
		myComponents[componentId]->SetShouldDebugRender(aShouldDebugRender);
	}
}

bool& GameObject::ShouldDebugRender()
{
	return myShouldDebugRender;
}

void GameObject::UpdatedebugRenderState()
{
	myShouldDebugRender = false;
	for (int componentId = 0; componentId < myComponents.size(); componentId++)
	{
		myShouldDebugRender = myComponents[componentId]->ShouldDebugRender();
		if(myShouldDebugRender)
		{
			break;
		}
	}
}


void GameObject::SetEnemyType(eEnemyType aEnemyType)
{
	myEnemyType = aEnemyType;
}

eEnemyType GameObject::GetEnemyType()
{
	return myEnemyType;
}

void GameObject::SetEnemyGroupId(const int& aId)
{
	myEnemyGroup = aId;
}

int GameObject::GetEnemyGroupId() const
{
	return myEnemyGroup;
}
