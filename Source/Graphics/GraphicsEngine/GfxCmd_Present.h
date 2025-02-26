#pragma once
#include "GraphicsCommandList.h"

class GfxCmd_Present : public GraphicsCommandBase
{
public:
	GfxCmd_Present();
	~GfxCmd_Present() override = default;

	void Execute() override;
	void Destroy() override;
};