#include "GraphicsEngine.pch.h"
#include "GfxCmd_SetMiscBuffer.h"
GfxCmd_SetMiscBuffer::GfxCmd_SetMiscBuffer(MiscBuffer aMiscBuffer)
{
	myMiscBuffer = aMiscBuffer;
}

void GfxCmd_SetMiscBuffer::Execute()
{
	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::MiscBuffer, myMiscBuffer);
}

void GfxCmd_SetMiscBuffer::Destroy()
{


}
