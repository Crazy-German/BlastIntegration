#include "GraphicsEngine.pch.h"
#include "PipelineStateObject.h"

#include "Texture.h"
#include "Shader.h"

PipelineStateObject::PipelineStateObject() 
{
	for (int i = 0; i < ClearRenderTarget.size(); i++)
	{
		ClearRenderTarget[i] = false;
		ClearDepthStencil = false;
	}
}
PipelineStateObject::~PipelineStateObject() = default;
