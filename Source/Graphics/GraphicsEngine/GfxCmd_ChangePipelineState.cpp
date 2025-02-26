#include "GraphicsEngine.pch.h"
#include "GfxCmd_ChangePipelineState.h"
#include "PipelineStateObject.h"

GfxCmd_ChangePipelineState::GfxCmd_ChangePipelineState()
{
	myPipelineStateObject = GraphicsEngine::Get().GetForwardPSO();
}

GfxCmd_ChangePipelineState::GfxCmd_ChangePipelineState(PipelineStateObject* aPipelineStateObject)
{
	myPipelineStateObject = aPipelineStateObject;
}

GfxCmd_ChangePipelineState::~GfxCmd_ChangePipelineState()
{
	myPipelineStateObject = nullptr;
}

void GfxCmd_ChangePipelineState::Execute()
{
	GraphicsEngine::Get().ChangePipelineState(*myPipelineStateObject);
}

void GfxCmd_ChangePipelineState::Destroy()
{
	myPipelineStateObject = nullptr;

}
