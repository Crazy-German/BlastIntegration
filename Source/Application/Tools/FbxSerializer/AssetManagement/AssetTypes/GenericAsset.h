#pragma once
#include "Asset.h"
template <typename T>
struct GenericAsset : public Asset
{
	std::shared_ptr<T> myAsset;
	~GenericAsset() override = default;
};