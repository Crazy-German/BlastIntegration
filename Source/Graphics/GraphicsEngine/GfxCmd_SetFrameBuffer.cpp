#include "GraphicsEngine.pch.h"
#include "GfxCmd_SetFrameBuffer.h"
#include "FrameBuffer.h"


GfxCmd_SetFrameBuffer::GfxCmd_SetFrameBuffer(std::shared_ptr<FrameBuffer> aFrameBuffer)
{
	myFrameBuffer = aFrameBuffer;
}

void GfxCmd_SetFrameBuffer::Execute()
{
	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::FrameBuffer, *myFrameBuffer);
}

void GfxCmd_SetFrameBuffer::Destroy()
{
	myFrameBuffer = nullptr;
}
