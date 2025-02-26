#pragma once
#include "Asset.h"
#include <GraphicsEngine/Text/Font.h>

struct FontAsset : public Asset
{
	//Add specific variables

	std::shared_ptr<Font> myFont;
	~FontAsset() override = default;
};