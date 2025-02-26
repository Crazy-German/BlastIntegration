#include "GraphicsEngine.pch.h"
#include "GfxCmd_Tonemap.h"

GfxCmd_Tonemap::GfxCmd_Tonemap() = default;

void GfxCmd_Tonemap::Execute()
{
	auto& GE = GraphicsEngine::Get();
	GE.BeginEvent("Tonemapping");
	GE.SetPixelShader("Tonemap_PS");
	GE.SetVertexShader("Quad_VS");
	GE.ChangePipelineState(*GE.GetTonemapPSO());
	GE.SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 126, *GE.GetIntermediateLDRBuffer());
	GE.DrawQuad();
	GE.EndEvent();
	GE.ClearTextureResourceSlot(static_cast<unsigned>(PIPELINE_STAGE_PIXEL_SHADER), 126);
}

void GfxCmd_Tonemap::Destroy()
{
}
