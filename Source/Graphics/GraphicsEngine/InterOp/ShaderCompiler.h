#pragma once

class ShaderCompiler
{
public:
	ShaderCompiler();
	~ShaderCompiler();
	uint8_t* CompileShader(const std::filesystem::path& aPath, const unsigned& aShaderType, size_t& inOutDataSize);

private:
	HRESULT CompileShaderInternal(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob);

private:
	const char* myVSShaderProfile;
	const char* myPSShaderProfile;
	const char* myGSShaderProfile;
	const char* myCurrentShaderProfile;
};
