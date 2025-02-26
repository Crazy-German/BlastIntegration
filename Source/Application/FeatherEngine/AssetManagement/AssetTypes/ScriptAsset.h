#pragma once
#include "Asset.h"

struct ScriptAsset: Asset
{
	std::shared_ptr<std::vector<uint8_t>> myScriptData;
	~ScriptAsset() override = default;
};