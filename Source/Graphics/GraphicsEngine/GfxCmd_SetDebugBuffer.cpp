#include "GraphicsEngine.pch.h"
#include "GfxCmd_SetDebugBuffer.h"
#ifndef _RETAIL
#include "DebugBuffer.h"
GfxCmd_SetDebugBuffer::GfxCmd_SetDebugBuffer(std::shared_ptr<DebugBuffer> aBuffer)
{
	myBuffer = aBuffer;
}

void GfxCmd_SetDebugBuffer::Execute()
{

	GraphicsEngine::Get().UpdateAndSetConstantBuffer<DebugBuffer>(ConstantBufferType::DebugBuffer, *myBuffer);
}

void GfxCmd_SetDebugBuffer::Destroy()
{
	myBuffer = nullptr;
}
#endif
