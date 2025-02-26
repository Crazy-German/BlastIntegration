#include "GraphicsEngine.pch.h"
#include "GfxCmd_SetShaders.h"

GfxCmd_SetPixelShader::GfxCmd_SetPixelShader(const std::string_view& aShaderName)
{
	myShaderName = aShaderName;
}

void GfxCmd_SetPixelShader::Execute()
{
	GraphicsEngine::Get().SetPixelShader(myShaderName);
}

void GfxCmd_SetPixelShader::Destroy()
{
	myShaderName.~basic_string_view();
}

GfxCmd_SetVertexShader::GfxCmd_SetVertexShader() = default;

GfxCmd_SetVertexShader::GfxCmd_SetVertexShader(const std::string_view& aShaderName)
{
	myShaderName = aShaderName;
}


void GfxCmd_SetVertexShader::Execute()
{
	GraphicsEngine::Get().SetVertexShader(myShaderName);
}

void GfxCmd_SetVertexShader::Destroy()
{


	myShaderName.~basic_string_view();
}

GfxCmd_SetGeometryShader::GfxCmd_SetGeometryShader()
{
}

GfxCmd_SetGeometryShader::GfxCmd_SetGeometryShader(const std::string_view& aShaderName)
{
	myShaderName = aShaderName;
}

void GfxCmd_SetGeometryShader::Execute()
{
	GraphicsEngine::Get().SetGeometryShader(myShaderName);
}

void GfxCmd_SetGeometryShader::Destroy()
{
	myShaderName.~basic_string_view();
}
