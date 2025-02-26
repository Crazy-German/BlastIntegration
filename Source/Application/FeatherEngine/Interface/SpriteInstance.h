#pragma once
#include "GraphicsEngine/Sprite2D.h"
#include <array>
#include <AssetManagement/AssetTypes/Sprite.h>
#include "InterfaceComponent.h"

class SpriteInstance : public InterfaceComponent
{
public:
	SpriteInstance();
	~SpriteInstance();

	void Draw() override;

protected:

};