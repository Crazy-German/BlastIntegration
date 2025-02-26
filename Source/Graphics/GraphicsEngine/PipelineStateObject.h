#pragma once

#include <wrl.h>
#include <array>
#include <string>
#include "PSOType.h"

class Texture;
struct Shader;
struct ID3D11BlendState;
struct ID3D11RasterizerState;
struct ID3D11DepthStencilState;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11InputLayout;

struct PipelineStateObject
{
	PipelineStateObject();
	~PipelineStateObject();

	Microsoft::WRL::ComPtr<ID3D11BlendState> BlendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> RasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthStencilState;

	std::array <std::shared_ptr<Texture>, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT> RenderTarget;
	std::array <bool, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT> ClearRenderTarget;

	std::shared_ptr<Texture> DepthStencil;
	bool ClearDepthStencil;

	std::unordered_map<unsigned, Microsoft::WRL::ComPtr<ID3D11SamplerState>> SamplerStates;

	int UsedRenderTargets;
	PSOType Type;
};