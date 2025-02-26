#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <wrl.h>
#include "../../Utilities/CommonUtilities/Vector2.hpp"

#include "ConstantBuffer.h"
#include "PipelineStateObject.h"
#include "RenderHardwareInterface.h"
#include "../../Utilities/Logger/Logger.h"
#include "ViewPort.h"

#ifdef _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(GraphicsEngineLog, "GraphicsEngine", Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(GraphicsEngineLog, "GraphicsEngine", Error);		
#endif

inline DEFINE_LOG_CATEGORY(GraphicsEngineLog);

#include "GBuffer.h"

enum class ShaderType : unsigned;
class Text;
class Material;
class Texture;
class Mesh;
class RenderHardwareInterface;
class Sprite2D;
class ShaderCompiler;
struct Shader;
struct VertexShader;
class DebugLineObject;
class ParticleEmitter;

#include "SpriteVertex.h"
#include "Vertex.h"

struct ID3D11InputLayout;

enum class EnemyMaps
{
	RenderTarget,
	CPUResource
};

enum class ConstantBufferType : unsigned
{
	ObjectBuffer,
	FrameBuffer,
	AnimationBuffer,
	MaterialBuffer,
	LightBuffer,
	MiscBuffer,
	TABuffer
#ifndef _RETAIL
	, DebugBuffer
#endif
};

class GraphicsEngine
{
public:

	static GraphicsEngine& Get();

	bool Initialize(HWND aWindowHandle, unsigned aResolutionWidth, unsigned aResolutionHeight);

	template<typename BufferData>
	bool UpdateAndSetConstantBuffer(ConstantBufferType aBufferType, const BufferData& aDataBlock);

	void ChangePipelineState(const PipelineStateObject& aNewPSO);
	void ChangeRenderTarget(const std::shared_ptr<Texture>& aRenderTarget) const;

	ViewPort GetViewPort();

	CU::Vector2<unsigned> GetDeviceSize() const;
	
	FORCEINLINE PipelineStateObject* GetForwardPSO()	 { return &myForwardPSO; }
	FORCEINLINE PipelineStateObject* GetDeferredPSO()	 { return &myDeferredPSO; }
	FORCEINLINE PipelineStateObject* GetShadowPSO()		 { return &myShadowPSO; }
	FORCEINLINE PipelineStateObject* GetLightPSO()		 { return &myLightPSO; }
	FORCEINLINE PipelineStateObject* GetSpritePSO()		 { return &mySpritePSO; }
	FORCEINLINE PipelineStateObject* GetLuminancePSO()	 { return &myLuminancePSO; }
	FORCEINLINE PipelineStateObject* GetTonemapPSO()	 { return &myTonemapPSO; }
	FORCEINLINE PipelineStateObject* GetTextPSO()		 { return &myTextPSO; }
	FORCEINLINE PipelineStateObject* GetVideoPSO()		 { return &myVideoPSO; }
	FORCEINLINE PipelineStateObject* GetSkyboxPSO()		 { return &mySkyboxPSO; }
	FORCEINLINE PipelineStateObject* GetResamplingPSO()	 { return &myResamplingPSO; }
	FORCEINLINE PipelineStateObject* GetGaussianHPSO()	 { return &myGaussianHPSO; }
	FORCEINLINE PipelineStateObject* GetGaussianVPSO()	 { return &myGaussianVPSO; }
	FORCEINLINE PipelineStateObject* GetBloomPSO()		 { return &myBloomPSO; }
	FORCEINLINE PipelineStateObject* GetHealthBarPSO()	 { return &myHealthBarPSO; }
	FORCEINLINE PipelineStateObject* GetSpiritBarPSO()	 { return &mySpiritBarPSO; }
	FORCEINLINE PipelineStateObject* GetCustomPSO()		 { return &myCustomPSO; }
	FORCEINLINE PipelineStateObject* GetCustomPPPSO()		 { return &myCustomPPPSO; }
	FORCEINLINE PipelineStateObject* GetParticlePSO()		 { return &myParticlePSO; }
	void RenderSkybox(const Mesh& aMesh) const;
	void RenderMesh(const Mesh& aMesh, const std::vector<std::shared_ptr<Material>>& someMaterials) const;
	void RenderDecal(const Texture& aTexture);
	void RenderSprite2D(const Sprite2D& aElement) const;
	void RenderText(const Text& aText);
	void RenderDebugLines(const DebugLineObject& aDebugLineObject);
	void RenderPoints(unsigned aPoints);

	Texture CreateTexture(const std::string& aName, const unsigned& aWidth, const unsigned& aHeight, const RHITextureFormat& aFormat, const bool&
	                       aStaging, const bool& aShaderResource, const bool& aRenderTarget, const bool& aCpuAccessRead, const bool&
	                       aCpuAccessWrite) const;
	bool LoadTexture(std::string_view aName, const uint8_t* aTextureDataPtr, const size_t& aTextureDataSize, Texture& outTexture) const;
	bool LoadTexture(const std::filesystem::path& aFilePath, Texture& outTexture) const;

	bool SetTextureResource(unsigned aPipelineStages, unsigned aSlot, const Texture& aTexture) const;
	bool SetCustomTextureResources(unsigned aPipeLineStages) const;
	bool ClearTextureResourceSlot(unsigned aPipelineStages, unsigned aSlot) const;

	void BeginEvent(std::string_view aEvent) const;
	void EndEvent() const;
	void SetMarker(std::string_view aMarker) const;

	void DrawQuad(const CU::Vector2f& aSize = {-1, -1});

	void ClearBackBuffer() const;

	std::shared_ptr<Texture> GetLuminanceBuffer() { return myLuminanceBuffer; }
	std::shared_ptr<Texture> GetIntermediateLDRBuffer() { return myIntermediateLDRBuffer; }
	std::shared_ptr<Texture> GetIntermediateHDRBuffer() { return myIntermediateHDRBuffer; }
	std::shared_ptr<Texture> GetHalfSizeScreenBuffer() { return myHalfSizeScreenBuffer; }
	std::shared_ptr<Texture> GetQuarterSizeScreenBuffer() { return myQuarterSizeScreenBuffer; }
	std::shared_ptr<Texture> GetEightSizeScreenBufferA() { return myEightSizeScreenBufferA; }
	std::shared_ptr<Texture> GetEightSizeScreenBufferB() { return myEightSizeScreenBufferB; }

	void Present() const { myRHI->Present(); }

	CU::Vector2f GetWindowSize();

	template<typename VertexType>
	void CreateVertexBuffer(std::string_view aName, const std::vector<VertexType>& aVertexList,
		Microsoft::WRL::ComPtr<ID3D11Buffer>& aOutVxBuffer, bool aDynamic = false);

	void CreateIndexBuffer(std::string_view aName, const std::vector<unsigned>& aIndexList,
		Microsoft::WRL::ComPtr<ID3D11Buffer>& aOutIxBuffer);

	Microsoft::WRL::ComPtr<ID3D11Device> GetDevice() { return myRHI->GetDevice(); }
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetContext() { return myRHI->GetContext(); }

	//video player thing?
	bool UpdateFromIYUV(const uint8_t* data, size_t data_size, Texture& aTexture);

	void SetPixelShader(const std::string_view& aShaderName);
	void SetVertexShader(const std::string_view& aShaderName);
	void SetGeometryShader(const std::string_view& aShaderName);

	void SetShader(unsigned aShaderID);

	std::shared_ptr<Shader> GetShader(const std::string& aShader);

	bool AddShader(const std::filesystem::path& aPath, ShaderType aShaderType);

	void SetInternalResolution(float aWidth, float aHeight);
	bool CompileShader(const std::filesystem::path& aPath);
	bool CompileAllCustomShaders();

	const std::vector<unsigned> GetTAPostProcessShaders() const;
	bool SetGBufferResource() const;

	bool SetCubemapLevel(int aLevel);

	void SetIndexBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aIndexBuffer) const;
	void SetVertexBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aVertexBuffer, size_t aVertexSize) const;

	void PrepareParticleEmitter(ParticleEmitter* aParticleEmitter);

private:


	GraphicsEngine();
	~GraphicsEngine();

	void CreateShadowMap(const std::string& aName);
	void CreateLUTTexture();

	void CreateResamplingTextures();

	bool CreateGBuffer();
	bool ClearGBuffer() const;

	bool CreateForwardPSO();
	bool CreateParticlePSO();
	bool CreateShadowPSO();
	bool CreateDeferredPSO();
	bool CreateLightPSO();
	bool CreateSpritePSO();
	bool CreateVideoPSO();
	bool CreateCustomPSO();
	bool CreateCustomPPPSO();

	bool CreateSkyboxPSO();
	bool CreateTonemapPSO();
	bool CreateResamplingPSO();
	bool CreateLuminancePSO();
	bool CreateGaussianHPSO();
	bool CreateGaussianVPSO();
	bool CreateTextPSO();
	bool CreateBloomPSO();

	//this should be. not here really for ideal decoupling
	bool CreateHealthBarPSO();
	bool CreateSpiritBarPSO();

	bool CreateAdditiveBlendState() const;

	void CreateShaders();

	void CreateDefaultShaders();
	void CreatePostProcessingShaders();
	void CreateUIShaders();
	void CreateDebugShaders();

	void LoadCustomShaders();

	void InitCustomTextures();


private:
	std::unique_ptr<RenderHardwareInterface> myRHI;
	std::unordered_map<ConstantBufferType, ConstantBuffer> myConstantBuffers;

	HWND myWindowHandle {};

	std::unique_ptr<ShaderCompiler> myShaderCompiler;

	//TODO: unordered map <PSOType, PSO>  (to send PSOType instead of pso in changePipelineState())
	PipelineStateObject myForwardPSO;
	PipelineStateObject myDeferredPSO;
	PipelineStateObject myCurrentPSO;
	PipelineStateObject myShadowPSO;
	PipelineStateObject myLightPSO;
	PipelineStateObject mySpritePSO;
	PipelineStateObject myTextPSO;
	PipelineStateObject myVideoPSO;
	PipelineStateObject mySkyboxPSO;
	PipelineStateObject myResamplingPSO;
	PipelineStateObject myLuminancePSO;
	PipelineStateObject myGaussianHPSO;
	PipelineStateObject myGaussianVPSO;
	PipelineStateObject myBloomPSO;
	PipelineStateObject myHealthBarPSO;
	PipelineStateObject mySpiritBarPSO;
	PipelineStateObject myTonemapPSO;
	PipelineStateObject myCustomPSO;
	PipelineStateObject myCustomPPPSO;
	PipelineStateObject myParticlePSO;

	std::vector<PipelineStateObject> myGarbagePSOs;

	GBuffer myGBuffer;
	std::shared_ptr<Texture> myDirlightShadowMap;
	std::shared_ptr<Texture> myLUTTexture;
	std::unordered_map<int, int> myTextureIndexToSlot;
	std::vector<std::shared_ptr<Texture>> myCustomTextures;

	std::shared_ptr<Texture> myLuminanceBuffer;
	std::shared_ptr<Texture> myHalfSizeScreenBuffer;
	std::shared_ptr<Texture> myQuarterSizeScreenBuffer;
	std::shared_ptr<Texture> myEightSizeScreenBufferA;
	std::shared_ptr<Texture> myEightSizeScreenBufferB;
	std::shared_ptr<Texture> myIntermediateLDRBuffer;
	std::shared_ptr<Texture> myIntermediateHDRBuffer;

	std::shared_ptr<Texture> myEnemyTexture;

	std::unordered_map<std::string_view, unsigned> myShaderMap;
	std::vector<std::string> myShaderNames;
	std::vector<std::shared_ptr<Shader>> myShaders;
	std::vector<unsigned> myTaPPShaders;
	std::shared_ptr<Shader> myCurrentPixelShader;
	std::shared_ptr<Shader> myCurrentGeometryShader;
	std::shared_ptr<VertexShader> myCurrentVertexShader;
};

template<typename BufferData>
inline bool GraphicsEngine::UpdateAndSetConstantBuffer(ConstantBufferType aBufferType, const BufferData& aDataBlock)
{
	if (!myConstantBuffers.contains(aBufferType))
	{
		LOG(GraphicsEngineLog, Error, "Constant buffers does not contain buffertype.");
		return false;
	}

	ConstantBuffer& buffer = myConstantBuffers.at(aBufferType);
	if (!myRHI->UpdateConstantBuffer(buffer, aDataBlock))
	{
		LOG(GraphicsEngineLog, Error, "Failed to update constant buffer.");
		return false;
	}

	myRHI->SetConstantBuffer(buffer);
	return true;
}

template<typename VertexType>
inline void GraphicsEngine::CreateVertexBuffer(std::string_view aName, const std::vector<VertexType>& aVertexList, Microsoft::WRL::ComPtr<ID3D11Buffer>& aOutVxBuffer, bool aDynamic)
{
	myRHI->CreateVertexBuffer(aName, aVertexList, aOutVxBuffer, aDynamic);
}
