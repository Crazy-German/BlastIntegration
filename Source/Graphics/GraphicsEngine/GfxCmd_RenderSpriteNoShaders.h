#pragma once
#include "GraphicsCommandList.h"
#include "CommonUtilities/Matrix4x4.hpp"

class Sprite2D;

class GfxCmd_RenderSpriteNoShaders : public GraphicsCommandBase
{
public:
	GfxCmd_RenderSpriteNoShaders(std::shared_ptr<Sprite2D> aElement);
	~GfxCmd_RenderSpriteNoShaders() override;

	void Execute() override;
	void Destroy() override;

private:

	std::shared_ptr<Sprite2D> myElement;
};

