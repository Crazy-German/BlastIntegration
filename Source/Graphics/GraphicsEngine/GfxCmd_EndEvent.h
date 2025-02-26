#pragma once
#include "GraphicsCommandList.h"
class GfxCmd_EndEvent : public GraphicsCommandBase
{
public:
	GfxCmd_EndEvent() = default;
	~GfxCmd_EndEvent() override;
	void Execute() override;
	void Destroy() override;

private:
	std::string_view myName;
};


