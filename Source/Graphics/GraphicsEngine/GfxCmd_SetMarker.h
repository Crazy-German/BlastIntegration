#pragma once

#include "GraphicsCommandList.h"

class GfxCmd_SetMarker : public GraphicsCommandBase
{
public:


	explicit GfxCmd_SetMarker(std::string_view aMarker);
	~GfxCmd_SetMarker() override = default;
	void Execute() override;
	void Destroy() override;

private:
	std::string_view myMarker;
};


