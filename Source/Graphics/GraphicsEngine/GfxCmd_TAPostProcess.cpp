#include "GfxCmd_TAPostProcess.h"

GfxCmd_TAPostProcess::GfxCmd_TAPostProcess()
{
}

void GfxCmd_TAPostProcess::Execute()
{
	GraphicsEngine& ge = GraphicsEngine::Get();
	ge.BeginEvent("PP shaders");
	ge.ChangePipelineState(*ge.GetCustomPPPSO());

	ge.SetGBufferResource();
	ge.SetVertexShader("Quad_VS");
	for(auto index : ge.GetTAPostProcessShaders())
	{
		ge.SetShader(index);
		ge.DrawQuad();
	}
	ge.EndEvent();
}

void GfxCmd_TAPostProcess::Destroy()
{
}
