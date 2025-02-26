#pragma once
#include "GraphicsCommandList.h"
class GfxCmd_SetCustomTextures : public GraphicsCommandBase
{
public:
	GfxCmd_SetCustomTextures(unsigned aPipelineStages);
	~GfxCmd_SetCustomTextures() override = default ;

	void Execute() override;
	void Destroy() override;
private:

	unsigned myPipelineStages;
};
