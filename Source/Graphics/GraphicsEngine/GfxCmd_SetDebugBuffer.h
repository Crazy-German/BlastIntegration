#pragma once
#include "GraphicsCommandList.h"
#ifndef RETAIL_
struct DebugBuffer;

class GfxCmd_SetDebugBuffer : public GraphicsCommandBase
{
public:
	GfxCmd_SetDebugBuffer(std::shared_ptr<DebugBuffer> aBuffer);

	~GfxCmd_SetDebugBuffer() override = default;
	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<DebugBuffer> myBuffer;
};
#endif
