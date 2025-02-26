#pragma once
#include "GraphicsCommandList.h"

class GfxCmd_TAPostProcess : public GraphicsCommandBase
{
public:
	GfxCmd_TAPostProcess();
	~GfxCmd_TAPostProcess() override = default;
	void Execute() override;
	void Destroy() override;
};