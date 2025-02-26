#pragma once
#include <array>
#include <memory>
#include <vector>
#include "Components\Component.h"
#include "iostream"

class ComponentBank
{
public:
	virtual void Awake() = 0;
	virtual void Render() = 0;
	virtual void Update(const float& aDeltaTime) = 0;
	virtual void CleanUp() = 0;
private:
};

template <class T, int MaxAmount>
class ComponentStore : public ComponentBank
{
public:
	void Awake() override;
	void Update(const float& aDeltaTime) override;
	void Render() override;
	void CleanUp() override;
	T* AddComponent(T aComponent);
	void RemoveComponent(const int& aIndex);
	std::array<T, MaxAmount>& GetArray();
	int GetLatestIndex();
private:
	int myLatestIndex;
	std::vector<int> myFreeIndexes;
	std::array<T, MaxAmount> myComponents;
	std::vector<T> myOverflowingComponents;
};

template<class T, int MaxAmount>
inline void ComponentStore<T, MaxAmount>::Awake()
{
	int totalSize = myLatestIndex - static_cast<int>(myOverflowingComponents.size());
	for (int componentIndex = 0; componentIndex < totalSize; componentIndex++)
	{
		if (myComponents[componentIndex].GetEnabled())
		{
			myComponents[componentIndex].Awake();
		}
	}
	for (int componentIndex = 0; componentIndex < myOverflowingComponents.size(); componentIndex++)
	{
		if (myOverflowingComponents[componentIndex].GetEnabled())
		{
			myOverflowingComponents[componentIndex].Awake();
		}
	}
}

template<class T, int MaxAmount>
inline void ComponentStore<T, MaxAmount>::Update(const float& aDeltaTime)
{
	int totalSize = myLatestIndex - static_cast<int>(myOverflowingComponents.size());
	for (int componentIndex = 0; componentIndex < totalSize; componentIndex++)
	{
		if (myComponents[componentIndex].GetEnabled())
		{
			myComponents[componentIndex].Update(aDeltaTime);
		}
	}
	for (int componentIndex = 0; componentIndex < myOverflowingComponents.size(); componentIndex++)
	{
		if (myOverflowingComponents[componentIndex].GetEnabled())
		{
			myOverflowingComponents[componentIndex].Update(aDeltaTime);
		}
	}
}

template<class T, int MaxAmount>
inline void ComponentStore<T, MaxAmount>::Render()
{
	int totalSize = myLatestIndex - static_cast<int>(myOverflowingComponents.size());
	for (int componentIndex = 0; componentIndex < totalSize; componentIndex++)
	{
		if (myComponents[componentIndex].GetEnabled())
		{
			myComponents[componentIndex].Render();
		}
	}
	for (int componentIndex = 0; componentIndex < static_cast<int>(myOverflowingComponents.size()); componentIndex++)
	{
		if (myOverflowingComponents[componentIndex].GetEnabled())
		{
			myOverflowingComponents[componentIndex].Render();
		}
	}
}

template<class T, int MaxAmount>
inline void ComponentStore<T, MaxAmount>::CleanUp()
{
	int totalSize = myLatestIndex - static_cast<int>(myOverflowingComponents.size());
	for (int componentIndex = 0; componentIndex < totalSize; componentIndex++)
	{
		if (myComponents[componentIndex].GetMarkedDestroy())
		{
			RemoveComponent(myComponents[componentIndex].GetId());
		}
	}
	for (int componentIndex = 0; componentIndex < static_cast<int>(myOverflowingComponents.size()); componentIndex++)
	{
		if (myOverflowingComponents[componentIndex].GetMarkedDestroy())
		{
			RemoveComponent(myOverflowingComponents[componentIndex].GetId());
		}
	}
}

template<class T, int MaxAmount>
inline T* ComponentStore<T, MaxAmount>::AddComponent(T aComponent)
{
	// Gör så man kan lägga till i overflow
	int id = 0;
	if (myLatestIndex < myComponents.size())
	{
		if (myFreeIndexes.size() <= 0)
		{
			id = myLatestIndex;
			myLatestIndex += 1;
		}
		else
		{
			id = myFreeIndexes[myFreeIndexes.size() - 1];
			myFreeIndexes.pop_back();
		}
	}
	else
	{
		if (myLatestIndex == myComponents.size())
		{
			myOverflowingComponents.reserve(MaxAmount*10); // Galen mängd så vektorn inte flyr från pekarna
		}
		std::cout << "WARNING: " << typeid(T).name() << "Store is at max capacity " << myLatestIndex << "/" << MaxAmount << " , please increase the capacity in the " << typeid(T).name() << ".h file" << std::endl;
		myOverflowingComponents.push_back(aComponent);
		myLatestIndex += 1;
		return &myOverflowingComponents[myOverflowingComponents.size() - 1];
	}
	
	aComponent.SetId(id);
	aComponent.SetEnabled(true);
	myComponents[id] = aComponent;
	return &myComponents[id];
}

template<class T, int MaxAmount>
inline void ComponentStore<T, MaxAmount>::RemoveComponent(const int& aIndex)
{
	if (aIndex < myComponents.size())
	{
		if (aIndex == -1)
		{
			return;
		}

		if (myOverflowingComponents.empty())
		{
			myFreeIndexes.push_back(aIndex);
			myComponents[aIndex] = T();
			myComponents[aIndex].SetEnabled(false);
		}
		else
		{
			Component*& link = myOverflowingComponents[myOverflowingComponents.size()-1].GetLink(&myOverflowingComponents[myOverflowingComponents.size() - 1]);
			myComponents[aIndex] = myOverflowingComponents[myOverflowingComponents.size() - 1];
			link = &myComponents[aIndex];
			myOverflowingComponents.pop_back();
			myLatestIndex -= 1;
			//Update GameObject Pointers
		}
	}
	else
	{
		const int index = static_cast<int>(myComponents.size()) - 1 - aIndex;
		if (index < myOverflowingComponents.size() && index > 0)
		{
			Component*& link = myOverflowingComponents[myOverflowingComponents.size() - 1].GetLink(&myOverflowingComponents[myOverflowingComponents.size() - 1]);
			myOverflowingComponents[index] = myOverflowingComponents[myOverflowingComponents.size() - 1];
			link = &myComponents[aIndex];
			myOverflowingComponents.pop_back();
			myLatestIndex -= 1;
			//Update GameObject Pointers Tror inte den behövs här i med det är delete
		}
	}
	
}

template<class T, int MaxAmount>
inline std::array<T, MaxAmount>& ComponentStore<T, MaxAmount>::GetArray()
{
	return myComponents;
}

template<class T, int MaxAmount>
inline int ComponentStore<T, MaxAmount>::GetLatestIndex()
{
	return myLatestIndex;
}
