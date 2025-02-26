#include "GraphicsEngine.pch.h"
#include "GfxCmd_Bloom.h"

GfxCmd_Bloom::GfxCmd_Bloom()
{

}

void GfxCmd_Bloom::Execute()
{
	GraphicsEngine& ge = GraphicsEngine::Get();
	ge.BeginEvent("Blooming");

	ge.ChangePipelineState(*GraphicsEngine::Get().GetLuminancePSO());
	ge.SetVertexShader("Quad_VS");
	ge.SetPixelShader("Luminance_PS");
	ge.SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 126, *GraphicsEngine::Get().GetIntermediateLDRBuffer());
	ge.DrawQuad();
	ge.SetMarker("Shrinking");
	ge.SetPixelShader("Resample_PS");
	ge.ChangePipelineState(*GraphicsEngine::Get().GetResamplingPSO()); // 1/2 size
	ge.DrawQuad(GraphicsEngine::Get().GetHalfSizeScreenBuffer()->GetSize());
	ge.ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 126);

	ge.ChangeRenderTarget(GraphicsEngine::Get().GetQuarterSizeScreenBuffer()); // 1/4 size
	ge.SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 126, *GraphicsEngine::Get().GetHalfSizeScreenBuffer());
	ge.DrawQuad(GraphicsEngine::Get().GetQuarterSizeScreenBuffer()->GetSize());
	ge.ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 126);

	ge.ChangeRenderTarget(GraphicsEngine::Get().GetEightSizeScreenBufferA()); // 1/8 size
	ge.SetTextureResource((PIPELINE_STAGE_PIXEL_SHADER), 126, *GraphicsEngine::Get().GetQuarterSizeScreenBuffer());
	ge.DrawQuad(GraphicsEngine::Get().GetEightSizeScreenBufferA()->GetSize());
	ge.ClearTextureResourceSlot((PIPELINE_STAGE_PIXEL_SHADER), 126);
	ge.SetMarker("Gaussian");
	for (size_t i = 0; i < 6; i++)
	{
		ge.SetPixelShader("GaussianH_PS");
		ge.ChangePipelineState(*GraphicsEngine::Get().GetGaussianHPSO());
		ge.SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 126, *GraphicsEngine::Get().GetEightSizeScreenBufferA());
		ge.DrawQuad(GraphicsEngine::Get().GetEightSizeScreenBufferB()->GetSize());
		ge.ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 126);

		ge.SetPixelShader("GaussianV_PS");
		ge.ChangePipelineState(*GraphicsEngine::Get().GetGaussianVPSO());
		ge.SetTextureResource((PIPELINE_STAGE_PIXEL_SHADER), 126, *GraphicsEngine::Get().GetEightSizeScreenBufferB());
		ge.DrawQuad(GraphicsEngine::Get().GetEightSizeScreenBufferA()->GetSize());
		ge.ClearTextureResourceSlot ((PIPELINE_STAGE_PIXEL_SHADER), 126);
	}
	ge.SetMarker("Upsampling");
	ge.SetPixelShader("Resample_PS");
	ge.ChangePipelineState(*GraphicsEngine::Get().GetResamplingPSO());
	ge.ChangeRenderTarget(GraphicsEngine::Get().GetQuarterSizeScreenBuffer()); // 1/4 size
	ge.SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 126, *GraphicsEngine::Get().GetEightSizeScreenBufferA());
	ge.DrawQuad(GraphicsEngine::Get().GetQuarterSizeScreenBuffer()->GetSize());
	ge.ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 126);

	ge.ChangeRenderTarget(GraphicsEngine::Get().GetHalfSizeScreenBuffer()); // 1/2 size
	ge.SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 126, *GraphicsEngine::Get().GetQuarterSizeScreenBuffer());
	ge.DrawQuad(GraphicsEngine::Get().GetHalfSizeScreenBuffer()->GetSize());
	ge.ClearTextureResourceSlot((PIPELINE_STAGE_PIXEL_SHADER), 126);

	ge.ChangeRenderTarget(GraphicsEngine::Get().GetLuminanceBuffer());
	ge.SetTextureResource((PIPELINE_STAGE_PIXEL_SHADER), 126, *GraphicsEngine::Get().GetHalfSizeScreenBuffer());
	ge.DrawQuad();
	ge.ClearTextureResourceSlot((PIPELINE_STAGE_PIXEL_SHADER), 126);

	ge.SetMarker("Blending");
	ge.SetPixelShader("Bloom_PS");
	ge.ChangePipelineState(*GraphicsEngine::Get().GetBloomPSO());
	ge.SetTextureResource((PIPELINE_STAGE_PIXEL_SHADER), 126, *GraphicsEngine::Get().GetLuminanceBuffer());
	ge.SetTextureResource((PIPELINE_STAGE_PIXEL_SHADER), 125, *GraphicsEngine::Get().GetIntermediateHDRBuffer());
	ge.DrawQuad();
	ge.ClearTextureResourceSlot((PIPELINE_STAGE_PIXEL_SHADER), 125);
	ge.ClearTextureResourceSlot((PIPELINE_STAGE_PIXEL_SHADER), 126);
	ge.EndEvent();
}

void GfxCmd_Bloom::Destroy()
{

}
