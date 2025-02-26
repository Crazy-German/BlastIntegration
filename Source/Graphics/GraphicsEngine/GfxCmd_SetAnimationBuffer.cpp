#include "GraphicsEngine.pch.h"
#include "AnimationBuffer.h"
#include "GfxCmd_SetAnimationBuffer.h"

GfxCmd_SetAnimationBuffer::GfxCmd_SetAnimationBuffer(std::shared_ptr<AnimationBuffer> aAnimationBuffer)
{
	myAnimationBuffer = aAnimationBuffer;
}

GfxCmd_SetAnimationBuffer::~GfxCmd_SetAnimationBuffer()
{

}

void GfxCmd_SetAnimationBuffer::Execute()
{
	if(myAnimationBuffer)
	{
		GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::AnimationBuffer, *myAnimationBuffer);
	}
}

void GfxCmd_SetAnimationBuffer::Destroy()
{
	myAnimationBuffer = nullptr;
}
