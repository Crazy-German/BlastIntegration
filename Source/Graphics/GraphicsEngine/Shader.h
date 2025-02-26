#pragma once
#include <wrl.h>

struct ID3D11DeviceChild;
struct ID3D11InputLayout;

enum class ShaderType : unsigned
{
	Unknown,
	VertexShader,
	GeometryShader,
	PixelShader
};

struct Shader
{
public:
	virtual ~Shader() = default;
	ShaderType type = ShaderType::Unknown;
	Microsoft::WRL::ComPtr<ID3D11DeviceChild> shader;
	std::filesystem::path path;
};

struct VertexShader : public Shader
{
public:
	~VertexShader() override = default;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	unsigned VertexStride = 0;
};