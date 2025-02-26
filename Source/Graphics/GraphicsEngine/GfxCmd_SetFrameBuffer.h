#pragma once
#include "GraphicsCommandList.h"


struct FrameBuffer;

class GfxCmd_SetFrameBuffer : public GraphicsCommandBase
{
public:
	GfxCmd_SetFrameBuffer(std::shared_ptr<FrameBuffer> aFrameBuffer);
	~GfxCmd_SetFrameBuffer() override = default;

	void Execute() override;
	void Destroy() override;

private:
	std::shared_ptr<FrameBuffer> myFrameBuffer;
};

