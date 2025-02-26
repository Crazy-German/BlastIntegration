#include "GraphicsEngine.pch.h"
#include "GfxCmd_SetTextureResource.h"



GfxCmd_SetTextureResource::GfxCmd_SetTextureResource(unsigned aPipelineStages, unsigned aSlot, std::shared_ptr<Texture> aTexture)
{
	myPipelineStages = aPipelineStages;
	mySlot = aSlot;
	myTexture = aTexture;
}

void GfxCmd_SetTextureResource::Execute()
{
	GraphicsEngine::Get().SetTextureResource(myPipelineStages, mySlot, *myTexture);
}

void GfxCmd_SetTextureResource::Destroy()
{
	myTexture = nullptr;
}
