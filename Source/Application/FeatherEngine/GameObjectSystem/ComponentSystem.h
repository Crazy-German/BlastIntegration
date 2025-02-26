#pragma once
#include <array>
#include <vector>
#include <string>
#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <memory>
#include "ComponentStore.h"
#include <iostream>



class ComponentSystem
{
public:
	static std::shared_ptr<ComponentSystem> Get();
	ComponentSystem();
	template <class T, int MaxAmount>
	void RegisterComponent();
	//DO NOT FORGET TO REGISTER COMPONENTS
	template <class T, int MaxAmount>
	T* AddComponent();
	template <class T, int MaxAmount>
	void RemoveComponent(std::shared_ptr<T> aComponent);
	template <class T, int MaxAmount>
	void RemoveComponent(const int& anId);

	void Awake();
	void Update(const float& aDeltaTime);
	void Render();
	void CleanUp();
	template <class T>
	static int GetTypeID();
	template<class T, int MaxAmount>
	ComponentStore<T, MaxAmount>* GetComponentStore();
private:
	int myComponentTypesRegistred = 0;
	int myComponentsCreated = 0;
	std::vector<ComponentBank*> myComponentStores;
	
	static std::unordered_map<const char*, int> myIDMap;
};

template<class T, int MaxAmount>
inline void ComponentSystem::RegisterComponent()
{
	myIDMap[typeid(T).name()] = myComponentTypesRegistred;
	std::cout << "Regesting new component (" << typeid(T).name() << ") with Id: " << myComponentTypesRegistred << std::endl;
	myComponentTypesRegistred += 1;
	ComponentStore<T, MaxAmount>* componentStorePointer = GetComponentStore<T, MaxAmount>();
	myComponentStores.push_back(componentStorePointer);
}

template<class T, int MaxAmount>
inline T* ComponentSystem::AddComponent()
{
	T component = T();
	component.SetTypeId(myIDMap[typeid(T).name()]);
	myComponentsCreated += 1;
	return static_cast<T*>(GetComponentStore<T, MaxAmount>()->AddComponent(component));
}

template<class T, int MaxAmount>
inline void ComponentSystem::RemoveComponent(std::shared_ptr<T> aComponent)
{
	GetComponentStore<T, MaxAmount>()->RemoveComponent(aComponent->GetId());
}

template<class T, int MaxAmount>
inline void ComponentSystem::RemoveComponent(const int& anId)
{
	GetComponentStore<T, MaxAmount>()->RemoveComponent(anId);
}

template<class T>
inline int ComponentSystem::GetTypeID()
{
	//const int test = myIDMap[typeid(T).name()];
	return myIDMap[typeid(T).name()];
}

template<class T, int MaxAmount>
inline ComponentStore<T, MaxAmount>* ComponentSystem::GetComponentStore()
{
	static ComponentStore<T, MaxAmount> componentStore;
	return &componentStore;
}