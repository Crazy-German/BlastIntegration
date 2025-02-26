#include "GraphicsEngine.pch.h"
#include "GfxCmd_DrawQuad.h"
GfxCmd_DrawQuad::GfxCmd_DrawQuad(const CU::Vector2f& aSize)
{
	mySize = aSize;

}

void GfxCmd_DrawQuad::Execute()
{
	GraphicsEngine::Get().DrawQuad(mySize);
}

void GfxCmd_DrawQuad::Destroy()
{
}
