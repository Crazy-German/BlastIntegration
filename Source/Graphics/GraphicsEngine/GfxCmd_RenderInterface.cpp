#include "GraphicsEngine.pch.h"
#include "GfxCmd_RenderInterface.h"

#include "Material.h"
#include "ObjectBuffer.h"
#include "Sprite2D.h"

GfxCmd_RenderInterface::GfxCmd_RenderInterface(Sprite2D* aElement)
{
	myElement = aElement;
}



GfxCmd_RenderInterface::~GfxCmd_RenderInterface()
{
}

void GfxCmd_RenderInterface::Execute()
{
	GraphicsEngine::Get().ChangePipelineState((*GraphicsEngine::Get().GetSpritePSO()));
	GraphicsEngine::Get().SetPixelShader("UI_PS");
	GraphicsEngine::Get().SetVertexShader("UI_VS");
	GraphicsEngine::Get().RenderSprite2D(*myElement);
}

void GfxCmd_RenderInterface::Destroy()
{
	myElement = nullptr;
}
