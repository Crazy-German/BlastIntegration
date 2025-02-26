#pragma once
#include "GraphicsCommandList.h"

class GfxCmd_Tonemap : GraphicsCommandBase
{
public:
	GfxCmd_Tonemap();
	~GfxCmd_Tonemap() override = default;
	void Execute() override;
	void Destroy() override;

private:

};

