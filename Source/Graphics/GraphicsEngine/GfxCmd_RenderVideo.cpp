#include "GraphicsEngine.pch.h"
#include "GfxCmd_RenderVideo.h"
#include "Texture.h"

GfxCmd_RenderVideo::GfxCmd_RenderVideo(std::shared_ptr<Texture> aTexture/*, std::shared_ptr<Sprite2D> aSprite*/)
{
	myTexture = aTexture;
}

GfxCmd_RenderVideo::~GfxCmd_RenderVideo()
{
}

void GfxCmd_RenderVideo::Execute()
{
	auto& graphics = GraphicsEngine::Get();
	graphics.SetVertexShader("Quad_VS");
	graphics.SetPixelShader("Video_PS");
	graphics.ChangePipelineState(*graphics.GetVideoPSO());
	graphics.SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 0, *myTexture);
	graphics.DrawQuad({ GetViewportSize(graphics.GetViewPort())});
}

void GfxCmd_RenderVideo::Destroy()
{
	myTexture = nullptr;
	mySprite = nullptr;

}
