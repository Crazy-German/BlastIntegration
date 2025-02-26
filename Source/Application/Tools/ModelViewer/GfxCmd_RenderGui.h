#pragma once
#include "GraphicsEngine/GraphicsCommandList.h"

class ModelViewerGui;

class GfxCmd_RenderGui : public GraphicsCommandBase
{
public:
	GfxCmd_RenderGui();
	~GfxCmd_RenderGui() override = default;

	void Execute() override;
};