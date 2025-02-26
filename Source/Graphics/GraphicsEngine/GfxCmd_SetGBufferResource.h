#pragma once
#include "GraphicsCommandList.h"

class GfxCmd_SetGBufferResource : GraphicsCommandBase
{
public:
	GfxCmd_SetGBufferResource();
	~GfxCmd_SetGBufferResource() override = default;
	void Execute() override;
	void Destroy() override;

private:

};

