#include "GraphicsEngine.pch.h"
#include "RenderHardwareInterface.h"
#include <d3d11_1.h>
#include "ConstantBuffer.h"

#include <d3dcompiler.h>
#include <d3d11shader.h>

#include "PipelineStateObject.h"
#include "stringHelpers.h"
#include "Texture.h"
#include "Shader.h"
#include "InterOp/DDSTextureLoader11.h"

#ifdef _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(RhiLog, "RHI", Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(RhiLog, "RHI", Error);
#endif

DEFINE_LOG_CATEGORY(RhiLog);

using namespace Microsoft::WRL;

RenderHardwareInterface::RenderHardwareInterface() = default;
RenderHardwareInterface::~RenderHardwareInterface() = default;

bool RenderHardwareInterface::Initialize(HWND aWindowHandle, bool aEnableDebug, unsigned aResolutionWidth, unsigned aResolutionHeight)
{
	HRESULT result = E_FAIL;

	ComPtr<IDXGIFactory> dxFactory;
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), &dxFactory);
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create DX Factory");
		return false;
	}
	LOG(RhiLog, Log, "Initializing RHI...");

	ComPtr<IDXGIAdapter> tempAdapter;
	std::vector<ComPtr<IDXGIAdapter>> adapters;
	unsigned adapterCount = 0;
	while (dxFactory->EnumAdapters(adapterCount, &tempAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		adapters.push_back(tempAdapter);
		adapterCount++;
	}

	ComPtr<IDXGIAdapter> selectedAdapter;
	size_t selectedAdapterVRAM = 0;
	DXGI_ADAPTER_DESC selectedAdapterDesc = {};
	for (const auto& adapter : adapters)
	{
		DXGI_ADAPTER_DESC currentDesc = {};
		adapter->GetDesc(&currentDesc);

		if (currentDesc.DedicatedVideoMemory > selectedAdapterVRAM)
		{
			selectedAdapterVRAM = currentDesc.DedicatedVideoMemory;
			selectedAdapter = adapter;
			selectedAdapterDesc = currentDesc;
		}
	}

	const wchar_t* wideAdapterName = selectedAdapterDesc.Description;
	std::string adapterName = str::wide_to_utf8(wideAdapterName);
	LOG(RhiLog, Log, "Selected adapter is {}", adapterName);

	constexpr size_t megabyte = (1024ULL * 1024ULL);
	if (selectedAdapterVRAM > megabyte)
	{
		selectedAdapterVRAM /= megabyte;
	}
	LOG(RhiLog, Log, " * VRAM: {} MB", selectedAdapterVRAM);

	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	result = D3D11CreateDevice(
		selectedAdapter.Get(),
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL,
		aEnableDebug ? D3D11_CREATE_DEVICE_DEBUG : 0,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&device,
		NULL,
		&context
	);
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create D3D11 device");
		return false;
	}

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = aWindowHandle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
	swapChainDesc.Windowed = true;



	ComPtr<IDXGISwapChain> swapChain;
	result = dxFactory->CreateSwapChain(device.Get(), &swapChainDesc, &swapChain);
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create SwapChain");
		return false;
	}

	//myBackBuffer->GetRTV()->Release();
	//myDepthBuffer->GetDSV()->Release();
	//myContext->Flush();

	result = swapChain->ResizeBuffers(2, aResolutionWidth, aResolutionHeight, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to resize swapchain buffer");
		return false;
	}

	ComPtr<ID3D11Texture2D> backBufferTexture;
	result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBufferTexture);
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to retrieve back buffer");
		return false;
	}

	myBackBuffer = std::make_shared<Texture>();
	result = device->CreateRenderTargetView(backBufferTexture.Get(), nullptr, myBackBuffer->myRTV.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create Render Target View for the back buffer");
		return false;
	}

	RECT clientRect = {};
	GetClientRect(aWindowHandle, &clientRect);
	//const float clientWidth = static_cast<float>(clientRect.right - clientRect.left);
	//const float clientHeight = static_cast<float>(clientRect.bottom - clientRect.top);
	const float clientWidth = static_cast<float>(aResolutionWidth);
	const float clientHeight = static_cast<float>(aResolutionHeight);
	//myBackBuffer->myViewPort = { 0, 0, static_cast<float>(aResolutionWidth), static_cast<float>(aResolutionHeight), 0, 1 };
	myBackBuffer->myViewPort = { 0, 0, clientWidth, clientHeight, 0, 1 };

	myDepthBuffer = std::make_shared<Texture>();

	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = static_cast<unsigned>(clientWidth);
	depthDesc.Height = static_cast<unsigned>(clientHeight);
	depthDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;

	ComPtr<ID3D11Texture2D> depthTexture;
	result = device->CreateTexture2D(&depthDesc, nullptr, depthTexture.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create depth buffer");
		return false;
	}

	SetObjectName(depthTexture, "DepthBuffer_T2D");

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	result = device->CreateDepthStencilView(depthTexture.Get(), &dsvDesc, myDepthBuffer->myDSV.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create depth stencil view for depth buffer");
		return false;
	}

	SetObjectName(myDepthBuffer->myDSV, "DepthBuffer_DSV");

	myDevice = device;
	myContext = context;
	mySwapChain = swapChain;

	myRasterizerStates[RS_Default] = nullptr;

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = false;
	result = myDevice->CreateRasterizerState(&rasterizerDesc, myRasterizerStates[RS_CullNone].GetAddressOf());
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create default rasterizer state");
		return false;
	}

	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	myDevice->CreateRasterizerState(&rasterizerDesc, myRasterizerStates[RS_Wireframe].GetAddressOf());

	myDepthStates[DS_Default] = nullptr;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = false;
	result = myDevice->CreateDepthStencilState(&depthStencilDesc, myDepthStates[DS_LessEqual].GetAddressOf());
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create default depth stencil state");
		return false;
	}

	myDepthStates[DS_ReadOnly] = nullptr;
	D3D11_DEPTH_STENCIL_DESC readOnlyDepth = CD3D11_DEPTH_STENCIL_DESC();
	readOnlyDepth.DepthEnable = true;
	readOnlyDepth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	readOnlyDepth.DepthFunc = D3D11_COMPARISON_LESS;
	readOnlyDepth.StencilEnable = false;
	result = myDevice->CreateDepthStencilState(&readOnlyDepth, myDepthStates[DS_ReadOnly].GetAddressOf());
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create read-only depth stencil state");
		return false;
	}

	myContext->QueryInterface(IID_PPV_ARGS(&myUDA));

	SetObjectName(myContext, "Device_Context");
	SetObjectName(myBackBuffer->myRTV, "BackBuffer_RTV");

	LOG(RhiLog, Log, "RHI Initialized");

	return true;
}

void RenderHardwareInterface::Present() const
{
	mySwapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
}

bool RenderHardwareInterface::CreateIndexBuffer(std::string_view aName, const std::vector<unsigned>& aIndexList,
	Microsoft::WRL::ComPtr<ID3D11Buffer>& outIxBuffer)
{
	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.ByteWidth = static_cast<unsigned>(aIndexList.size() * sizeof(unsigned));
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexSubresourceData = {};
	indexSubresourceData.pSysMem = aIndexList.data();

	const HRESULT result = myDevice->CreateBuffer(&indexBufferDesc, &indexSubresourceData, outIxBuffer.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create index buffer - {}.", aName);
		return false;
	}

	SetObjectName(outIxBuffer, aName);

	return true;
}

bool RenderHardwareInterface::CreateConstantBuffer(std::string_view aName, size_t aSize, unsigned aSlot, unsigned aPipelineStages, ConstantBuffer& outBuffer)
{
	if (aSize > 65536)
	{
		LOG(RhiLog, Error, "Failed to create constant buffer {}. Size is larger than 64Kb ({}).", aName, aSize);
		return false;
	}

	outBuffer.myName = aName;
	outBuffer.mySize = aSize;
	outBuffer.myPipelineStages = aPipelineStages;
	outBuffer.mySlotIndex = aSlot;

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.ByteWidth = static_cast <unsigned>(aSize);

	const HRESULT result = myDevice->CreateBuffer(&bufferDesc, nullptr, outBuffer.myBuffer.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create constant buffer {}. Check DirectX log for more information", aName);
		return false;
	}

	SetObjectName(outBuffer.myBuffer, aName);
	LOG(RhiLog, Log, "Created constant buffer {}.", aName);

	return true;
}

void RenderHardwareInterface::SetVertexBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aVertexBuffer, size_t aVertexSize, size_t aVertexOffset) const
{
	const unsigned vertexSize = static_cast<unsigned>(aVertexSize);
	const unsigned vertexOffset = static_cast<unsigned>(aVertexOffset);
	myContext->IASetVertexBuffers(0, 1, aVertexBuffer.GetAddressOf(), &vertexSize, &vertexOffset);
}

void RenderHardwareInterface::SetIndexBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aIndexBuffer) const
{
	DXGI_FORMAT indexBufferFormat = DXGI_FORMAT_R32_UINT;
	myContext->IASetIndexBuffer(aIndexBuffer.Get(), indexBufferFormat, 0);
}

void RenderHardwareInterface::SetConstantBuffer(const ConstantBuffer& aBuffer)
{
	if (aBuffer.myPipelineStages & PIPELINE_STAGE_VERTEX_SHADER)
	{
		myContext->VSSetConstantBuffers(aBuffer.mySlotIndex, 1, aBuffer.myBuffer.GetAddressOf());
	}
	if (aBuffer.myPipelineStages & PIPELINE_STAGE_PIXEL_SHADER)
	{
		myContext->PSSetConstantBuffers(aBuffer.mySlotIndex, 1, aBuffer.myBuffer.GetAddressOf());
	}
	if (aBuffer.myPipelineStages & PIPELINE_STAGE_GEOMETRY_SHADER)
	{
		myContext->GSSetConstantBuffers(aBuffer.mySlotIndex, 1, aBuffer.myBuffer.GetAddressOf());
	}
}

void RenderHardwareInterface::UpdateVertexBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aVertexBuffer, const uint8_t* aVertexDataPointer, size_t aVertices, size_t aVertexSize) const
{
	D3D11_MAPPED_SUBRESOURCE bufferData = {};

	HRESULT result = myContext->Map(aVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);

	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to map cbuffer");
		return;
	}

	memcpy_s(bufferData.pData, aVertices * aVertexSize, aVertexDataPointer, aVertices * aVertexSize);

	myContext->Unmap(aVertexBuffer.Get(), 0);

	return;
}

void RenderHardwareInterface::SetPrimitiveTopology(unsigned aTopology) const
{
	myContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(aTopology));
}

bool RenderHardwareInterface::CreateInputLayout(Microsoft::WRL::ComPtr<ID3D11InputLayout>& outInputLayout,
	const std::vector<VertexElementDesc>& aInputLayoutDefinition, const uint8_t* aShaderDataPtr, size_t aShaderDataSize)
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
	inputElements.reserve(aInputLayoutDefinition.size());

	for (const auto& vxED : aInputLayoutDefinition)
	{
		D3D11_INPUT_ELEMENT_DESC element = {};
		element.SemanticName = vxED.Semantic.data();
		element.SemanticIndex = vxED.SemanticIndex;
		element.Format = static_cast<DXGI_FORMAT>(vxED.Type);

		element.InputSlot = 0;
		element.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		element.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		element.InstanceDataStepRate = 0;

		inputElements.emplace_back(element);
	}

	const HRESULT result = myDevice->CreateInputLayout(
		inputElements.data(),
		static_cast<unsigned>(inputElements.size()),
		aShaderDataPtr,
		aShaderDataSize,
		outInputLayout.GetAddressOf()
	);
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create input layout");
		return false;
	}

	SetObjectName(outInputLayout, "Default Input Layout");

	return true;
}

void RenderHardwareInterface::SetInputLayout(const Microsoft::WRL::ComPtr<ID3D11InputLayout>& aInputLayout)
{
	myContext->IASetInputLayout(aInputLayout.Get());
}

bool RenderHardwareInterface::LoadShaderFromMemory(std::string_view aName, Shader& outShader,
	const uint8_t* aShaderDataPtr, size_t aShaderDataSize)
{
	ComPtr<ID3D11ShaderReflection> shaderReflection;
	HRESULT result = D3DReflect(
		aShaderDataPtr,
		aShaderDataSize,
		IID_ID3D11ShaderReflection,
		reinterpret_cast<void**>(shaderReflection.GetAddressOf())
	);
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to load shader {}", aName);
		return false;
	}

	D3D11_SHADER_DESC shaderDesc = {};
	shaderReflection->GetDesc(&shaderDesc);

	D3D11_SHADER_VERSION_TYPE shaderVersion = static_cast<D3D11_SHADER_VERSION_TYPE>(D3D11_SHVER_GET_TYPE(shaderDesc.Version));
	switch (shaderVersion)
	{
	case D3D11_SHVER_VERTEX_SHADER:
	{
		outShader.type = ShaderType::VertexShader;
		ComPtr<ID3D11VertexShader> vsShader;
		result = myDevice->CreateVertexShader(aShaderDataPtr, aShaderDataSize, nullptr, &vsShader);
		if (FAILED(result))
		{
			LOG(RhiLog, Error, "Failed to create vertex shader {}", aName);
			return false;
		}
		outShader.shader = vsShader;
		break;
	}
	case D3D11_SHVER_PIXEL_SHADER:
	{
		outShader.type = ShaderType::PixelShader;
		ComPtr<ID3D11PixelShader> psShader;
		result = myDevice->CreatePixelShader(aShaderDataPtr, aShaderDataSize, nullptr, &psShader);
		if (FAILED(result))
		{
			LOG(RhiLog, Error, "Failed to create pixel shader {}", aName);
			return false;
		}
		outShader.shader = psShader;
		break;
	}
	case D3D11_SHVER_GEOMETRY_SHADER:
	{
		outShader.type = ShaderType::GeometryShader;
		ComPtr<ID3D11GeometryShader> gsShader;
		result = myDevice->CreateGeometryShader(aShaderDataPtr, aShaderDataSize, nullptr, &gsShader);
		if (FAILED(result))
		{
			LOG(RhiLog, Error, "Failed to create geometry shader {}", aName);
			return false;
		}
		outShader.shader = gsShader;
		break;
	}
	}

	SetObjectName(outShader.shader, aName);

	return true;
}

void RenderHardwareInterface::Draw(unsigned aVertexCount) const
{
	myContext->Draw(aVertexCount, 0);
}

void RenderHardwareInterface::DrawIndexed(unsigned aStartIndex, unsigned aIndexCount) const
{
	myContext->DrawIndexed(aIndexCount, aStartIndex, 0);
}

void RenderHardwareInterface::ChangePipelineState(const PipelineStateObject& aNewPSO,
	const PipelineStateObject& aOldPSO) const
{
	for (int i = 0; i < aOldPSO.UsedRenderTargets; i++)
	{
		if (aOldPSO.RenderTarget[i])
		{
			ID3D11RenderTargetView* nullRTV = nullptr;
			myContext->OMSetRenderTargets(1, &nullRTV, nullptr);
		}
		else
		{
			if (aOldPSO.DepthStencil)
			{
				myContext->OMSetRenderTargets(0, nullptr, nullptr);
				break;
			}
		}
	}

	const std::array<float, 4> blendFactor = { 0, 0, 0, 0 };
	constexpr unsigned samplerMask = 0xffffffff;
	myContext->OMSetBlendState(aNewPSO.BlendState.Get(), blendFactor.data(), samplerMask);
	myContext->RSSetState(aNewPSO.RasterizerState.Get());
	myContext->OMSetDepthStencilState(aNewPSO.DepthStencilState.Get(), 0);

	for (const auto& [slot, sampler] : aNewPSO.SamplerStates)
	{
		myContext->VSSetSamplers(slot, 1, sampler.GetAddressOf());
		myContext->PSSetSamplers(slot, 1, sampler.GetAddressOf());
	}

	constexpr int size = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;
	ID3D11RenderTargetView* renderViews[size] = {};

	for (int i = 0; i < aNewPSO.UsedRenderTargets; i++)
	{
		if (aNewPSO.ClearRenderTarget[i])
		{
			myContext->ClearRenderTargetView(aNewPSO.RenderTarget[i]->myRTV.Get(), aNewPSO.RenderTarget[i]->myClearColor.data());
		}
	}

	if (aNewPSO.ClearDepthStencil)
	{
		if (aNewPSO.DepthStencil)
		{
			myContext->ClearDepthStencilView(aNewPSO.DepthStencil->myDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		}
	}

	for (int i = 0; i < aNewPSO.UsedRenderTargets; i++)
	{
		if (aNewPSO.RenderTarget[i])
		{
			renderViews[i] = *aNewPSO.RenderTarget[i]->myRTV.GetAddressOf();
		}
	}

	D3D11_VIEWPORT viewports[size] = {};
	for (int i = 0; i < aNewPSO.UsedRenderTargets; i++)
	{
		D3D11_VIEWPORT viewport = {};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = aNewPSO.RenderTarget[i]->GetSize().x;
		viewport.Height = aNewPSO.RenderTarget[i]->GetSize().y;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1;
		viewports[i] = viewport;
	}

	ID3D11DepthStencilView* nullDSV[] = { nullptr };
	ID3D11RenderTargetView* nullRTV = nullptr;

	if (aNewPSO.UsedRenderTargets > 0)
	{
		if (aNewPSO.Type == PSOType::Sprite) //förlåt mig
		{
			myContext->OMSetRenderTargets(aNewPSO.UsedRenderTargets, renderViews, *nullDSV);
		}
		else 
		if (aNewPSO.DepthStencil)
		{
			myContext->OMSetRenderTargets(aNewPSO.UsedRenderTargets, renderViews, aNewPSO.DepthStencil->myDSV.Get());
		}
		else
		{
			myContext->OMSetRenderTargets(aNewPSO.UsedRenderTargets, renderViews, nullptr);
		}
		myContext->RSSetViewports(aNewPSO.UsedRenderTargets, viewports);
	}
	else
	{
		
		if (aNewPSO.DepthStencil != nullptr)
		{
			myContext->OMSetRenderTargets(1, &nullRTV, aNewPSO.DepthStencil->myDSV.Get());
		}
		else
		{
			myContext->OMSetRenderTargets(1, &nullRTV, *nullDSV);
		}

		D3D11_VIEWPORT viewport = {};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;
		if (aNewPSO.DepthStencil != nullptr)
		{
			viewport.Width = aNewPSO.DepthStencil->myViewPort.Width;
			viewport.Height = aNewPSO.DepthStencil->myViewPort.Height;
		}
		else
		{
			viewport.Width = myBackBuffer->myViewPort.Width;
			viewport.Height = myBackBuffer->myViewPort.Height;
		}

		myContext->RSSetViewports(1, &viewport);
	}


}

bool RenderHardwareInterface::CreateSamplerState(std::string_view aName, const D3D11_SAMPLER_DESC& aSamplerDesc)
{
	ComPtr<ID3D11SamplerState> samplerState = {};
	const HRESULT result = myDevice->CreateSamplerState(&aSamplerDesc, &samplerState);
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create sampler state {}", aName);
		return false;
	}
	SetObjectName(samplerState, aName);
	std::string mapKey(aName);
	mySamplerStates.emplace(mapKey, samplerState);

	return true;
}

const Microsoft::WRL::ComPtr<ID3D11SamplerState>& RenderHardwareInterface::GetSamplerState(
	const std::string& aName) const
{
	return mySamplerStates.at(aName);
}

bool RenderHardwareInterface::CreateBlendState(std::string_view aName, const D3D11_BLEND_DESC& aBlendDesc)
{
	ComPtr<ID3D11BlendState> blendState = {};
	const HRESULT result = myDevice->CreateBlendState(&aBlendDesc, &blendState);
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create blend state {}", aName);
		return false;
	}
	SetObjectName(blendState, aName);
	std::string mapKey(aName);
	myBlendStates.emplace(mapKey, blendState);

	return true;
}

const Microsoft::WRL::ComPtr<ID3D11BlendState>& RenderHardwareInterface::GetBlendState(const std::string& aName) const
{
	return myBlendStates.at(aName);
}

bool RenderHardwareInterface::LoadTexture(std::string_view aName, const uint8_t* aTextureDataPtr,
	size_t aTextureDataSize, Texture& outTexture) const
{
	ComPtr<ID3D11Resource> resource;
	const HRESULT result = DirectX::CreateDDSTextureFromMemory(myDevice.Get(), aTextureDataPtr,
		aTextureDataSize, &resource, &outTexture.mySRV);

	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to load texture {}", aName);
	}

	D3D11_RESOURCE_DIMENSION resourceDimension = {};
	resource->GetType(&resourceDimension);
	switch (resourceDimension)
	{
	case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
	{
		ComPtr<ID3D11Texture2D> texture;
		resource.As(&texture);

		D3D11_TEXTURE2D_DESC desc = {};
		texture->GetDesc(&desc);
		outTexture.myProperties.myBindFlags = desc.BindFlags;
		outTexture.myProperties.myCPUAccessFlags = desc.CPUAccessFlags;
		outTexture.myProperties.myUsage = desc.Usage;
		outTexture.myProperties.mySize = desc.ArraySize;
		outTexture.SetSize({ static_cast<float>(desc.Width), static_cast<float>(desc.Height) });
	}
	break;

	default:
		LOG(RhiLog, Error, "Wrong dimension format on texture {}", aName);
		break;
	}

	SetObjectName(resource, aName);
	std::string srvName(aName);
	srvName.append("_SRV");
	SetObjectName(outTexture.mySRV, srvName);

	return true;
}

bool RenderHardwareInterface::SetTextureResource(unsigned aPipelineStages, unsigned aSlot,
	const Texture& aTexture) const
{
	if (!(aTexture.myProperties.myBindFlags & D3D11_BIND_SHADER_RESOURCE))
	{
		LOG(RhiLog, Error, "Couldn't set texture resource on slot {}", aSlot);
		return false;
	}

	if (aPipelineStages & PIPELINE_STAGE_VERTEX_SHADER)
	{
		myContext->VSSetShaderResources(aSlot, 1, aTexture.mySRV.GetAddressOf());
	}
	if (aPipelineStages & PIPELINE_STAGE_PIXEL_SHADER)
	{
		myContext->PSSetShaderResources(aSlot, 1, aTexture.mySRV.GetAddressOf());
	}

	return true;
}

bool RenderHardwareInterface::ClearTextureResourceSlot(unsigned aPipelineStages, unsigned aSlot) const
{
	ID3D11ShaderResourceView* dummy[] = { nullptr };
	if (aPipelineStages & PIPELINE_STAGE_VERTEX_SHADER)
	{
		myContext->VSSetShaderResources(aSlot, 1, dummy);
	}
	if (aPipelineStages & PIPELINE_STAGE_PIXEL_SHADER)
	{
		myContext->PSSetShaderResources(aSlot, 1, dummy);
	}

	return true;
}

bool RenderHardwareInterface::CreateShadowMap(const std::string& aName, CommonUtilities::Vector2<unsigned> aSize,
	Texture& outTexture) const
{
	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = aSize.x;
	depthDesc.Height = aSize.y;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;

	HRESULT result;
	ComPtr<ID3D11Texture2D> depthTexture = {};
	result = myDevice->CreateTexture2D(&depthDesc, nullptr, depthTexture.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Couldn't create 2D texture in ShadowMap {}", aName);
		return false;
	}

	std::string baseName(aName);
	SetObjectName(depthTexture, baseName);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	result = myDevice->CreateDepthStencilView(depthTexture.Get(), &dsvDesc, outTexture.myDSV.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Couldn't create depth stencil view in ShadowMap {}", aName);
		return false;
	}
	SetObjectName(outTexture.myDSV, baseName + "_DSV");

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = depthDesc.MipLevels;

	result = myDevice->CreateShaderResourceView(depthTexture.Get(), &srvDesc, &outTexture.mySRV);
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Couldn't create shader resource view in ShadowMap {}", aName);
		return false;
	}
	SetObjectName(outTexture.mySRV, baseName + "_SRV");

	outTexture.myProperties.myCPUAccessFlags = depthDesc.CPUAccessFlags;
	outTexture.myProperties.myUsage = depthDesc.Usage;
	outTexture.myProperties.mySize = depthDesc.ArraySize;
	outTexture.myProperties.myBindFlags = depthDesc.BindFlags;
	outTexture.myViewPort = { 0.f, 0.f, static_cast<float>(aSize.x), static_cast<float>(aSize.y), 1.f, 1.f };
	return true;
}

void RenderHardwareInterface::BeginEvent(const std::string_view& aEvent) const
{
	const std::wstring eventW = str::utf8_to_wide(aEvent.data());
	myUDA->BeginEvent(eventW.c_str());
}

void RenderHardwareInterface::EndEvent() const
{
	myUDA->EndEvent();
}

void RenderHardwareInterface::SetMarker(const std::string_view& aMarker) const
{
	const std::wstring markerW = str::utf8_to_wide(aMarker.data());
	myUDA->SetMarker(markerW.c_str());
}

void RenderHardwareInterface::ClearRenderTargets(const int& aNumViews) const
{
	myContext->OMSetRenderTargets(aNumViews - 1, nullptr, nullptr);
}

void RenderHardwareInterface::ClearBackBuffer() const
{
	FLOAT clearColor[4] = { 0, 0, 0, 0 };
	myContext->ClearRenderTargetView(myBackBuffer->GetRTV().Get(), clearColor);
}

void RenderHardwareInterface::SetRasterizerState(const RasterizerState aState)
{
	aState;
}

void RenderHardwareInterface::SetDepthState(const DepthState aState)
{
	myContext->OMSetDepthStencilState(myDepthStates[aState].Get(), 0);
}

const Microsoft::WRL::ComPtr<ID3D11DepthStencilState>& RenderHardwareInterface::GetDepthStencilState(DepthState aDepthState) const
{
	return myDepthStates[aDepthState];
}

void RenderHardwareInterface::SetRenderTarget(std::shared_ptr<Texture> aRenderTarget) const
{
	myContext->OMSetRenderTargets(1, aRenderTarget->GetRTV().GetAddressOf(), aRenderTarget->GetDSV().Get());
}

CU::Vector2f RenderHardwareInterface::GetViewPortSize() const
{
	return myBackBuffer->GetSize();
}

void RenderHardwareInterface::SetPixelShader(std::shared_ptr<Shader> aPixelShader)
{
	ComPtr<ID3D11PixelShader> psShader = nullptr;
	if (aPixelShader)
	{
		aPixelShader->shader.As(&psShader);
		myContext->PSSetShader(psShader.Get(), nullptr, 0);
	}
	else
	{
		myContext->PSSetShader(nullptr, nullptr, 0);
	}
}

void RenderHardwareInterface::SetVertexShader(std::shared_ptr<Shader> aVertexShader)
{
	ComPtr<ID3D11VertexShader> vsShader;
	aVertexShader->shader.As(&vsShader);
	if (aVertexShader)
	{
		aVertexShader->shader.As(&vsShader);
	}
	myContext->VSSetShader(vsShader.Get(), nullptr, 0);

}

void RenderHardwareInterface::SetGeometryShader(std::shared_ptr<Shader> aGeometryShader)
{
	ComPtr<ID3D11GeometryShader> gsShader = nullptr;
	if (aGeometryShader)
	{
		aGeometryShader->shader.As(&gsShader);
		myContext->GSSetShader(gsShader.Get(), nullptr, 0);
	}
	else
	{
		myContext->GSSetShader(nullptr, nullptr, 0);
	}
}

void RenderHardwareInterface::SetInternalResolution(float aWidth, float aHeight)
{
	HRESULT result = E_FAIL;

	ID3D11RenderTargetView* nullViews[] = { nullptr };
	myContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
	myBackBuffer->GetRTV()->Release();
	myDepthBuffer->GetDSV()->Release();
	myContext->Flush();

	result = mySwapChain->ResizeBuffers(2, static_cast<unsigned>(aWidth), static_cast<unsigned>(aHeight), DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to resize swapchain buffer");
		return;
	}


	ComPtr<ID3D11Texture2D> backBufferTexture;
	result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBufferTexture);
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to get backbuffer");
		return;
	}

	//myBackBuffer = std::make_shared<Texture>();
	result = myDevice->CreateRenderTargetView(backBufferTexture.Get(), nullptr, myBackBuffer->myRTV.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create render target view for backbuffer");
		return;
	}

	myBackBuffer->myViewPort = { 0, 0, aWidth, aHeight, 0, 1 };

	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = static_cast<unsigned>(aWidth);
	depthDesc.Height = static_cast<unsigned>(aHeight);
	depthDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;

	ComPtr<ID3D11Texture2D> depthTexture;
	result = myDevice->CreateTexture2D(&depthDesc, nullptr, depthTexture.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create depth buffer texture");
		return;
	}

	SetObjectName(depthTexture, "DepthBuffer_T2D");

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	result = myDevice->CreateDepthStencilView(depthTexture.Get(), &dsvDesc, myDepthBuffer->myDSV.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed create dsv");
		return;
	}

	SetObjectName(myDepthBuffer->myDSV, "DepthBuffer_DSV");
	myBackBuffer;
	
}

bool RenderHardwareInterface::CreateVertexBufferInterval(std::string_view aName, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer,
                                                         const uint8_t* aVertexDataPointer, size_t aNumVertices, size_t aVertexSize, bool aDynamic) const
{
	D3D11_BUFFER_DESC vxBufferDesc = {};
	vxBufferDesc.ByteWidth = static_cast<unsigned>(aNumVertices * aVertexSize);
	vxBufferDesc.Usage = aDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
	vxBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	if (aDynamic)
	{
		vxBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}

	D3D11_SUBRESOURCE_DATA vxResource = {};
	vxResource.pSysMem = aVertexDataPointer;

	const HRESULT result = myDevice->CreateBuffer(&vxBufferDesc, &vxResource, outVxBuffer.GetAddressOf());
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create vertex buffer {}", aName);
		return false;
	}

	SetObjectName(outVxBuffer, aName);

	return true;
}

bool RenderHardwareInterface::UpdateConstantBufferInternal(const ConstantBuffer& aBuffer, const void* aBufferData, size_t aBufferDataSize) const
{
	if (!aBuffer.myBuffer)
	{
		LOG(RhiLog, Error, "Failed to update constant buffer. Buffer {} is invalid.", aBuffer.myName);
		return false;
	}

	if (aBufferDataSize > aBuffer.mySize)
	{
		LOG(RhiLog, Error, "Failed to update constant buffer {}. Data too large.", aBuffer.myName);
		return false;
	}

	D3D11_MAPPED_SUBRESOURCE bufferData = {};

	HRESULT result = myContext->Map(aBuffer.myBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to map constant buffer {}.", aBuffer.myName);
		return false;
	}

	memcpy_s(bufferData.pData, aBuffer.mySize, aBufferData, aBufferDataSize);
	myContext->Unmap(aBuffer.myBuffer.Get(), 0);

	return true;
}

void RenderHardwareInterface::SetObjectName(Microsoft::WRL::ComPtr<ID3D11DeviceChild> aObject, std::string_view aName) const
{
	if (aObject)
	{
		aObject->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<unsigned>(sizeof(char) * aName.size()), aName.data());
	}
}


// These come from dxgiformat.h
// You can ctrl+click on the words DXGI_FORMAT to open
// that file and look for values when adding
// new entries to this enum


bool RenderHardwareInterface::CreateTexture(const std::string& aName, const unsigned& aWidth, const unsigned& aHeight,
                                            const RHITextureFormat& aFormat, Texture* outTexture, const bool& aStaging, const bool& aShaderResource, const bool& aRenderTarget,
                                            const bool& aCpuAccessRead, const bool& aCpuAccessWrite) const
{
	outTexture->SetSize({static_cast<float>(aWidth), static_cast<float>(aHeight)});

	// If this texture is a Staging texture or not.
	// Staging textures cannot be rendered to but are useful in i.e. screen picking.
	/*const */unsigned usage = aStaging ? D3D11_USAGE_STAGING : D3D11_USAGE_DEFAULT;

	if (aName == "VideoTexture")
	{
		usage = D3D11_USAGE_DYNAMIC;
	}

	// Bind flags for this texture.
	// I.e. if it's used as a Resource (read)
	// or a Target (write)
	unsigned bindFlags = {};
	if (aShaderResource)
	{
		bindFlags |= D3D11_BIND_SHADER_RESOURCE;
	}

	if (aRenderTarget)
	{
		bindFlags |= D3D11_BIND_RENDER_TARGET;
	}

	// CPU flags letting us read and/or write
	// from/to a texture. Imposes several restrictions.
	// Primary use in TGP is if you implement picking.
	unsigned cpuFlags = {};
	if (aCpuAccessRead)
	{
		cpuFlags |= D3D11_CPU_ACCESS_READ;
	}
	if (aCpuAccessWrite)
	{
		cpuFlags |= D3D11_CPU_ACCESS_WRITE;
	}

	outTexture->myProperties.myBindFlags = bindFlags;
	outTexture->myProperties.myCPUAccessFlags = cpuFlags;
	outTexture->myProperties.myUsage = static_cast<D3D11_USAGE>(usage);

	HRESULT result = E_FAIL;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = aWidth;
	desc.Height = aHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = static_cast<DXGI_FORMAT>(aFormat);
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = static_cast<D3D11_USAGE>(usage);
	desc.BindFlags = bindFlags;
	desc.CPUAccessFlags = cpuFlags;
	desc.MiscFlags = 0;

	ComPtr<ID3D11Texture2D> texture;
	result = myDevice->CreateTexture2D(&desc, nullptr, reinterpret_cast<ID3D11Texture2D**>(texture.GetAddressOf()));
	if (FAILED(result))
	{
		LOG(RhiLog, Error, "Failed to create the texture {}!", aName);
		return false;
	}

	SetObjectName(texture, aName);

	if (aShaderResource)
	{
		result = myDevice->CreateShaderResourceView(texture.Get(), nullptr, &outTexture->mySRV);
		if (FAILED(result))
		{
			LOG(RhiLog, Error, "Failed to create the texture {}! Failed to create a shader resource view!", aName);
			return false;
		}

		SetObjectName(outTexture->mySRV, aName + "_SRV");
	}

	if (aRenderTarget)
	{
		result = myDevice->CreateRenderTargetView(texture.Get(), nullptr, &outTexture->myRTV);
		if (FAILED(result))
		{
			LOG(RhiLog, Error, "Failed to create the texture {}! Failed to create a render target view!", aName);
			return false;
		}

		SetObjectName(outTexture->myRTV, aName + "_RTV");
	}

	return true;
}

bool RenderHardwareInterface::CreateLUTTexture(Texture& aTexture, const std::shared_ptr<Shader>& aBrdfPixelShader, const std::shared_ptr<Shader>& aQuadVertexShader)
{
	BeginEvent("LUT");
	if (!CreateTexture("LUT", 512, 512, RHITextureFormat::R16G16_Float, &aTexture, false, true, true, false, false))
	{
		LOG(RhiLog, Error, "Failed to create LUT");
		return false;
	}
	myContext->OMSetRenderTargets(1, aTexture.myRTV.GetAddressOf(), nullptr);
	D3D11_SAMPLER_DESC lutSamplerDesc = {};
	lutSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	lutSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	lutSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	lutSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	lutSamplerDesc.MipLODBias = 0.0f;
	lutSamplerDesc.MaxAnisotropy = 1;
	lutSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	lutSamplerDesc.BorderColor[0] = 0.0f;
	lutSamplerDesc.BorderColor[1] = 0.0f;
	lutSamplerDesc.BorderColor[2] = 0.0f;
	lutSamplerDesc.BorderColor[3] = 0.0f;
	lutSamplerDesc.MinLOD = 0;
	lutSamplerDesc.MaxLOD = 0;

	if (!CreateSamplerState("LUTSampler", lutSamplerDesc)) {
		LOG(RhiLog, Error, "Failed to create lut sampler");
	}
	myContext->VSSetSamplers(14, 1, mySamplerStates.at("LUTSampler").GetAddressOf());
	myContext->PSSetSamplers(14, 1, mySamplerStates.at("LUTSampler").GetAddressOf());

#include "CompiledShaderHeaders\brdfLUT_PS.h"

	Shader psShader = *aBrdfPixelShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> d3d11PsShader;
	psShader.shader.As(&d3d11PsShader);
	myContext->PSSetShader(d3d11PsShader.Get(), nullptr, 0);

#include "CompiledShaderHeaders\QUAD_VS.h"

	Shader vsShader = *aQuadVertexShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> d3d11VsShader;
	vsShader.shader.As(&d3d11VsShader);
	myContext->VSSetShader(d3d11VsShader.Get(), nullptr, 0);

	CU::Vector2f size = { 512, 512 };
	DrawQuad(size);

	ClearRenderTargets();
	SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 127, aTexture);
	EndEvent();
	return true;
}

void RenderHardwareInterface::DrawQuad(const CU::Vector2f& aSize)
{
	SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	SetVertexBuffer(nullptr, 0, 0);
	SetIndexBuffer(nullptr);
	SetInputLayout(nullptr);

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = aSize.x > 0 ? aSize.x : myBackBuffer->GetSize().x;
	viewport.Height = aSize.y > 0 ? aSize.y : myBackBuffer->GetSize().y;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	myContext->RSSetViewports(1, &viewport);

	Draw(4);
}
