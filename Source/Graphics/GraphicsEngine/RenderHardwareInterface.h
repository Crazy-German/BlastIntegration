#pragma once
#include <wrl.h>

#include "RHIStructs.h"
#include "Texture.h"

#include "ViewPort.h"

struct PipelineStateObject;
class ConstantBuffer;
class Texture;
struct Shader;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11Buffer;
struct ID3D11InputLayout;
struct ID3DUserDefinedAnnotation;
struct ID3D11DeviceChild;
struct D3D11_BLEND_DESC;

enum class RHITextureFormat : unsigned
{
	R32G32B32A32_Float = 2,
	R16G16B16A16_FLOAT = 10,
	R16G16B16A16_SNORM = 13,
	R32G8X24_Typeless = 19,
	D32_Float_S8X24_UINT = 20,
	R32_Float_X8X24_Typeless = 21,
	R8G8B8A8_UNORM = 28,
	R16G16_Float = 34,
	R32_Typeless = 39,
	D32_Float = 40,
	R32_Float = 41,
	R8_UNORM = 61,
};

enum RasterizerState
{
	RS_Default,
	RS_CullNone,
	RS_Wireframe,
	RS_COUNT
};

enum DepthState
{
	DS_Default,
	DS_LessEqual,
	DS_ReadOnly,
	DS_COUNT
};


class RenderHardwareInterface
{
public:
	RenderHardwareInterface();
	~RenderHardwareInterface();

	bool Initialize(HWND aWindowHandle, bool aEnableDebug, unsigned aResolutionWidth, unsigned aResolutionHeight);
	void Present() const;

	template<typename VertexType>
	bool CreateVertexBuffer(std::string_view aName, const std::vector<VertexType>& aVertexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer, bool aDynamic = false);
	bool CreateIndexBuffer(std::string_view aName, const std::vector<unsigned>& aIndexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& outIxBuffer);
	bool CreateConstantBuffer(std::string_view aName, size_t aSize, unsigned aSlot, unsigned aPipelineStages, ConstantBuffer& outBuffer);

	void SetVertexBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aVertexBuffer, size_t aVertexSize, size_t aVertexOffset = 0) const;
	void SetIndexBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aIndexBuffer) const;
	void SetConstantBuffer(const ConstantBuffer& aBuffer);


	template<typename BufferData>
	bool UpdateConstantBuffer(const ConstantBuffer& aBuffer, const BufferData& aBufferData);
	void UpdateVertexBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aVertexBuffer, const uint8_t* aVertexDataPointer, size_t aVertices, size_t aVertexSize) const;

	void SetPrimitiveTopology(unsigned aTopology) const;

	bool CreateInputLayout(Microsoft::WRL::ComPtr<ID3D11InputLayout>& outInputLayout, const std::vector<VertexElementDesc>& aInputLayoutDefinition, const uint8_t* aShaderDataPtr, size_t aShaderDataSize);
	void SetInputLayout(const Microsoft::WRL::ComPtr<ID3D11InputLayout>& aInputLayout);

	bool LoadShaderFromMemory(std::string_view aName, Shader& outShader, const uint8_t* aShaderDataPtr, size_t aShaderDataSize);

	void Draw(unsigned aVertexCount) const;
	void DrawIndexed(unsigned aStartIndex, unsigned aIndexCount) const;

	void ChangePipelineState(const PipelineStateObject& aNewPSO, const PipelineStateObject& aOldPSO) const;
	std::shared_ptr<Texture> GetBackBuffer() const { return myBackBuffer; }
	std::shared_ptr<Texture> GetDepthBuffer() const { return myDepthBuffer; }

	ViewPort GetViewPort() const { return myBackBuffer->myViewPort; };

	bool CreateSamplerState(std::string_view aName, const D3D11_SAMPLER_DESC& aSamplerDesc);
	const Microsoft::WRL::ComPtr<ID3D11SamplerState> &GetSamplerState(const std::string& aName) const;

	bool CreateBlendState(std::string_view aName, const D3D11_BLEND_DESC& aBlendDesc);
	const Microsoft::WRL::ComPtr<ID3D11BlendState>& GetBlendState(const std::string& aName) const;

	bool CreateTexture(const std::string& aName, const unsigned& aWidth, const unsigned& aHeight, const RHITextureFormat& aFormat, Texture*
	                   outTexture, const bool& aStaging, const bool& aShaderResource, const bool& aRenderTarget, const bool& aCpuAccessRead, const
	                   bool& aCpuAccessWrite) const;
	bool LoadTexture(std::string_view aName, const uint8_t* aTextureDataPtr, size_t aTextureDataSize, Texture& outTexture) const;
	bool SetTextureResource(unsigned aPipelineStages, unsigned aSlot, const Texture& aTexture) const;
	bool ClearTextureResourceSlot(unsigned aPipelineStages, unsigned aSlot) const;

	bool CreateShadowMap(const std::string& aName, CommonUtilities::Vector2<unsigned> aSize, Texture& outTexture) const;
	
	bool CreateLUTTexture(Texture& aTexture, const std::shared_ptr<Shader>& aBrdfPixelShader, const std::shared_ptr<Shader>& aQuadVertexShader);

	void DrawQuad(const CU::Vector2f& aSize);

	void BeginEvent(const std::string_view& aEvent) const;
	void EndEvent() const;
	void SetMarker(const std::string_view& aMarker) const;

	void ClearRenderTargets(const int& aNumViews = 1) const;
	void ClearBackBuffer() const;

	void SetRasterizerState(const RasterizerState aState);

	void SetDepthState(const DepthState aState);
	const Microsoft::WRL::ComPtr<ID3D11DepthStencilState>& GetDepthStencilState(DepthState aDepthState) const;


	void SetRenderTarget(std::shared_ptr<Texture> aRenderTarget) const;

	CU::Vector2f GetViewPortSize() const;

	Microsoft::WRL::ComPtr<ID3D11Device> GetDevice() { return myDevice; }
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetContext() { return myContext; }

	void SetPixelShader(std::shared_ptr<Shader>aVertexShader);
	void SetVertexShader(std::shared_ptr<Shader> aVertexShader);
	void SetGeometryShader(std::shared_ptr<Shader> aGeometryShader);

	void SetInternalResolution(float aWidth, float aHeight);
private:
	Microsoft::WRL::ComPtr<ID3D11Device> myDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> myContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mySwapChain;
	Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation> myUDA;

	std::shared_ptr<Texture> myBackBuffer;
	std::shared_ptr<Texture> myDepthBuffer;
	
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> mySamplerStates;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11BlendState>> myBlendStates;
	std::array<Microsoft::WRL::ComPtr<ID3D11RasterizerState>, RS_COUNT> myRasterizerStates;
	std::array<Microsoft::WRL::ComPtr<ID3D11DepthStencilState>, DS_COUNT> myDepthStates;

	bool CreateVertexBufferInterval(std::string_view aName, Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer, const uint8_t* aVertexDataPointer, size_t aNumVertices, size_t aVertexSize, bool aDynamic) const;
	bool UpdateConstantBufferInternal(const ConstantBuffer& aBuffer, const void* aBufferData, size_t aBufferDataSize) const;

	void SetObjectName(Microsoft::WRL::ComPtr<ID3D11DeviceChild> aObject, std::string_view aName) const;

};

template<typename VertexType>
inline bool RenderHardwareInterface::CreateVertexBuffer(std::string_view aName, const std::vector<VertexType>& aVertexList,
	Microsoft::WRL::ComPtr<ID3D11Buffer>& outVxBuffer, bool aDynamic)
{
	const size_t vxSize = sizeof(VertexType);
	const size_t vxCount = aVertexList.size();

	return CreateVertexBufferInterval(aName, outVxBuffer, reinterpret_cast<const uint8_t*>(aVertexList.data()), vxCount, vxSize, aDynamic);
}

template<typename BufferData>
inline bool RenderHardwareInterface::UpdateConstantBuffer(const ConstantBuffer& aBuffer, const BufferData& aBufferData)
{
	const size_t dataSize = sizeof(BufferData);
	return UpdateConstantBufferInternal(aBuffer, &aBufferData, dataSize);
}
