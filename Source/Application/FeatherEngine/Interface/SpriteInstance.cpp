#include "FeatherEngine.pch.h"
#include "SpriteInstance.h"
#include <AssetManagement/AssetManager.h>
#include <GraphicsEngine/GraphicsCommandList.h>
#include <GraphicsEngine/GfxCmd_ChangePipelineState.h>
#include <GraphicsEngine/GfxCmd_SetTextureResource.h>
#include <GraphicsEngine/GraphicsEngine.h>
#include <MainSingleton.h>

SpriteInstance::SpriteInstance()
{
}

SpriteInstance::~SpriteInstance()
{
}

void SpriteInstance::Draw()
{
	//Needs to call renderer instead
	MainSingleton::Get().GetRenderer().Enqueue<GfxCmd_ChangePipelineState>(GraphicsEngine::Get().GetSpritePSO());
	MainSingleton::Get().GetRenderer().Enqueue<GfxCmd_SetTextureResource>(PIPELINE_STAGE_PIXEL_SHADER, 0, mySprite->GetTexture());
	mySprite->Draw();
}
