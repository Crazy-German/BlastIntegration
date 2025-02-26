#include "GraphicsEngine.pch.h"
#include "GfxCmd_ClearbackBuffer.h"

GfxCmd_ClearBackBuffer::GfxCmd_ClearBackBuffer() = default;

void GfxCmd_ClearBackBuffer::Execute()
{
	GraphicsEngine::Get().ClearBackBuffer();
}

void GfxCmd_ClearBackBuffer::Destroy()
{
}
