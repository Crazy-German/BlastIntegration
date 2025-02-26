#include "GraphicsEngine.pch.h"
#include "GfxCmd_RenderSpriteNoShaders.h"

#include "Material.h"
#include "ObjectBuffer.h"
#include "Sprite2D.h"

GfxCmd_RenderSpriteNoShaders::GfxCmd_RenderSpriteNoShaders(std::shared_ptr<Sprite2D> aElement)
{
	myElement = aElement;
}

GfxCmd_RenderSpriteNoShaders::~GfxCmd_RenderSpriteNoShaders()
{
}

void GfxCmd_RenderSpriteNoShaders::Execute()
{
	GraphicsEngine::Get().RenderSprite2D(*myElement);
}

void GfxCmd_RenderSpriteNoShaders::Destroy()
{

	myElement = nullptr;
}
