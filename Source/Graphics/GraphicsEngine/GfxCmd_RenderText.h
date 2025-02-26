#pragma once
#include "GraphicsCommandList.h"

class Text;
class GfxCmd_RenderText : public GraphicsCommandBase
{
public:
	GfxCmd_RenderText(std::shared_ptr<Text> aText);
	~GfxCmd_RenderText() override;

	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<Text> myText;
};
