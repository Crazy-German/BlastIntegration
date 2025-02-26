#include "GraphicsEngine.pch.h"
#include "GfxCmd_RenderText.h"
#include "Text/TextFactory.h"
#include "GraphicsEngine.h"

GfxCmd_RenderText::GfxCmd_RenderText(std::shared_ptr<Text> aText)
{
	myText = aText;

}

GfxCmd_RenderText::~GfxCmd_RenderText()
{
}

void GfxCmd_RenderText::Execute()
{
	if(!myText)
	{
		return;
	}
	auto& graphics = GraphicsEngine::Get();
	graphics.SetPixelShader("Text_PS");
	graphics.SetVertexShader("Text_VS");
	graphics.ChangePipelineState(*graphics.GetTextPSO());
	graphics.RenderText(*myText);
}

void GfxCmd_RenderText::Destroy()
{
	myText = nullptr;
}
