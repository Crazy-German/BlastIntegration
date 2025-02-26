#pragma once
#include "GraphicsCommandList.h"
#include "CommonUtilities/Matrix4x4.hpp"

class Sprite2D;

class GfxCmd_RenderInterface : public GraphicsCommandBase
{
public:
	GfxCmd_RenderInterface(Sprite2D* aElement);

	~GfxCmd_RenderInterface() override;

	void Execute() override;
	void Destroy() override;

private:
	Sprite2D* myElement;
};

