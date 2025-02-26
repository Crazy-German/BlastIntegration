#pragma once
#include "GraphicsCommandList.h"
#include "MiscBuffer.h"

struct FrameBuffer;

class GfxCmd_SetMiscBuffer : public GraphicsCommandBase
{
public:
	GfxCmd_SetMiscBuffer(MiscBuffer aMiscBuffer);

	~GfxCmd_SetMiscBuffer() override = default;

	void Execute() override;
	void Destroy() override;



private:
	MiscBuffer myMiscBuffer;
};

