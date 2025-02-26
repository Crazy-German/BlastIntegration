#include "GraphicsEngine.pch.h"
#include "GfxCmd_SetGBufferResource.h"

GfxCmd_SetGBufferResource::GfxCmd_SetGBufferResource() = default;

void GfxCmd_SetGBufferResource::Execute()
{
	GraphicsEngine::Get().SetGBufferResource();
}

void GfxCmd_SetGBufferResource::Destroy()
{
}
