#pragma once
#include "GraphicsCommandList.h"
struct Shader;

class GfxCmd_SetPixelShader : GraphicsCommandBase
{
public:
	GfxCmd_SetPixelShader(const std::string_view& aShaderName);
	~GfxCmd_SetPixelShader() override = default; 
	void Execute() override;
	void Destroy() override;
private:
	std::string_view myShaderName;
};

class GfxCmd_SetVertexShader : GraphicsCommandBase
{
public:
	GfxCmd_SetVertexShader();
	GfxCmd_SetVertexShader(const std::string_view& aShaderName);
	~GfxCmd_SetVertexShader() override = default;
	void Execute() override;
	void Destroy() override;
private:

	std::string_view myShaderName;
};

class GfxCmd_SetGeometryShader : GraphicsCommandBase
{
public:
	GfxCmd_SetGeometryShader();
	GfxCmd_SetGeometryShader(const std::string_view& aShaderName);
	~GfxCmd_SetGeometryShader() override = default;
	void Execute() override;
	void Destroy() override;
private:

	std::string_view myShaderName;
};