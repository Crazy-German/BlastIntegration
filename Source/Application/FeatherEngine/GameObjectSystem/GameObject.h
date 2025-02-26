#pragma once
#include "Components/Component.h"
#include <vector>
#include <memory>
#include <string>
#include "Components/Transform.h"
#include "ComponentSystem.h"

class ComponentBank;

enum class eEnemyType // Temporär o Olaglig
{
	None,
	Anger,
	Anxiety,
	Sadness,
	Boss
};

class GameObject
{
public:
	GameObject();
	~GameObject() = default;
	void Awake();
	void Update(const float& aDeltaTime);
	void Render();
	void DebugRender();
	Transform* GetTransform();

	Component* AddComponent(Component* aComponent);
	template <class T>
	T* GetComponent();
	template <class T>
	std::vector<T*> GetComponents();
	template<class T, int MaxSize>
	void RemoveComponent();
	template<class T, int MaxSize>
	void RemoveComponent(Component* aComponent);
	template<class T, int MaxSize>
	void RemoveComponent(const int& anIndex);
	template<class T, int MaxSize>
	void RemoveComponents();
	Component*& FindComponentLink(void* aComponentPointer);

	void Destroy();

	bool GetEnabled() const;
	void SetEnabled(const bool& aIsEnabled);
	std::string& GetName();
	void SetName(const std::string& aName);
	void SendInternal(InternalMSG aMsg) const;
	void SetIsVisible(const bool aIsVisible);
	bool GetIsVisible() const;
	void SetShouldDebugRender(const bool aShouldDebugRender);
	bool& ShouldDebugRender();
	void UpdatedebugRenderState();

	int GetEnemyGroupId() const;
	void SetEnemyGroupId(const int& aId);


	eEnemyType GetEnemyType();
	void SetEnemyType(eEnemyType aEnemyType);
	eEnemyType myEnemyType; // VILL RÖJA
	int myEnemyGroup; // VILL RÖJA
private:
	friend ComponentBank; // Kanske inte behövs

	bool myIsEnabled;
	std::string myName = "Unnamed";
	std::shared_ptr<GameObject> myParent;
	std::vector<std::shared_ptr<GameObject>> myChildren;
	std::vector<Component*> myComponents;
	bool myIsVisible = true;
	bool myShouldDebugRender = false;

};

template<class T>
inline T* GameObject::GetComponent()
{
	if (this == nullptr)
	{
		return nullptr;
	}
	for (int componentIndex = 0; componentIndex < myComponents.size(); componentIndex++)
	{
		if(myComponents[componentIndex]->GetTypeId() == ComponentSystem::GetTypeID<T>())
		{
			return static_cast<T*>(myComponents[componentIndex]);
		}
	}
	return nullptr;
}

template<class T>
inline std::vector<T*> GameObject::GetComponents()
{
	std::vector<T*> outComponents;
	for (int componentIndex = 0; componentIndex < myComponents.size(); componentIndex++)
	{
		if (myComponents[componentIndex]->GetTypeId() == ComponentSystem::GetTypeID<T>())
		{
			outComponents.push_back(static_cast<T*>(myComponents[componentIndex]));
		}
	}
	return outComponents;
}

template<class T, int MaxSize>
inline void GameObject::RemoveComponent()
{
	for (int componentIndex = 0; componentIndex < myComponents.size(); componentIndex++)
	{
		if (myComponents[componentIndex]->GetTypeId() == T::GetComponentId())
		{
			myComponents[componentIndex] = myComponents[myComponents.size() - 1];
			myComponents.pop_back();
			// ComponentSystem.RemoveComponent<T, MaxSize>()
			return;
		}
	}
}

template<class T, int MaxSize>
inline void GameObject::RemoveComponent(Component* aComponent)
{
	for (int componentIndex = 0; componentIndex < myComponents.size(); componentIndex++)
	{
		if (myComponents[componentIndex]->GetTypeId() == T::GetComponentId() && myComponents[componentIndex]->GetId() == aComponent->GetId())
		{
			myComponents[componentIndex] = myComponents[myComponents.size() - 1];
			myComponents.pop_back();
			// ComponentSystem.RemoveComponent<T, MaxSize>()
			return;
		}
	}
}

template<class T, int MaxSize>
inline void GameObject::RemoveComponent(const int& anIndex)
{
	// Vet inte varför man skulle vilja använda denna, kan göra så den utnyttjar IDn istället men då kan man lika gärna använda den ovan. Det är enklare om IDn stannar under huvven
	myComponents[anIndex] = myComponents[myComponents.size() - 1];
	myComponents.pop_back();
	// ComponentSystem.RemoveComponent<T, MaxSize>()
}

template<class T, int MaxSize>
inline void GameObject::RemoveComponents()
{
	for (int componentIndex = 0; componentIndex < myComponents.size(); componentIndex++)
	{
		if (myComponents[componentIndex]->GetTypeId() == T::GetComponentId())
		{
			myComponents[componentIndex] = myComponents[myComponents.size() - 1];
			myComponents.pop_back();
			// ComponentSystem.RemoveComponent<T, MaxSize>()
			componentIndex -= 1;
		}
	}
}
