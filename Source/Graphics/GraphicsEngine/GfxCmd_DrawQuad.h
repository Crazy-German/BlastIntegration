#pragma once
#include "GraphicsCommandList.h"

class GfxCmd_DrawQuad : public GraphicsCommandBase
{
public:
	GfxCmd_DrawQuad(const CU::Vector2f& aSize = {-1, -1});
	~GfxCmd_DrawQuad() override = default;

	void Execute() override;

	void Destroy() override;
private:
	CU::Vector2f mySize;
};

