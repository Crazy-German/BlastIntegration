#include "GraphicsEngine.pch.h"
#include "GfxCmd_SetCustomTextures.h"

GfxCmd_SetCustomTextures::GfxCmd_SetCustomTextures(unsigned aPipelineStages)
{
	myPipelineStages = aPipelineStages;
}

void GfxCmd_SetCustomTextures::Execute()
{
	GraphicsEngine::Get().SetCustomTextureResources(myPipelineStages);
}

void GfxCmd_SetCustomTextures::Destroy()
{

}
