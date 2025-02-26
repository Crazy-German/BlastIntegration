#include "GraphicsEngine.pch.h"
#include "GfxCmd_SetLightBuffer.h"
#include "LightBuffer.h"

GfxCmd_SetLightBuffer::GfxCmd_SetLightBuffer(const LightBuffer& aLightBuffer)
{
	myLightBuffer = aLightBuffer;
}

void GfxCmd_SetLightBuffer::Execute()
{
	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::LightBuffer, myLightBuffer);
}

void GfxCmd_SetLightBuffer::Destroy()
{

}
