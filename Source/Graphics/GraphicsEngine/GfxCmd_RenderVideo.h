#pragma once
#include "GraphicsCommandList.h"

class Texture;

class GfxCmd_RenderVideo : public GraphicsCommandBase
{
public:
	GfxCmd_RenderVideo(std::shared_ptr<Texture> aTexture/*, std::shared_ptr<Sprite2D> aSprite*/);
	~GfxCmd_RenderVideo() override;

	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<Texture> myTexture;
	std::shared_ptr<Sprite2D> mySprite;

};

