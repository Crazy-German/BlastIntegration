#pragma once
#include "GraphicsCommandList.h"


class Texture;

class GfxCmd_SetTextureResource : public GraphicsCommandBase
{
public:
	GfxCmd_SetTextureResource(unsigned aPipelineStages, unsigned aSlot, std::shared_ptr<Texture> aTexture);
	~GfxCmd_SetTextureResource() override = default;

	void Execute() override;
	void Destroy() override;
private:
	unsigned myPipelineStages;
	unsigned mySlot;
	std::shared_ptr<Texture> myTexture;
};

