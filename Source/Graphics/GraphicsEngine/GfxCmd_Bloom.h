#pragma once
#include "GraphicsCommandList.h"

class GfxCmd_Bloom : public GraphicsCommandBase
{
public:
	GfxCmd_Bloom();
	~GfxCmd_Bloom() override = default;

	void Execute() override;
	void Destroy() override;

private:

};
