#pragma once
#include "GraphicsCommandList.h"
#include "LightBuffer.h"

class GfxCmd_SetLightBuffer : public GraphicsCommandBase
{
public:
	GfxCmd_SetLightBuffer(const LightBuffer& aLightBuffer);
	~GfxCmd_SetLightBuffer() override = default;

	void Execute() override;
	void Destroy() override;

private:

	LightBuffer myLightBuffer;
};