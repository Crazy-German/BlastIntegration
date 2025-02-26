#pragma once
#include "GraphicsCommandList.h"

struct PipelineStateObject;

class GfxCmd_ChangePipelineState : public GraphicsCommandBase
{
public:
	GfxCmd_ChangePipelineState();
	GfxCmd_ChangePipelineState(PipelineStateObject* aPipelineStateObject);
	~GfxCmd_ChangePipelineState() override;

	void Execute() override;

	void Destroy() override;

private:
	PipelineStateObject* myPipelineStateObject;
};



