#pragma once
#include "GraphicsCommandList.h"

class GfxCmd_ClearBackBuffer : public GraphicsCommandBase
{
public:
	GfxCmd_ClearBackBuffer();
	~GfxCmd_ClearBackBuffer() override = default;

	void Execute() override;
	void Destroy() override;
};

