#include "GraphicsEngine.pch.h"
#include "GfxCmd_Present.h"


GfxCmd_Present::GfxCmd_Present() = default;

void GfxCmd_Present::Execute()
{
	GraphicsEngine::Get().Present();
}

void GfxCmd_Present::Destroy()
{
}
