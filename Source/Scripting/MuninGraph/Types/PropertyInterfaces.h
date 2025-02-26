#pragma once

class VectorProperty;
class SingleProperty;

template<typename T>
class SinglePropertyInterface
{
public:
	SinglePropertyInterface(SingleProperty* aProperty)
		: myProperty(aProperty)
	{
		ensure(typeid(T) == aProperty->GetType());
	}

	SinglePropertyInterface& operator=(T& aValue)
	{
		myProperty->Set(&aValue, sizeof(T));
		return *this;
	}

	SinglePropertyInterface& operator=(const T& aValue)
	{
		myProperty->Set(&aValue, sizeof(T));
		return *this;
	}

	operator T& ()
	{
		return *static_cast<T*>(myProperty->Data());
	}

	T* operator->()
	{
		return static_cast<T*>(myProperty->Data());
	}

private:

	SingleProperty* myProperty;
};

template<typename T>
class VectorPropertyInterface
{
public:
	VectorPropertyInterface(VectorProperty* aProperty)
		: myProperty(aProperty)
	{
		ensure(typeid(T) == aProperty->GetType());
	}

	operator std::vector<T>& ()
	{
		return *static_cast<std::vector<T>*>(myProperty->Data());
	}

	bool operator==(const VectorPropertyInterface& aOther) const
	{
		const std::vector<T>& A = *static_cast<std::vector<T>*>(myProperty->Data());
		const std::vector<T>& B = *static_cast<std::vector<T>*>(aOther.myProperty->Data());
		return A == B;
	}

	std::vector<T>* operator->()
	{
		return static_cast<std::vector<T>*>(myProperty->Data());
	}

private:

	VectorProperty* myProperty;
};

template<typename K, typename V>
class MapPropertyInterface
{
public:
	MapPropertyInterface(MapProperty* aProperty)
		: myProperty(aProperty)
	{
		ensure(typeid(K) == aProperty->GetKeyType());
		ensure(typeid(V) == aProperty->GetValueType());
	}

	bool operator==(const MapPropertyInterface& aOther) const
	{
		const std::unordered_map<K, V>& A = *static_cast<std::unordered_map<K, V>*>(myProperty->Data());
		const std::unordered_map<K, V>& B = *static_cast<std::unordered_map<K, V>*>(aOther.myProperty->Data());
		return A == B;
	}

	operator std::unordered_map<K, V>& ()
	{
		return *static_cast<std::unordered_map<K, V>*>(myProperty->Data());
	}

	std::unordered_map<K, V>* operator->()
	{
		return static_cast<std::unordered_map<K, V>*>(myProperty->Data());
	}

private:

	MapProperty* myProperty;
};