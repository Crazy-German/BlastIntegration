#include "GraphicsEngine.pch.h"
#include "ShaderCompiler.h"
#include "Shader.h"

#include <stdio.h>

#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

ShaderCompiler::ShaderCompiler()
{
	myVSShaderProfile = "vs_5_0";
	myPSShaderProfile = "ps_5_0";
	myGSShaderProfile = "gs_5_0";
	myCurrentShaderProfile = myVSShaderProfile;
}


ShaderCompiler::~ShaderCompiler()
{
}


uint8_t* ShaderCompiler::CompileShader(const std::filesystem::path& aPath, const unsigned& aShaderType, size_t& inOutDataSize)
{
	HRESULT hr = {};
	ID3DBlob* shader = nullptr;

	switch (static_cast<ShaderType>(aShaderType))
	{
	case ShaderType::Unknown:
#ifndef _RETAIL

		LOG(GraphicsEngineLog, Error, "Cannot compile unspecified shader type!");
#endif
		return nullptr;
	case ShaderType::VertexShader:
		myCurrentShaderProfile = myVSShaderProfile;
		break;
	case ShaderType::GeometryShader:
		myCurrentShaderProfile = myGSShaderProfile;
		break;
	case ShaderType::PixelShader:
		myCurrentShaderProfile = myPSShaderProfile;
		break;
	default:
		break;
	}

	hr = CompileShaderInternal(aPath.c_str(), "main", myCurrentShaderProfile, &shader);
	if (FAILED(hr))
	{
#ifndef _RETAIL
		LOG(GraphicsEngineLog, Error, "Failed compiling shader with profile {}", myCurrentShaderProfile);
#endif 

		return nullptr;
	}
#ifndef _RETAIL

	LOG(GraphicsEngineLog, Log, "Compiled shader: {}", aPath.filename().string());
#endif

	inOutDataSize = shader->GetBufferSize();
	return reinterpret_cast<uint8_t*>(shader->GetBufferPointer());
}



HRESULT ShaderCompiler::CompileShaderInternal(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob)
{
	if (!srcFile || !entryPoint || !profile || !blob)
		return E_INVALIDARG;

	*blob = nullptr;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	const D3D_SHADER_MACRO defines[] =
	{
		"EXAMPLE_DEFINE", "1",
		NULL, NULL
	};

	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(srcFile, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint, profile,
		flags, 0, &shaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (shaderBlob)
			shaderBlob->Release();

		return hr;
	}

	*blob = shaderBlob;

	return hr;
}
