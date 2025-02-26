#include "GraphicsEngine.pch.h"
#include "GraphicsEngine.h"

#include "RenderHardwareInterface.h"
#include "Shader.h"
#include "Vertex.h"
#include "FrameBuffer.h"
#include "ObjectBuffer.h"
#include "MiscBuffer.h"
#include "DebugBuffer.h"

#include <functional>

#include "AnimationBuffer.h"
#include "LightBuffer.h"
#include "Material.h"
#include "MaterialBuffer.h"
#include "Texture.h"
#include "Sprite2D.h"
#include "SpriteVertex.h"
#include <Text/TextFactory.h>

#include "InterOp\ShaderCompiler.h"
#include <DebugLineObject.h>

#include "../External/nlohmann/json.hpp"
#include <ParticleEmitter.h>

#define CUSTOM_SHADER_PATH "Assets\\VFX\\Shaders"


/*
 * GraphicsEngine handles rendering of a scene.
 * Handles culling using whatever cameras it's told to use.
 * Renders onto a user-provided render target and depth.
 */

GraphicsEngine& GraphicsEngine::Get()
{
	static GraphicsEngine myInstance;
	return myInstance;
}

bool GraphicsEngine::Initialize(HWND aWindowHandle, unsigned aResolutionWidth, unsigned aResolutionHeight)
{
	myWindowHandle = aWindowHandle;
	myRHI = std::make_unique<RenderHardwareInterface>();
#ifdef _DEBUG
	if (!myRHI->Initialize(aWindowHandle, true, aResolutionWidth, aResolutionHeight))
	{
		myRHI.reset();
		GELOG(Error, "Failed to initialize RHI");
		return false;
	}
#else
	if (!myRHI->Initialize(aWindowHandle, false, aResolutionWidth, aResolutionHeight))
	{
		myRHI.reset();
		return false;
	}
#endif

	myShaderNames.reserve(300);
	CreateShaders();

#ifndef _RETAIL
	CreateDebugShaders();
#endif

	CreateResamplingTextures();


	if (!CreateForwardPSO())
	{
		LOG(GraphicsEngineLog, Error, "Failed to create forward PSO");
		return false;
	}

	CreateLUTTexture();

	SetCubemapLevel(0);

	CreateResamplingPSO();
	CreateLuminancePSO();
	CreateBloomPSO();
	CreateGaussianHPSO();
	CreateGaussianVPSO();

	CreateShadowMap("ShadowMap");
	if (!CreateShadowPSO())
	{
		LOG(GraphicsEngineLog, Error, "Failed to create shadow PSO");
		return false;
	}

	if (!CreateGBuffer())
	{
		LOG(GraphicsEngineLog, Error, "Failed to create Graphics buffer");
		return false;
	}

	if (!CreateDeferredPSO())
	{
		LOG(GraphicsEngineLog, Error, "Failed to create deferred PSO");
		return false;
	}

	if (!CreateLightPSO())
	{
		LOG(GraphicsEngineLog, Error, "Failed to create light PSO");
		return false;
	}

	if (!CreateSpritePSO())
	{
		LOG(GraphicsEngineLog, Error, "Failed to create sprite PSO");
		return false;
	}

	if (!CreateTextPSO())
	{
		LOG(GraphicsEngineLog, Error, "Failed to create text PSO");
		return false;
	}

	if (!CreateVideoPSO())
	{
		LOG(GraphicsEngineLog, Error, "Failed to create spirit PSO");
		return false;
	}

	if (!CreateHealthBarPSO())
	{
		LOG(GraphicsEngineLog, Error, "Failed to create health PSO");
		return false;
	}

	if (!CreateSpiritBarPSO())
	{
		LOG(GraphicsEngineLog, Error, "Failed to create spirit PSO");
		return false;
	}

	if (!CreateTonemapPSO())
	{
		LOG(GraphicsEngineLog, Error, "Failed to create tonemap PSO");
		return false;
	}

	if (!CreateCustomPSO())
	{
		LOG(GraphicsEngineLog, Error, "Failed to create Custom PSO");
		return false;
	}

	if (!CreateCustomPPPSO())
	{
		LOG(GraphicsEngineLog, Error, "Failed to create Custom Post Processing PSO");
		return false;
	}

	if (!CreateSkyboxPSO())
	{
		LOG(GraphicsEngineLog, Error, "Failed to create skybox PSO");
		return false;
	}

	if (!CreateParticlePSO())
	{
		LOG(GraphicsEngineLog, Error, "Failed to create particle PSO");
		return false;
	}


	ConstantBuffer frameBuffer;
	if (!myRHI->CreateConstantBuffer("FrameBuffer", sizeof(FrameBuffer), 0, PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER | PIPELINE_STAGE_GEOMETRY_SHADER, frameBuffer))
	{
		LOG(GraphicsEngineLog, Error, "Failed to create frame buffer");
		return false;
	}

	ConstantBuffer objectBuffer;
	if (!myRHI->CreateConstantBuffer("ObjectBuffer", sizeof(ObjectBuffer), 1, PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER, objectBuffer))
	{
		LOG(GraphicsEngineLog, Error, "Failed to create object buffer");
		return false;
	}

	ConstantBuffer animationBuffer;
	if (!myRHI->CreateConstantBuffer("AnimationBuffer", sizeof(AnimationBuffer), 2, PIPELINE_STAGE_VERTEX_SHADER, animationBuffer))
	{
		LOG(GraphicsEngineLog, Error, "Failed to create animation buffer");
		return false;
	}

	ConstantBuffer materialBuffer;
	if (!myRHI->CreateConstantBuffer("MaterialBuffer", sizeof(MaterialBuffer), 3, PIPELINE_STAGE_PIXEL_SHADER, materialBuffer))
	{
		LOG(GraphicsEngineLog, Error, "Failed to create material buffer");
		return false;
	}

	ConstantBuffer lightBuffer;
	if (!myRHI->CreateConstantBuffer("LightBuffer", sizeof(LightBuffer), 4, PIPELINE_STAGE_PIXEL_SHADER, lightBuffer))
	{
		LOG(GraphicsEngineLog, Error, "Failed to create light buffer");
		return false;
	}

	ConstantBuffer miscBuffer;
	if (!myRHI->CreateConstantBuffer("MiscBuffer", sizeof(MiscBuffer), 5, PIPELINE_STAGE_PIXEL_SHADER, miscBuffer))
	{
		LOG(GraphicsEngineLog, Error, "Failed to create misc buffer");
		return false;
	}

#ifndef _RETAIL
	ConstantBuffer debugBuffer;
	if (!myRHI->CreateConstantBuffer("DebugBuffer", sizeof(DebugBuffer), 10, PIPELINE_STAGE_PIXEL_SHADER, debugBuffer))
	{
		LOG(GraphicsEngineLog, Error, "Failed to create debug buffer");
		return false;
	}
#endif

	myConstantBuffers.emplace(ConstantBufferType::FrameBuffer, std::move(frameBuffer));
	myConstantBuffers.emplace(ConstantBufferType::ObjectBuffer, std::move(objectBuffer));
	myConstantBuffers.emplace(ConstantBufferType::AnimationBuffer, std::move(animationBuffer));
	myConstantBuffers.emplace(ConstantBufferType::MaterialBuffer, std::move(materialBuffer));
	myConstantBuffers.emplace(ConstantBufferType::LightBuffer, std::move(lightBuffer));
	myConstantBuffers.emplace(ConstantBufferType::MiscBuffer, std::move(miscBuffer));
#ifndef _RETAIL
	myConstantBuffers.emplace(ConstantBufferType::DebugBuffer, std::move(debugBuffer));
#endif
	myWindowHandle = aWindowHandle;


	myShaderCompiler = std::make_unique<ShaderCompiler>();
	LoadCustomShaders();
	InitCustomTextures();
	SetCustomTextureResources(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER);
	myEnemyTexture = std::make_shared<Texture>(CreateTexture("Enemy Texture", static_cast<unsigned>(GetWindowSize().x), static_cast<unsigned>(GetWindowSize().y), RHITextureFormat::R16G16_Float, false, true, true, false, false));
	
	CU::Matrix4x4f jonkler;
	ParticleEmitter emitter;
	emitter.Init(new ParticleEmitterSettings(), jonkler);
	emitter.Burst();
	return true;
}

void GraphicsEngine::ChangePipelineState(const PipelineStateObject& aNewPSO)
{
	myRHI->ChangePipelineState(aNewPSO, myCurrentPSO);
	myCurrentPSO = aNewPSO;
}

void GraphicsEngine::ChangeRenderTarget(const std::shared_ptr<Texture>& aRenderTarget) const
{
	myRHI->SetRenderTarget(aRenderTarget);
}

ViewPort GraphicsEngine::GetViewPort()
{
	return myRHI->GetViewPort();
}

CU::Vector2<unsigned> GraphicsEngine::GetDeviceSize() const
{
	RECT clientRect{};
	GetClientRect(myWindowHandle, &clientRect);

	const unsigned clientWidth = clientRect.right - clientRect.left;
	const unsigned clientHeight = clientRect.bottom - clientRect.top;

	return { clientWidth, clientHeight };
}

void GraphicsEngine::RenderSkybox(const Mesh& aMesh) const
{
	if (myCurrentPSO.DepthStencil != myDirlightShadowMap)
	{
		/*if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 100, *myShadowMap))
		{
			LOG(GraphicsEngineLog, Error, "Couldn't set shadow map on slot {}", 100);
		}*/
	}


	myRHI->SetVertexBuffer(aMesh.GetVertexBuffer(), myCurrentVertexShader->VertexStride, 0);
	myRHI->SetIndexBuffer(aMesh.GetIndexBuffer());

	myRHI->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (const auto& e : aMesh.GetElements())
	{
		myRHI->DrawIndexed(e.IndexOffset, e.NumIndices);
	}
}

void GraphicsEngine::RenderMesh(const Mesh& aMesh, const std::vector<std::shared_ptr<Material>>& someMaterials) const
{
	myCurrentPixelShader;
	myCurrentVertexShader;
	//if (myCurrentVertexShader->path.string().find("Shout") != std::string::npos)
	//{
	//	int a = 0;
	//}
	if (myCurrentPSO.DepthStencil != myDirlightShadowMap)
	{
		if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 100, *myDirlightShadowMap))
		{
			LOG(GraphicsEngineLog, Error, "Couldn't set shadow map on slot {}", 100);
		}
	}
	myRHI->SetVertexBuffer(aMesh.GetVertexBuffer(), myCurrentVertexShader->VertexStride, 0);
	myRHI->SetIndexBuffer(aMesh.GetIndexBuffer());
	myRHI->SetPrimitiveTopology(aMesh.GetPrimitiveTopology());

	unsigned materialIndex = UINT_MAX;
	for (const auto& element : aMesh.GetElements())
	{
		if (materialIndex != element.MaterialIndex)
		{
			materialIndex = element.MaterialIndex;
			if (someMaterials.size() > 0)
			{

				for (int i = 0; i < 4/*static_cast<int>(someMaterials.size())*/; i++)
				{
					int slot = 10 * i;
					if (materialIndex >= someMaterials.size())
					{
						break;
					}
					if (someMaterials[materialIndex]->GetAlbedoTexture() != nullptr)
					{
						if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, slot, *someMaterials[materialIndex]->GetAlbedoTexture()))
						{
							GELOG(Error, "Couldn't set albedo texture on slot {}", slot);
						}
					}
					if (someMaterials[materialIndex]->GetNormalTexture() != nullptr)
					{
						if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, slot + 1, *someMaterials[materialIndex]->GetNormalTexture()))
						{
							GELOG(Error, "Couldn't set normal texture on slot {}", slot + 1);
						}
					}
					if (someMaterials[materialIndex]->GetMaterialTexture() != nullptr)
					{
						if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, slot + 2, *someMaterials[materialIndex]->GetMaterialTexture()))
						{
							GELOG(Error, "Couldn't set material texture on slot {}", slot + 2);
						}
					}
					if (someMaterials[materialIndex]->GetFXTexture() != nullptr)
					{
						if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, slot + 3, *someMaterials[materialIndex]->GetFXTexture()))
						{
							GELOG(Error, "Couldn't set material texture on slot {}", slot + 3);
						}
					}
					materialIndex++;
				}
			}
		}

		myRHI->DrawIndexed(element.IndexOffset, element.NumIndices);
	}

	if (!ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 0))
	{
		LOG(GraphicsEngineLog, Error, "Failed to clear texture resource slot {}", 0);
	}
	if (!ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 1))
	{
		LOG(GraphicsEngineLog, Error, "Failed to clear texture resource slot {}", 1);
	}
	if (!ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 2))
	{
		LOG(GraphicsEngineLog, Error, "Failed to clear texture resource slot {}", 2);
	}

	if (myCurrentPSO.DepthStencil != myDirlightShadowMap)
	{
		if (!ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 100))
		{
			LOG(GraphicsEngineLog, Error, "Failed to clear texture resource slot {}", 100);
		}
	}
}


void GraphicsEngine::RenderDecal(const Texture& aTexture)
{
	myRHI->SetIndexBuffer(nullptr);
	myRHI->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 0, aTexture))
	{
		GELOG(Error, "Couldn't set decal as PS resource");
		return;
	}
	//GraphicsEngine::Get().GetRHI()->SetVertexBuffer(aElement.GetVertexBuffer(), myCurrentPSO.VertexStride, 0);
	myRHI->Draw(4);
}

void GraphicsEngine::RenderSprite2D(const Sprite2D& aElement) const
{
	myRHI->SetIndexBuffer(nullptr);
	myRHI->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//myRHI->ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 0);
	SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 0, *aElement.GetTexture());
	myRHI->SetVertexBuffer(aElement.GetVertexBuffer(), myCurrentVertexShader->VertexStride, 0);
	myCurrentPixelShader;
	myRHI->Draw(4);
}

void GraphicsEngine::RenderText(const Text& aText)
{
	auto& textData = aText.GetTextData();
	if (textData.VertexAmount < 1)
	{
		return;
	}
	myRHI->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	myRHI->SetVertexBuffer(textData.VertexBuffer, textData.Stride, textData.Offset);
	myRHI->SetIndexBuffer(textData.IndexBuffer);

	ObjectBuffer jobjectBuffer;
	jobjectBuffer.HasSkinning = false;
	jobjectBuffer.World = aText.GetTransform();
	UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, jobjectBuffer);

	MaterialBuffer materialBuffer;
	materialBuffer.AlbedoTint = aText.GetColor();
	UpdateAndSetConstantBuffer(ConstantBufferType::MaterialBuffer, materialBuffer);

	myRHI->GetContext()->PSSetShaderResources(5, 1, aText.GetFont().textTexture->GetSRV().GetAddressOf());
	//myRHI->SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 5, textFactory->myTextObjects[i]->GetFont()->SRV);
	myRHI->DrawIndexed(0, textData.IndexAmount);

}

void GraphicsEngine::RenderDebugLines(const DebugLineObject& aDebugLineObject)
{
	myRHI->SetVertexBuffer(aDebugLineObject.GetVertexBuffer(), myCurrentVertexShader->VertexStride, 0);
	myRHI->SetIndexBuffer(aDebugLineObject.GetIndexBuffer());
	myRHI->SetPrimitiveTopology(aDebugLineObject.GetPrimitiveTopology());

	myRHI->DrawIndexed(0, aDebugLineObject.GetNumIndices());

}

void GraphicsEngine::RenderPoints(unsigned aPoints)
{
	myRHI->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	myRHI->Draw(aPoints);
}

Texture GraphicsEngine::CreateTexture(const std::string& aName, const unsigned& aWidth, const unsigned& aHeight, const
	RHITextureFormat& aFormat, const bool& aStaging, const bool& aShaderResource, const bool& aRenderTarget,
	const bool& aCpuAccessRead, const bool& aCpuAccessWrite) const
{
	Texture texture = {};
	myRHI->CreateTexture(aName, aWidth, aHeight, aFormat, &texture, aStaging, aShaderResource, aRenderTarget, aCpuAccessRead, aCpuAccessWrite);
	return texture;
}

bool GraphicsEngine::LoadTexture(std::string_view aName, const uint8_t* aTextureDataPtr, const size_t& aTextureDataSize,
	Texture& outTexture) const
{
	outTexture.myName = aName;
	return myRHI->LoadTexture(aName, aTextureDataPtr, aTextureDataSize, outTexture);
}

bool GraphicsEngine::LoadTexture(const std::filesystem::path& aFilePath, Texture& outTexture) const
{
	std::ifstream file(aFilePath, std::ios::binary);

	file.seekg(0, std::ios::end);
	const std::streamoff fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	if (fileSize < 0)
	{
		GELOG(Error, "{} is an invalid path", aFilePath.string());
#ifdef _DEBUG
		//throw("Invalid Filepath in LoadTexture");
#endif
		return false;
	}

	std::vector<uint8_t> fileData;
	fileData.resize(fileSize);
	file.read(reinterpret_cast<char*>(fileData.data()), fileSize);
	file.close();

	outTexture.myName = aFilePath.string();
	return myRHI->LoadTexture(aFilePath.string(), fileData.data(), fileData.size(), outTexture);
}

bool GraphicsEngine::SetTextureResource(unsigned aPipelineStages, unsigned aSlot, const Texture& aTexture) const
{
	return myRHI->SetTextureResource(aPipelineStages, aSlot, aTexture);
}

bool GraphicsEngine::SetCustomTextureResources(unsigned aPipeLineStages) const
{
	for (int textureIndex = 0; textureIndex < myCustomTextures.size(); textureIndex++)
	{
		bool result = SetTextureResource(aPipeLineStages, myTextureIndexToSlot.at(textureIndex), *myCustomTextures[textureIndex]);
		if(!result)
		{
			GELOG(Error, "Couldn't set vfx texture on slot {}", myTextureIndexToSlot.at(textureIndex));
		}
	}
	return true;
}

bool GraphicsEngine::ClearTextureResourceSlot(unsigned aPipelineStages, unsigned aSlot) const
{
	return myRHI->ClearTextureResourceSlot(aPipelineStages, aSlot);
}

void GraphicsEngine::CreateShadowMap(const std::string& aName)
{
	CU::Vector2<unsigned> size = { 8000, 8000 };

	myDirlightShadowMap = std::make_shared<Texture>();
	if (!myRHI->CreateShadowMap(aName, size, *myDirlightShadowMap))
	{
		LOG(GraphicsEngineLog, Error, "Couldn't create shadow map");
	}
}

void GraphicsEngine::CreateLUTTexture()
{
	myLUTTexture = std::make_shared<Texture>();
	if (!myRHI->CreateLUTTexture(*myLUTTexture, myShaders[myShaderMap["brdfLUT_PS"]], myShaders[myShaderMap["Quad_VS"]]))
	{
		LOG(GraphicsEngineLog, Error, "Couldn't create LUT texture");
	}
	myForwardPSO.SamplerStates[14] = myRHI->GetSamplerState("LUTSampler");
}

void GraphicsEngine::CreateResamplingTextures()
{
	SetVertexShader("Quad_VS");
	CU::Vector2<unsigned> size;
	size.x = static_cast<unsigned>(myRHI->GetViewPortSize().x);
	size.y = static_cast<unsigned>(myRHI->GetViewPortSize().y);

	myIntermediateHDRBuffer = std::make_shared<Texture>();
	if (!myRHI->CreateTexture("Intermediate HDR Buffer", size.x, size.y, RHITextureFormat::R16G16B16A16_FLOAT, myIntermediateHDRBuffer.get(), false, true, true, false, false))
	{
		GELOG(Error, "Couldn't create HDR intermediate buffer");
	}
	myIntermediateLDRBuffer = std::make_shared<Texture>();
	if (!myRHI->CreateTexture("Intermediate LDR Buffer", size.x, size.y, RHITextureFormat::R16G16B16A16_FLOAT, myIntermediateLDRBuffer.get(), false, true, true, false, false))
	{
		GELOG(Error, "Couldn't create intermediate LDR buffer");
	}
	myLuminanceBuffer = std::make_shared<Texture>();
	if (!myRHI->CreateTexture("Luminance Buffer", size.x, size.y, RHITextureFormat::R16G16B16A16_FLOAT, myLuminanceBuffer.get(), false, true, true, false, false))
	{
		GELOG(Error, "Couldn't create Luminance buffer");
	}

	size.x = size.x / 2;
	size.y = size.y / 2;
	myHalfSizeScreenBuffer = std::make_shared<Texture>();
	if (!myRHI->CreateTexture("Half Size Buffer", size.x, (size.y), RHITextureFormat::R8G8B8A8_UNORM, myHalfSizeScreenBuffer.get(), false, true, true, false, false))
	{
		GELOG(Error, "Couldn't create Half Size Buffer");
	}
	size.x = size.x / 2;
	size.y = size.y / 2;

	myQuarterSizeScreenBuffer = std::make_shared<Texture>();
	if (!myRHI->CreateTexture("Quarter Size Buffer", size.x, size.y, RHITextureFormat::R8G8B8A8_UNORM, myQuarterSizeScreenBuffer.get(), false, true, true, false, false))
	{
		GELOG(Error, "Couldn't create Quarter Size Buffer");
	}

	size.x = size.x / 2;
	size.y = size.y / 2;	myEightSizeScreenBufferA = std::make_shared<Texture>();
	if (!myRHI->CreateTexture("Eight Size Buffer A", size.x, size.y, RHITextureFormat::R8G8B8A8_UNORM, myEightSizeScreenBufferA.get(), false, true, true, false, false))
	{
		GELOG(Error, "Couldn't create Eight Size Buffer A");
	}

	myEightSizeScreenBufferB = std::make_shared<Texture>();
	if (!myRHI->CreateTexture("Eight Size Buffer B", static_cast<unsigned>(size.x), static_cast<unsigned>(size.y), RHITextureFormat::R8G8B8A8_UNORM, myEightSizeScreenBufferB.get(), false, true, true, false, false))
	{
		GELOG(Error, "Couldn't create Eight Size Buffer B");
	}

}

void GraphicsEngine::BeginEvent(std::string_view aEvent) const
{
	myRHI->BeginEvent(aEvent);
}

void GraphicsEngine::EndEvent() const
{
	myRHI->EndEvent();
}

void GraphicsEngine::SetMarker(std::string_view aMarker) const
{
	myRHI->SetMarker(aMarker);
}

void GraphicsEngine::DrawQuad(const CU::Vector2f& aSize)
{
	//myCurrentPSO;
	myLightPSO.SamplerStates[15] = myRHI->GetSamplerState("ShadowSampler");
	if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 100, *myDirlightShadowMap))
	{
		LOG(GraphicsEngineLog, Error, "Couldn't set shadow map on slot {}", 100);
	}
	if (!SetGBufferResource())
	{
		LOG(GraphicsEngineLog, Error, "Couldn't set GBuffer as resource");
	}
	myRHI->DrawQuad(aSize);
	if (!ClearGBuffer())
	{
		LOG(GraphicsEngineLog, Error, "Couldn't clear GBuffer");
	}
	if (!ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 100))
	{
		LOG(GraphicsEngineLog, Error, "Failed to clear texture resource slot");
	}
}

void GraphicsEngine::ClearBackBuffer() const
{
	myRHI->ClearBackBuffer();
}


CU::Vector2f GraphicsEngine::GetWindowSize()
{
	return myRHI->GetBackBuffer()->GetSize();
}

void GraphicsEngine::CreateIndexBuffer(std::string_view aName, const std::vector<unsigned>& aIndexList,
	Microsoft::WRL::ComPtr<ID3D11Buffer>& aOutIxBuffer)
{
	myRHI->CreateIndexBuffer(aName, aIndexList, aOutIxBuffer);
}

bool GraphicsEngine::UpdateFromIYUV(const uint8_t* data, size_t /*data_size*/, Texture& aTexture)
{
	assert(data);
	D3D11_MAPPED_SUBRESOURCE ms;

	ID3D11Resource* resource;
	aTexture.GetSRV().Get()->GetResource(&resource);
	HRESULT hr = myRHI->GetContext()->Map(resource, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	if (FAILED(hr))
		return false;

	uint32_t bytes_per_texel = 1;
	//assert(format == DXGI_FORMAT_R8_UNORM);
	//assert(data_size == xres * yres * 3 / 4);

	const uint8_t* src = data;
	uint8_t* dst = (uint8_t*)ms.pData;

	// Copy the Y lines
	uint32_t nlines = aTexture.GetTextureSize().y / 2;
	uint32_t bytes_per_row = aTexture.GetTextureSize().x * bytes_per_texel;
	for (uint32_t y = 0; y < nlines; ++y) {
		memcpy(dst, src, bytes_per_row);
		src += bytes_per_row;
		dst += ms.RowPitch;
	}

	// Now the U and V lines, need to add Width/2 pixels of padding between each line
	uint32_t uv_bytes_per_row = bytes_per_row / 2;
	for (uint32_t y = 0; y < nlines; ++y) {
		memcpy(dst, src, uv_bytes_per_row);
		src += uv_bytes_per_row;
		dst += ms.RowPitch;
	}

	myRHI->GetContext()->Unmap(resource, 0);
	return true;
}

void GraphicsEngine::SetPixelShader(const std::string_view& aShaderName)
{
	if (aShaderName == "Empty")
	{
		myRHI->SetPixelShader(nullptr);
		myCurrentPixelShader = nullptr;
		return;
	}
	auto shader = myShaders[myShaderMap[aShaderName]];
	if (!shader)
	{
		GELOG(Error, "{} does not exist, {}", aShaderName);
		return;
	}
	//if (shader->type != ShaderType::PixelShader)
	//{
	//	GELOG(Error, "The pixel shader {} can't be set as it isn't a pixel shader", aShaderName);
	//}
	myRHI->SetPixelShader(shader);
	myCurrentPixelShader = shader;
}

void GraphicsEngine::SetVertexShader(const std::string_view& aShaderName)
{
	myCurrentPSO;
	auto shader = myShaders[myShaderMap[aShaderName]];

	if (!shader)
	{
#ifndef _RELEASE
		GELOG(Error, "{} does not exist, {}", aShaderName);
		return;
#endif
	}
	if (shader->type != ShaderType::VertexShader)
	{
		GELOG(Error, "The vertex shader can't be set as it isn't a vertex shader");
	}
	std::shared_ptr<VertexShader> vShader = std::dynamic_pointer_cast<VertexShader>(shader);
	if (vShader == nullptr)
	{
		GELOG(Error, "The vertex shader can't be set as it isn't a vertex shader");
	}
	myRHI->SetInputLayout(vShader->inputLayout);
	myRHI->SetVertexShader(vShader);
	myCurrentVertexShader = vShader;
}

void GraphicsEngine::SetGeometryShader(const std::string_view& aShaderName)
{
	if (aShaderName == "Empty")
	{
		myRHI->SetGeometryShader(nullptr);
		myCurrentGeometryShader = nullptr;
		return;
	}
	auto shader = myShaders[myShaderMap[aShaderName]];
	if (!shader)
	{
		GELOG(Error, "{} does not exist, {}", aShaderName);
		return;
	}
	myRHI->SetGeometryShader(shader);
	myCurrentGeometryShader = shader;
}

void GraphicsEngine::SetShader(unsigned aShaderID)
{
	auto name = myShaderNames[aShaderID];
	auto shader = myShaders[aShaderID];
	std::shared_ptr<VertexShader> vShader = std::dynamic_pointer_cast<VertexShader>(shader);
	switch (shader->type)
	{
	case ShaderType::Unknown:
		break;
	case ShaderType::VertexShader:
		if (vShader == nullptr)
		{
			GELOG(Error, "The vertex shader can't be set as it isn't a vertex shader though is typed as one");
		}
		myRHI->SetInputLayout(vShader->inputLayout);
		myRHI->SetVertexShader(vShader);
		myCurrentVertexShader = vShader;
		break;
	case ShaderType::GeometryShader:
		break;
	case ShaderType::PixelShader:
		myRHI->SetPixelShader(shader);
		myCurrentPixelShader = shader;
		break;
	}
}

std::shared_ptr<Shader> GraphicsEngine::GetShader(const std::string& aShader)
{
	if (!myShaderMap.contains(aShader))
	{
		GELOG(Error, "The shader {} doesn't exist", aShader);
		return nullptr;
	}
	return myShaders[myShaderMap.at(aShader)];
}
bool GraphicsEngine::AddShader(const std::filesystem::path& aPath, ShaderType aShaderType)
{
	size_t shaderSize = {};
	uint8_t* shaderPtr = myShaderCompiler->CompileShader(aPath, static_cast<unsigned>(aShaderType), shaderSize);
	myShaderNames.emplace_back(aPath.stem().string()); //

	if (shaderSize > 0)
	{
		myShaderMap.insert({ myShaderNames.back(), static_cast<unsigned>(myShaders.size()) });
		if (aPath.stem().string().starts_with("PP_"))
		{
			myTaPPShaders.push_back(myShaderMap.at(myShaderNames.back()));
		}
		if (aShaderType == ShaderType::VertexShader)
		{
			std::shared_ptr<VertexShader> vertexShader = std::make_shared<VertexShader>();
			myShaders.emplace_back(vertexShader);
			if (!myRHI->CreateInputLayout(vertexShader->inputLayout, Vertex::InputLayoutDefinition, shaderPtr, shaderSize))
			{
#ifndef _RETAIL
				LOG(GraphicsEngineLog, Error, "Failed to create input layout from custom shader");
#endif

			}
			vertexShader->VertexStride = sizeof(Vertex);
			myShaders.back()->path = aPath;
			myShaders.back()->type = ShaderType::VertexShader;
		}
		else
		{
			myShaders.emplace_back(std::make_shared<Shader>());
			myShaders.back()->type = ShaderType::PixelShader;
			myShaders.back()->path = aPath;
		}
		if (myRHI->LoadShaderFromMemory(myShaderNames.back(), *myShaders[myShaderMap.at(myShaderNames.back())], shaderPtr, shaderSize))
		{
			return true;
		}
	}
	return false;
}

void GraphicsEngine::SetInternalResolution(float aWidth, float aHeight)
{
	myCurrentPSO = PipelineStateObject();
	myCurrentPSO.UsedRenderTargets = 0;
	myBloomPSO.RenderTarget.fill(nullptr);
	myDeferredPSO.RenderTarget.fill(nullptr);
	myCustomPSO.RenderTarget.fill(nullptr);
	myForwardPSO.RenderTarget.fill(nullptr);
	myShadowPSO.RenderTarget.fill(nullptr);
	myTonemapPSO.RenderTarget.fill(nullptr);
	myLightPSO.RenderTarget.fill(nullptr);
	mySpritePSO.RenderTarget.fill(nullptr);
	myVideoPSO.RenderTarget.fill(nullptr);
	myResamplingPSO.RenderTarget.fill(nullptr);
	myLuminancePSO.RenderTarget.fill(nullptr);
	myGaussianHPSO.RenderTarget.fill(nullptr);
	myGaussianVPSO.RenderTarget.fill(nullptr);
	myTextPSO.RenderTarget.fill(nullptr);
	myBloomPSO.RenderTarget.fill(nullptr);
	myHealthBarPSO.RenderTarget.fill(nullptr);
	mySpiritBarPSO.RenderTarget.fill(nullptr);
	myCustomPSO.RenderTarget.fill(nullptr);
	mySkyboxPSO.RenderTarget.fill(nullptr);

	myBloomPSO.DepthStencil = nullptr;
	myDeferredPSO.DepthStencil = nullptr;
	myCustomPSO.DepthStencil = nullptr;
	myForwardPSO.DepthStencil = nullptr;
	myShadowPSO.DepthStencil = nullptr;
	myTonemapPSO.DepthStencil = nullptr;
	myLightPSO.DepthStencil = nullptr;
	mySpritePSO.DepthStencil = nullptr;
	myVideoPSO.DepthStencil = nullptr;
	myResamplingPSO.DepthStencil = nullptr;
	myLuminancePSO.DepthStencil = nullptr;
	myGaussianHPSO.DepthStencil = nullptr;
	myGaussianVPSO.DepthStencil = nullptr;
	myTextPSO.DepthStencil = nullptr;
	myBloomPSO.DepthStencil = nullptr;
	myHealthBarPSO.DepthStencil = nullptr;
	mySpiritBarPSO.DepthStencil = nullptr;
	myCustomPSO.DepthStencil = nullptr;
	mySkyboxPSO.DepthStencil = nullptr;

	myRHI->SetInternalResolution(aWidth, aHeight);
	CreateGBuffer();
	CreateResamplingTextures();
	CreateForwardPSO();
	CreateShadowPSO();
	CreateDeferredPSO();
	CreateLightPSO();
	CreateSpritePSO();
	CreateVideoPSO();

	CreateTonemapPSO();
	CreateResamplingPSO();
	CreateLuminancePSO();
	CreateGaussianHPSO();
	CreateGaussianVPSO();
	CreateTextPSO();
	CreateBloomPSO();


	CreateHealthBarPSO();
	CreateSpiritBarPSO();
	CreateCustomPSO();
	CreateCustomPPPSO();
	CreateSkyboxPSO();

	FrameBuffer frameBuffer;
	frameBuffer.Resolution = { aWidth, aHeight };
	UpdateAndSetConstantBuffer(ConstantBufferType::FrameBuffer, frameBuffer);
}

bool GraphicsEngine::CompileShader(const std::filesystem::path& aPath)
{
	ShaderType shaderType = ShaderType::Unknown;

	if (!myShaderMap.contains(aPath.stem().string()))
	{
		GELOG(Error, "The shader {} is not in the shadermap", aPath.stem().string());
		return false;
	}

	if (aPath.stem().string().ends_with("PS"))
	{
		shaderType = ShaderType::PixelShader;
	}
	else if (aPath.stem().string().ends_with("GS"))
	{
		shaderType = ShaderType::GeometryShader;
	}
	else if (aPath.stem().string().ends_with("VS"))
	{
		shaderType = ShaderType::VertexShader;
	}
	size_t shaderSize = {};
	uint8_t* shaderPtr = myShaderCompiler->CompileShader(aPath, static_cast<unsigned>(shaderType), shaderSize);

	if (shaderSize > 0)
	{

		if (myRHI->LoadShaderFromMemory(aPath.stem().string(), *myShaders[myShaderMap.at(aPath.stem().string())], shaderPtr, shaderSize))
		{
			return true;
		}

	}
	return false;
}

bool GraphicsEngine::CompileAllCustomShaders()
{
	std::filesystem::path shadersPath = CUSTOM_SHADER_PATH;
	for (auto& file : std::filesystem::recursive_directory_iterator(shadersPath))
	{
		if (file.path().has_filename() && file.path().has_extension())
		{
			if (file.path().extension() == ".hlsli" || file.path().extension() == ".dds" || file.path().extension() == ".json")
			{
				continue;
			}
			if (file.path().extension() != ".hlsl")
			{
				LOG(GraphicsEngineLog, Error, "Invalid extension in shader file {}", file.path().string());
				continue;
			}
			if (!CompileShader(file.path()))
			{
				LOG(GraphicsEngineLog, Error, "Unable to add shader {}", file.path().filename().string());

			}

		}
	}
	return true;
}

const std::vector<unsigned> GraphicsEngine::GetTAPostProcessShaders() const
{
	return myTaPPShaders;
}

bool GraphicsEngine::CreateGBuffer()
{
	myGBuffer.Albedo = std::make_shared<Texture>();
	myGBuffer.WorldNormal = std::make_shared<Texture>();
	myGBuffer.Material = std::make_shared<Texture>();
	myGBuffer.Effects = std::make_shared<Texture>();
	myGBuffer.WorldPosition = std::make_shared<Texture>();

	CommonUtilities::Vector2<float> size = GetWindowSize();

	if (!myRHI->CreateTexture("AlbedoTexture", static_cast<int>(size.x), static_cast<int>(size.y), RHITextureFormat::R8G8B8A8_UNORM, myGBuffer.Albedo.get(), false, true, true, false, false))
	{
		GELOG(Error, "Couldn't create albedo texture for GBuffer");
		return false;
	}
	if (!myRHI->CreateTexture("WorldNormalTexture", static_cast<int>(size.x), static_cast<int>(size.y), RHITextureFormat::R16G16B16A16_SNORM, myGBuffer.WorldNormal.get(), false, true, true, false, false))
	{
		GELOG(Error, "Couldn't create material texture for GBuffer");
		return false;
	}
	if (!myRHI->CreateTexture("MaterialTexture", static_cast<int>(size.x), static_cast<int>(size.y), RHITextureFormat::R8G8B8A8_UNORM, myGBuffer.Material.get(), false, true, true, false, false))
	{
		GELOG(Error, "Couldn't create pixel normal texture for GBuffer");
		return false;
	}
	if (!myRHI->CreateTexture("WorldPositionTexture", static_cast<int>(size.x), static_cast<int>(size.y), RHITextureFormat::R32G32B32A32_Float, myGBuffer.WorldPosition.get(), false, true, true, false, false))
	{
		GELOG(Error, "Couldn't create world position texture for GBuffer");
		return false;
	}
	if (!myRHI->CreateTexture("EffectsTexture", static_cast<int>(size.x), static_cast<int>(size.y), RHITextureFormat::R32G32B32A32_Float, myGBuffer.Effects.get(), false, true, true, false, false))
	{
		GELOG(Error, "Couldn't create emission texture for GBuffer");
		return false;
	}

	return true;
}

bool GraphicsEngine::ClearGBuffer() const
{
	bool result = true;
	if (!ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 10))
	{
		GELOG(Error, "Couldn't clear Gbuffer albedo");
		result = false;
	}
	if (!ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 11))
	{
		GELOG(Error, "Couldn't clear Gbuffer normal");
		result = false;
	}
	if (!ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 12))
	{
		GELOG(Error, "Couldn't clear Gbuffer material");
		result = false;
	}
	if (!ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 13))
	{
		GELOG(Error, "Couldn't clear Gbuffer position");
		result = false;
	}
	if (!ClearTextureResourceSlot(PIPELINE_STAGE_PIXEL_SHADER, 14))
	{
		GELOG(Error, "Couldn't clear Gbuffer position");
		result = false;
	}
	return result;
}

bool GraphicsEngine::SetGBufferResource() const
{
	if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 10, *myGBuffer.Albedo))
	{
		GELOG(Error, "Couldn't set texture resource at slot 10");
		return false;
	}
	if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 11, *myGBuffer.WorldNormal))
	{
		GELOG(Error, "Couldn't set texture resource at slot 11");
		return false;
	}
	if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 12, *myGBuffer.Material))
	{
		GELOG(Error, "Couldn't set texture resource at slot 12");
		return false;
	}
	if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 13, *myGBuffer.WorldPosition))
	{
		GELOG(Error, "Couldn't set texture resource at slot 13");
		return false;
	}
	if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 14, *myGBuffer.Effects))
	{
		GELOG(Error, "Couldn't set texture resource at slot 14");
		return false;
	}
	return true;
}

bool GraphicsEngine::SetCubemapLevel(int aLevel)
{
	//Environment cubemap loading (for BRDF)
	std::filesystem::path relativeTexturePath;
	Texture envCubeTexture;
	switch (aLevel)
	{
	case 1:
		relativeTexturePath = "Assets/Textures/Cubemaps/T_Level1_CM.dds";
		LoadTexture(relativeTexturePath, envCubeTexture);

		LOG(GraphicsEngineLog, Log, "uses cubemap for level 1");

		if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 120, envCubeTexture))
		{
			LOG(GraphicsEngineLog, Error, "Failed to initialize level 1 cubemap for ambient light");
		}
		break;
	case 2:
		relativeTexturePath = "Assets/Textures/Cubemaps/T_Level2_CM.dds";
		LoadTexture(relativeTexturePath, envCubeTexture);
		LOG(GraphicsEngineLog, Log, "uses cubemap for level 2");

		if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 120, envCubeTexture))
		{
			LOG(GraphicsEngineLog, Error, "Failed to initialize level 2 cubemap for ambient light");
		}
		break;
	case 3:
		relativeTexturePath = "Assets/Textures/Cubemaps/T_Level3_CM.dds";
		LoadTexture(relativeTexturePath, envCubeTexture);
		LOG(GraphicsEngineLog, Log, "uses cubemap for level 3");

		if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 120, envCubeTexture))
		{
			LOG(GraphicsEngineLog, Error, "Failed to initialize level 3 cubemap for ambient light");
		}
		break;
	default:
		relativeTexturePath = "Assets/Textures/Cubemaps/T_SnowyHillsideBC6HLinear_CM.dds";
		LoadTexture(relativeTexturePath, envCubeTexture);
		LOG(GraphicsEngineLog, Warning, "uses skansen cubemap");

		if (!SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 120, envCubeTexture))
		{
			LOG(GraphicsEngineLog, Error, "Failed to get a level when choosing cubemap. Defaulting to Skansen");
		}
		break;
	}
	return true;
}

void GraphicsEngine::SetIndexBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aIndexBuffer) const
{
	myRHI->SetIndexBuffer(aIndexBuffer);
}

void GraphicsEngine::SetVertexBuffer(const Microsoft::WRL::ComPtr<ID3D11Buffer>& aVertexBuffer, size_t aVertexSize) const
{
	myRHI->SetVertexBuffer(aVertexBuffer, aVertexSize);
}

void GraphicsEngine::PrepareParticleEmitter(ParticleEmitter* aParticleEmitter)
{
	if (aParticleEmitter->myVertexBuffer)
	{
		myRHI->UpdateVertexBuffer(aParticleEmitter->myVertexBuffer, (uint8_t*)(aParticleEmitter->myParticles.data()), aParticleEmitter->myParticles.size(), sizeof(ParticleVertex));
	}
	else
	{
		myRHI->CreateVertexBuffer("particle emitter vxbuf", aParticleEmitter->myParticles, aParticleEmitter->myVertexBuffer, true);
	}
}

GraphicsEngine::GraphicsEngine() = default;
GraphicsEngine::~GraphicsEngine() = default;

bool GraphicsEngine::CreateForwardPSO()
{
	myForwardPSO.Type = PSOType::Forward;
	myForwardPSO.UsedRenderTargets = 1;

	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	myRHI->CreateBlendState("AlphaBlend", blendDesc);
	myForwardPSO.BlendState = myRHI->GetBlendState("AlphaBlend");

	D3D11_SAMPLER_DESC defaultSamplerDesc = {};
	defaultSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	defaultSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	defaultSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	defaultSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	defaultSamplerDesc.MipLODBias = 0.0f;
	defaultSamplerDesc.MaxAnisotropy = 1;
	defaultSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	defaultSamplerDesc.BorderColor[0] = 1.0f;
	defaultSamplerDesc.BorderColor[1] = 1.0f;
	defaultSamplerDesc.BorderColor[2] = 1.0f;
	defaultSamplerDesc.BorderColor[3] = 1.0f;
	defaultSamplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
	defaultSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	myRHI->CreateSamplerState("default", defaultSamplerDesc);
	myForwardPSO.SamplerStates[0] = myRHI->GetSamplerState("default");

	myForwardPSO.RenderTarget[0] = myIntermediateLDRBuffer;
	myForwardPSO.ClearRenderTarget[0] = false;

	myForwardPSO.DepthStencil = myRHI->GetDepthBuffer();
	myForwardPSO.ClearDepthStencil = false;

	return true;
}

bool GraphicsEngine::CreateParticlePSO()
{
	D3D11_BLEND_DESC blenderDescription = {};
	blenderDescription.AlphaToCoverageEnable = false;
	blenderDescription.IndependentBlendEnable = false;
	blenderDescription.RenderTarget[0].BlendEnable = true;
	blenderDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blenderDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blenderDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blenderDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
	blenderDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blenderDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blenderDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	D3D11_DEPTH_STENCIL_DESC depthDesc = {};
	depthDesc.DepthEnable = true;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDesc.StencilEnable = false;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	myRHI->GetDevice()->CreateDepthStencilState(&depthDesc, myParticlePSO.DepthStencilState.GetAddressOf());

	myRHI->CreateBlendState("particleBlend", blenderDescription);
	myParticlePSO.BlendState = myRHI->GetBlendState("particleBlend");

	myParticlePSO.ClearDepthStencil = false;
	myParticlePSO.ClearRenderTarget[0] = false;

	D3D11_SAMPLER_DESC pointWrapSamplerDesc = {};
	pointWrapSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	pointWrapSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	pointWrapSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	pointWrapSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	pointWrapSamplerDesc.MipLODBias = 0.0f;
	pointWrapSamplerDesc.MaxAnisotropy = 1;
	pointWrapSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	pointWrapSamplerDesc.BorderColor[0] = 1.0f;
	pointWrapSamplerDesc.BorderColor[1] = 1.0f;
	pointWrapSamplerDesc.BorderColor[2] = 1.0f;
	pointWrapSamplerDesc.BorderColor[3] = 1.0f;
	pointWrapSamplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
	pointWrapSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	myRHI->CreateSamplerState("pointWrap", pointWrapSamplerDesc);

	myParticlePSO.RenderTarget[0] = GetIntermediateLDRBuffer();
	myParticlePSO.DepthStencil = myRHI->GetDepthBuffer();
	myParticlePSO.SamplerStates.emplace(0, myRHI->GetSamplerState("pointWrap"));

	myParticlePSO.UsedRenderTargets = 1;
	return true;
}

bool GraphicsEngine::CreateDeferredPSO()
{
	myDeferredPSO.Type = PSOType::Deferred;
	myDeferredPSO.BlendState = nullptr;
	myDeferredPSO.UsedRenderTargets = 5;

	myDeferredPSO.SamplerStates[0] = myRHI->GetSamplerState("default");

	myDeferredPSO.RenderTarget[0] = myGBuffer.Albedo;
	myDeferredPSO.RenderTarget[1] = myGBuffer.WorldNormal;
	myDeferredPSO.RenderTarget[2] = myGBuffer.Material;
	myDeferredPSO.RenderTarget[3] = myGBuffer.WorldPosition;
	myDeferredPSO.RenderTarget[4] = myGBuffer.Effects;

	for (int i = 0; i < myDeferredPSO.UsedRenderTargets; i++)
	{
		myDeferredPSO.ClearRenderTarget[i] = true;
	}
	myDeferredPSO.DepthStencil = myRHI->GetDepthBuffer();
	myDeferredPSO.ClearDepthStencil = true;
	return true;
}

bool GraphicsEngine::CreateShadowPSO()
{
	myShadowPSO.Type = PSOType::Shadow;
	myShadowPSO.BlendState = nullptr;
	myShadowPSO.UsedRenderTargets = 0;

	D3D11_SAMPLER_DESC shadowSamplerDesc = {};
	shadowSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	shadowSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.MipLODBias = 0.0f;
	shadowSamplerDesc.MaxAnisotropy = 1;
	shadowSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	shadowSamplerDesc.BorderColor[0] = 0.0f;
	shadowSamplerDesc.BorderColor[1] = 0.0f;
	shadowSamplerDesc.BorderColor[2] = 0.0f;
	shadowSamplerDesc.BorderColor[3] = 0.0f;
	shadowSamplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
	shadowSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	myRHI->CreateSamplerState("ShadowSampler", shadowSamplerDesc);
	myShadowPSO.SamplerStates[15] = myRHI->GetSamplerState("ShadowSampler");

	myShadowPSO.RenderTarget[0] = nullptr;
	myShadowPSO.ClearRenderTarget[0] = false;

	myShadowPSO.DepthStencil = myDirlightShadowMap;
	myShadowPSO.ClearDepthStencil = true;

	return true;
}

bool GraphicsEngine::CreateLightPSO()
{
	myLightPSO.Type = PSOType::Directional;

	CreateAdditiveBlendState();

	myLightPSO.BlendState = myRHI->GetBlendState("Additive");
	myLightPSO.UsedRenderTargets = 1;
	myLightPSO.SamplerStates[0] = myRHI->GetSamplerState("default");

#include "CompiledShaderHeaders/Quad_VS.h"
#include "CompiledShaderHeaders/Directional_Light_PS.h"

	myLightPSO.RenderTarget[0] = myIntermediateLDRBuffer;
	myLightPSO.ClearRenderTarget[0] = true;

	myLightPSO.DepthStencil = nullptr;
	myLightPSO.ClearDepthStencil = false;


	return true;
}

bool GraphicsEngine::CreateSpritePSO()
{
	mySpritePSO.Type = PSOType::Sprite;
	mySpritePSO.UsedRenderTargets = 1;

	D3D11_SAMPLER_DESC spriteSamplerDesc = {};
	spriteSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	spriteSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	spriteSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	spriteSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	spriteSamplerDesc.MipLODBias = 0.0f;
	spriteSamplerDesc.MaxAnisotropy = 1;
	spriteSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	spriteSamplerDesc.BorderColor[0] = 1.0f;
	spriteSamplerDesc.BorderColor[1] = 1.0f;
	spriteSamplerDesc.BorderColor[2] = 1.0f;
	spriteSamplerDesc.BorderColor[3] = 1.0f;
	spriteSamplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
	spriteSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	myRHI->CreateSamplerState("sprite", spriteSamplerDesc);

	mySpritePSO.SamplerStates[0] = myRHI->GetSamplerState("sprite");

	D3D11_BLEND_DESC blendStateDescription = {};
	blendStateDescription = {};
	blendStateDescription.AlphaToCoverageEnable = false;
	blendStateDescription.IndependentBlendEnable = false;
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	myRHI->CreateBlendState("SpriteBlendState", blendStateDescription);
	mySpritePSO.BlendState = myRHI->GetBlendState("SpriteBlendState");

	mySpritePSO.RenderTarget[0] = myRHI->GetBackBuffer();
	mySpritePSO.ClearRenderTarget[0] = false;

	mySpritePSO.DepthStencil = nullptr;
	mySpritePSO.ClearDepthStencil = false;
	return true;
}

bool GraphicsEngine::CreateVideoPSO()
{
	myVideoPSO.Type = PSOType::Video;
	myVideoPSO.UsedRenderTargets = 1;

	myVideoPSO.SamplerStates[0] = myRHI->GetSamplerState("default");

	myVideoPSO.BlendState = myRHI->GetBlendState("SpriteBlendState");

	myVideoPSO.RenderTarget[0] = myRHI->GetBackBuffer();
	myVideoPSO.ClearRenderTarget[0] = false;

	myVideoPSO.DepthStencil = nullptr;
	myVideoPSO.ClearDepthStencil = false;
	return true;
}

bool GraphicsEngine::CreateCustomPSO()
{
	myCustomPSO.Type = PSOType::Custom;
	myCustomPSO.UsedRenderTargets = 1;

	myCustomPSO.DepthStencilState = myRHI->GetDepthStencilState(DepthState::DS_ReadOnly);
	myCustomPSO.BlendState = myRHI->GetBlendState("AlphaBlend");
	myCustomPSO.SamplerStates[0] = myRHI->GetSamplerState("default");

	myCustomPSO.RenderTarget[0] = myIntermediateLDRBuffer;
	myCustomPSO.ClearRenderTarget[0] = false;

	myCustomPSO.DepthStencil = myRHI->GetDepthBuffer();
	myCustomPSO.ClearDepthStencil = false;

	return true;
}

bool GraphicsEngine::CreateCustomPPPSO()
{
	myCustomPPPSO.Type = PSOType::CustomPP;
	myCustomPPPSO.UsedRenderTargets = 1;

	myCustomPPPSO.BlendState = myRHI->GetBlendState("AlphaBlend");
	myCustomPPPSO.SamplerStates[0] = myRHI->GetSamplerState("default");

	myCustomPPPSO.RenderTarget[0] = myIntermediateLDRBuffer;
	myCustomPPPSO.ClearRenderTarget[0] = false;

	myCustomPPPSO.DepthStencil = nullptr;
	myCustomPPPSO.ClearDepthStencil = false;

	return true;
}


bool GraphicsEngine::CreateTonemapPSO()
{
	myTonemapPSO.Type = PSOType::Tonemap;
	myTonemapPSO.BlendState = nullptr;
	myTonemapPSO.UsedRenderTargets = 1;
	myTonemapPSO.SamplerStates[0] = myRHI->GetSamplerState("default");

	myTonemapPSO.RenderTarget[0] = myIntermediateHDRBuffer;
	myTonemapPSO.ClearRenderTarget[0] = false;

	myTonemapPSO.DepthStencil = nullptr;
	myTonemapPSO.ClearDepthStencil = false;
	return true;
}

bool GraphicsEngine::CreateResamplingPSO()
{
	myResamplingPSO.Type = PSOType::Resampling;
	myResamplingPSO.BlendState = nullptr;
	myResamplingPSO.UsedRenderTargets = 1;

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	samplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	myRHI->CreateSamplerState("defaultClamp", samplerDesc);

	myResamplingPSO.SamplerStates[0] = myRHI->GetSamplerState("defaultClamp");

	myResamplingPSO.RenderTarget[0] = myHalfSizeScreenBuffer;
	myResamplingPSO.ClearRenderTarget[0] = false;

	myResamplingPSO.DepthStencil = nullptr;
	myResamplingPSO.ClearDepthStencil = false;
	return true;
}

bool GraphicsEngine::CreateLuminancePSO()
{
	myLuminancePSO.Type = PSOType::Luminance;
	myLuminancePSO.BlendState = nullptr;
	myLuminancePSO.UsedRenderTargets = 1;
	myLuminancePSO.SamplerStates[0] = myRHI->GetSamplerState("default");

	myLuminancePSO.RenderTarget[0] = myLuminanceBuffer;
	myLuminancePSO.ClearRenderTarget[0] = false;

	myLuminancePSO.DepthStencil = nullptr;
	myLuminancePSO.ClearDepthStencil = false;
	return true;
}

bool GraphicsEngine::CreateGaussianHPSO()
{
	myGaussianHPSO.Type = PSOType::GaussianH;
	myGaussianHPSO.BlendState = nullptr;
	myGaussianHPSO.UsedRenderTargets = 1;

	myGaussianHPSO.SamplerStates[0] = myRHI->GetSamplerState("defaultClamp");

	myGaussianHPSO.RenderTarget[0] = myEightSizeScreenBufferB;
	myGaussianHPSO.ClearRenderTarget[0] = false;

	myGaussianHPSO.DepthStencil = nullptr;
	myGaussianHPSO.ClearDepthStencil = false;
	return true;
}

bool GraphicsEngine::CreateSkyboxPSO()
{
#include "CompiledShaderHeaders/Default_VS.h"
#include "CompiledShaderHeaders/Skybox_PS.h"

	PipelineStateObject skyboxPSO;

	skyboxPSO.BlendState = myRHI->GetBlendState("AlphaBlend");

	//skyboxPSO.VertexShader = std::make_shared<Shader>();
	//myRHI->LoadShaderFromMemory("Default_VS", *skyboxPSO.VertexShader, BuiltIn_Default_VS_ByteCode, sizeof(BuiltIn_Default_VS_ByteCode));

	//skyboxPSO.PixelShader = std::make_shared<Shader>();
	//myRHI->LoadShaderFromMemory("Skybox_PS",  *skyboxPSO.PixelShader, BuiltIn_Skybox_PS_ByteCode, sizeof(BuiltIn_Skybox_PS_ByteCode));

	skyboxPSO.ClearDepthStencil = false;
	skyboxPSO.ClearRenderTarget[0] = false;


	//myRHI->CreateInputLayout(skyboxPSO.InputLayout, Vertex::InputLayoutDefinition, BuiltIn_Default_VS_ByteCode, sizeof(BuiltIn_Default_VS_ByteCode));
	//skyboxPSO.VertexStride = sizeof(Vertex);

	skyboxPSO.RenderTarget[0] = myRHI->GetBackBuffer();
	skyboxPSO.DepthStencil = myRHI->GetDepthBuffer();
	skyboxPSO.SamplerStates.emplace(0, myRHI->GetSamplerState("default"));

	skyboxPSO.UsedRenderTargets = 1;

	mySkyboxPSO = skyboxPSO;

	return true;
}

bool GraphicsEngine::CreateGaussianVPSO()
{
	myGaussianVPSO.Type = PSOType::GaussianV;
	myGaussianVPSO.BlendState = nullptr;
	myGaussianVPSO.UsedRenderTargets = 1;

	myGaussianVPSO.SamplerStates[0] = myRHI->GetSamplerState("defaultClamp");

	myGaussianVPSO.RenderTarget[0] = myEightSizeScreenBufferA;
	myGaussianVPSO.ClearRenderTarget[0] = false;

	myGaussianVPSO.DepthStencil = nullptr;
	myGaussianVPSO.ClearDepthStencil = false;
	return true;
}

bool GraphicsEngine::CreateBloomPSO()
{
	myBloomPSO.Type = PSOType::Bloom;

	D3D11_BLEND_DESC alphaBlendDesc = {};
	alphaBlendDesc.RenderTarget[0].BlendEnable = true;
	alphaBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	alphaBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	alphaBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	alphaBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	alphaBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	alphaBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
	alphaBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	if (!myRHI->CreateBlendState("Alpha", alphaBlendDesc))
	{
		GELOG(Error, "Couldn't create alpha blend desc");
		return false;
	}
	myBloomPSO.BlendState = myRHI->GetBlendState("Alpha");
	myBloomPSO.UsedRenderTargets = 1;

	myBloomPSO.SamplerStates[0] = myRHI->GetSamplerState("defaultClamp");

	myBloomPSO.RenderTarget[0] = myRHI->GetBackBuffer();
	myBloomPSO.ClearRenderTarget[0] = false;

	myBloomPSO.DepthStencil = nullptr;
	myBloomPSO.ClearDepthStencil = false;
	return true;
}

bool GraphicsEngine::CreateTextPSO()
{
	myTextPSO.Type = PSOType::Sprite;
	myTextPSO.BlendState = myRHI->GetBlendState("Alpha");
	myTextPSO.UsedRenderTargets = 1;

	myTextPSO.SamplerStates[0] = myRHI->GetSamplerState("default");

	myTextPSO.RenderTarget[0] = myRHI->GetBackBuffer();
	myTextPSO.ClearRenderTarget[0] = false;

	myTextPSO.DepthStencil = nullptr;
	myTextPSO.ClearDepthStencil = false;
	return true;
}

bool GraphicsEngine::CreateHealthBarPSO()
{
	myHealthBarPSO.Type = PSOType::Sprite;
	myHealthBarPSO.UsedRenderTargets = 1;

	myHealthBarPSO.SamplerStates[0] = myRHI->GetSamplerState("default");

	//D3D11_BLEND_DESC blendStateDescription = {};
	//blendStateDescription = {};
	//blendStateDescription.AlphaToCoverageEnable = false;
	//blendStateDescription.IndependentBlendEnable = false;
	//blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	//blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	//blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	//blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	//blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	//blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	//myRHI->CreateBlendState("SpriteBlendState", blendStateDescription);
	myHealthBarPSO.BlendState = myRHI->GetBlendState("SpriteBlendState");

	//myInterfacePSO.InputLayout = myDefaultPSO.InputLayout;

	myHealthBarPSO.RenderTarget[0] = myRHI->GetBackBuffer();
	myHealthBarPSO.ClearRenderTarget[0] = false;

	myHealthBarPSO.DepthStencil = nullptr;
	myHealthBarPSO.ClearDepthStencil = false;
	return true;
}

bool GraphicsEngine::CreateSpiritBarPSO()
{
	mySpiritBarPSO.Type = PSOType::Sprite;
	mySpiritBarPSO.UsedRenderTargets = 1;

	mySpiritBarPSO.SamplerStates[0] = myRHI->GetSamplerState("default");

	//D3D11_BLEND_DESC blendStateDescription = {};
	//blendStateDescription = {};
	//blendStateDescription.AlphaToCoverageEnable = false;
	//blendStateDescription.IndependentBlendEnable = false;
	//blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	//blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	//blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	//blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	//blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	//blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	//myRHI->CreateBlendState("SpriteBlendState", blendStateDescription);
	mySpiritBarPSO.BlendState = myRHI->GetBlendState("SpriteBlendState");

	//myInterfacePSO.InputLayout = myDefaultPSO.InputLayout;

	mySpiritBarPSO.RenderTarget[0] = myRHI->GetBackBuffer();
	mySpiritBarPSO.ClearRenderTarget[0] = false;

	mySpiritBarPSO.DepthStencil = nullptr;
	mySpiritBarPSO.ClearDepthStencil = false;
	return true;
}

bool GraphicsEngine::CreateAdditiveBlendState() const
{
	D3D11_BLEND_DESC additiveBlendDesc = {};
	additiveBlendDesc.RenderTarget[0].BlendEnable = true;
	additiveBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	additiveBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	additiveBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	additiveBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	additiveBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	additiveBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
	additiveBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	if (!myRHI->CreateBlendState("Additive", additiveBlendDesc))
	{
		GELOG(Error, "Couldn't create additive blend desc");
		return false;
	}
	return true;
}

void GraphicsEngine::LoadCustomShaders()
{
	std::filesystem::path shadersPath = CUSTOM_SHADER_PATH;
	for (auto& file : std::filesystem::recursive_directory_iterator(shadersPath))
	{
		if (file.path().has_filename() && file.path().has_extension())
		{
			if (file.path().extension() == ".hlsli" || file.path().extension() == ".dds" || file.path().extension() == ".json")
			{
				continue;
			}
			if (file.path().extension() != ".hlsl")
			{
				LOG(GraphicsEngineLog, Error, "Invalid extension in shader file {}", file.path().string());

				continue;
			}
			ShaderType shaderType = ShaderType::Unknown;
			if (file.path().filename().stem().string().ends_with("_VS"))
			{
				shaderType = ShaderType::VertexShader;
			}
			else if (file.path().filename().stem().string().ends_with("_PS"))
			{
				shaderType = ShaderType::PixelShader;
			}
			else if (file.path().filename().stem().string().ends_with("_GS"))
			{
				shaderType = ShaderType::GeometryShader;
			}
			if (!AddShader(file.path(), shaderType))
			{
				LOG(GraphicsEngineLog, Error, "Unable to add shader {}", file.path().filename().string());

			}

		}
	}

}

void GraphicsEngine::InitCustomTextures()
{
	std::string filePath = "Assets\\VFX\\Textures\\TexturesToSlots.json";
	std::filesystem::path texturesDirectory = "Assets\\VFX\\Textures\\";
	nlohmann::json jsonReader;
	std::fstream file(filePath);
	if (!file.good())
	{
		LOG(GraphicsEngineLog, Error, "The TexturesToSlots file could not be loaded. Is it located in bin/settings? Is it formatted correctly?");
		return;
	}
	file >> jsonReader;

	if (jsonReader.contains("Textures"))
	{
		for (auto& texture : jsonReader.at("Textures"))
		{
			int slot = -1;
			if (texture.contains("Slot"))
			{
				slot = texture["Slot"];
				if (slot < 0 || slot < 50 || slot > 99)
				{
					LOG(GraphicsEngineLog, Error, "You cannot set texture {} on this slot. Slots available are 50-99");
					continue;
				}
			}
			if (texture.contains("Texture"))
			{
				std::string textureName = texture["Texture"];
				if (textureName.empty())
				{
					continue;
				}

				std::filesystem::path texturePath = texturesDirectory.string() + textureName;

				myCustomTextures.emplace_back(std::make_shared<Texture>());
				if (!LoadTexture(texturePath, *myCustomTextures.back()))
				{
					LOG(GraphicsEngineLog, Error, "Unable to load texture: ", texturePath.string());
					myCustomTextures.back() = nullptr;
					myCustomTextures.erase(myCustomTextures.begin() + myCustomTextures.size() - 1);
					continue;
				}
				LOG(GraphicsEngineLog, Log, "Added texture {} to slot : {}", texturePath.string(), slot);
				myTextureIndexToSlot.insert({ static_cast<int>(myCustomTextures.size() - 1), slot });
			}
		}
	}



	//std::filesystem::path texturesPath = "Assets\\Shaders\\TATextures";
	//int textureSlotCount = 50;
	//for (auto& file : std::filesystem::recursive_directory_iterator(texturesPath))
	//{
	//	if (file.path().has_filename() && file.path().has_extension())
	//	{
	//		if (file.path().extension() != ".dds" && file.path().extension() != "DDS")
	//		{
	//			LOG(GraphicsEngineLog, Error, "{} has incorrect fileformat", file.path().string());
	//			continue;
	//
	//		}
	//	}
	//	myCustomTextures.emplace_back(std::make_shared<Texture>());
	//	if (!LoadTexture(file.path(), *myCustomTextures.back()))
	//	{
	//		LOG(GraphicsEngineLog, Error, "Unable to load texture: ", file.path().string()); 
	//		myCustomTextures.back() = nullptr;
	//		myCustomTextures.erase(myCustomTextures.begin() + myCustomTextures.size() - 1);
	//		continue;
	//	}
	//
	//	LOG(GraphicsEngineLog, Log, "Added texture {} to slot : {}", file.path().filename().string(), textureSlotCount);
	//	myTextureIndexToSlot.insert({ static_cast<int>(myCustomTextures.size() - 1), textureSlotCount });
	//	textureSlotCount++;
	//
	//	if (textureSlotCount > 59)
	//	{
	//		break;
	//	}
	//}

}


void GraphicsEngine::CreateShaders()
{
	CreateDefaultShaders();
	CreateUIShaders();
	CreatePostProcessingShaders();
}

void GraphicsEngine::CreateDefaultShaders()
{
	{
#pragma region DefaultShaders
#include "CompiledShaderHeaders/Default_VS.h"
#include "CompiledShaderHeaders/Default_PS.h"

		myShaderNames.emplace_back("Default_VS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<VertexShader>());
		myShaders.back()->path = "Default_VS";
		std::shared_ptr<VertexShader> vertexShader = std::dynamic_pointer_cast<VertexShader>(myShaders.back());
		if (vertexShader == nullptr)
		{
			GELOG(Error, "{} is not a vertex shader", myShaderNames.back());
		}
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *vertexShader, BuiltIn_Default_VS_ByteCode, sizeof(BuiltIn_Default_VS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to load VS-Shader from memory, {}", myShaderNames.back());
		}
		if (!myRHI->CreateInputLayout(vertexShader->inputLayout, Vertex::InputLayoutDefinition, BuiltIn_Default_VS_ByteCode, sizeof(BuiltIn_Default_VS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to create default input layout");
		}
		vertexShader->VertexStride = sizeof(Vertex);

		myShaderNames.emplace_back("Default_PS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		myShaders.back()->path = "Default_PS";
		std::shared_ptr<Shader> shader = myShaders.back();
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *shader, BuiltIn_Default_PS_ByteCode, sizeof(BuiltIn_Default_PS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to load PS-Shader from memory, {}", myShaderNames.back());
		}
#pragma endregion
	}
	{
#pragma region DirectionalLightShader
#include "CompiledShaderHeaders/Directional_Light_PS.h"

		myShaderNames.emplace_back("Directional_Light_PS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		myShaders.back()->path = "Directional_Light_PS";
		std::shared_ptr<Shader> shader = myShaders.back();
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *shader, BuiltIn_Directional_Light_PS_ByteCode, sizeof(BuiltIn_Directional_Light_PS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to load PS-Shader from memory, {}", myShaderNames.back());
		}
#pragma endregion
	}

	{
#pragma region PointLightShader
#include "CompiledShaderHeaders/PointLight_PS.h"

		myShaderNames.emplace_back("PointLight_PS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		myShaders.back()->path = "PointLight_PS";
		std::shared_ptr<Shader> shader = myShaders.back();
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *shader, BuiltIn_PointLight_PS_ByteCode, sizeof(BuiltIn_PointLight_PS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to load PS-Shader from memory, {}", myShaderNames.back());
		}
#pragma endregion
	}

	{
#pragma region SpotlightShader
#include "CompiledShaderHeaders/Spotlight_PS.h"

		myShaderNames.emplace_back("Spotlight_PS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		myShaders.back()->path = "Spotlight_PS";
		std::shared_ptr<Shader> shader = myShaders.back();
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *shader, BuiltIn_Spotlight_PS_ByteCode, sizeof(BuiltIn_Spotlight_PS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to load PS-Shader from memory, {}", myShaderNames.back());
		}
#pragma endregion
	}

	{
#pragma region QuadShaders
#include "CompiledShaderHeaders/Quad_VS.h"

		myShaderNames.emplace_back("Quad_VS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<VertexShader>());
		myShaders.back()->path = "Quad_VS";
		std::shared_ptr<VertexShader> vertexShader = std::dynamic_pointer_cast<VertexShader>(myShaders.back());
		if (vertexShader == nullptr)
		{
			GELOG(Error, "{} is not a vertex shader", myShaderNames.back());
		}
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *vertexShader, BuiltIn_Quad_VS_ByteCode, sizeof(BuiltIn_Quad_VS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to load VS-Shader from memory, {}", myShaderNames.back());
		}
		if (!myRHI->CreateInputLayout(vertexShader->inputLayout, Vertex::InputLayoutDefinition, BuiltIn_Quad_VS_ByteCode, sizeof(BuiltIn_Quad_VS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to create input layout for {}", myShaderNames.back());
		}
		vertexShader->VertexStride = sizeof(Vertex);
#pragma endregion
	}
}

void GraphicsEngine::CreatePostProcessingShaders()
{
	{
#pragma region BRDFShader
#include "CompiledShaderHeaders/brdfLUT_PS.h"

		myShaderNames.emplace_back("brdfLUT_PS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		myShaders.back()->path = "brdfLUT_PS";
		std::shared_ptr<Shader> shader = myShaders.back();
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *shader, BuiltIn_brdfLUT_PS_ByteCode, sizeof(BuiltIn_brdfLUT_PS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to load PS-Shader from memory, {}", myShaderNames.back());
		}
#pragma endregion
	}
	{
#pragma region TonemapShader
#include "CompiledShaderHeaders/Tonemap_PS.h"

		myShaderNames.emplace_back("Tonemap_PS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		myShaders.back()->path = "Tonemap_PS";
		std::shared_ptr<Shader> shader = myShaders.back();
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *shader, BuiltIn_Tonemap_PS_ByteCode, sizeof(BuiltIn_Tonemap_PS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to load PS-Shader from memory, {}", myShaderNames.back());
		}
#pragma endregion
	}
	{
#pragma region LuminanceShader
#include "CompiledShaderHeaders/Luminance_PS.h"

		myShaderNames.emplace_back("Luminance_PS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		myShaders.back()->path = "Luminance_PS";
		std::shared_ptr<Shader> shader = myShaders.back();
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *shader, BuiltIn_Luminance_PS_ByteCode, sizeof(BuiltIn_Luminance_PS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to load PS-Shader from memory, {}", myShaderNames.back());
		}
#pragma endregion
	}

	{
#pragma region GaussianHShader
#include "CompiledShaderHeaders/GaussianH_PS.h"

		myShaderNames.emplace_back("GaussianH_PS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		myShaders.back()->path = "GaussianH_PS";
		std::shared_ptr<Shader> shader = myShaders.back();
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *shader, BuiltIn_GaussianH_PS_ByteCode, sizeof(BuiltIn_GaussianH_PS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to load PS-Shader from memory, {}", myShaderNames.back());
		}
#pragma endregion
	}

	{
#pragma region GaussianVShader
#include "CompiledShaderHeaders/GaussianV_PS.h"

		myShaderNames.emplace_back("GaussianV_PS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		myShaders.back()->path = "GaussianV_PS";
		std::shared_ptr<Shader> shader = myShaders.back();
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *shader, BuiltIn_GaussianV_PS_ByteCode, sizeof(BuiltIn_GaussianV_PS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to load PS-Shader from memory, {}", myShaderNames.back());
		}
#pragma endregion
	}

	{
#pragma region GBufferShader
#include "CompiledShaderHeaders/GBuffer_PS.h"

		myShaderNames.emplace_back("GBuffer_PS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		myShaders.back()->path = "GBuffer_PS";
		std::shared_ptr<Shader> shader = myShaders.back();
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *shader, BuiltIn_GBuffer_PS_ByteCode, sizeof(BuiltIn_GBuffer_PS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to load PS-Shader from memory, {}", myShaderNames.back());
		}
#pragma endregion
	}

	{
#pragma region BloomShader
#include "CompiledShaderHeaders/Bloom_PS.h"

		myShaderNames.emplace_back("Bloom_PS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		myShaders.back()->path = "Bloom_PS";
		std::shared_ptr<Shader> shader = myShaders.back();
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *shader, BuiltIn_Bloom_PS_ByteCode, sizeof(BuiltIn_Bloom_PS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to load PS-Shader from memory, {}", myShaderNames.back());
		}
#pragma endregion
	}
}

void GraphicsEngine::CreateUIShaders()
{
	{
#pragma region ResampleShader
#include "CompiledShaderHeaders/Resample_PS.h"

		myShaderNames.emplace_back("Resample_PS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		myShaders.back()->path = "Resample_PS";
		std::shared_ptr<Shader> shader = myShaders.back();
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *shader, BuiltIn_Resample_PS_ByteCode, sizeof(BuiltIn_Resample_PS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to load PS-Shader from memory, {}", myShaderNames.back());
		}
#pragma endregion
	}

	{
#pragma region HP_orbShader
#include "CompiledShaderHeaders/HP_orb_3_PS.h"

		myShaderNames.emplace_back("HP_orb_3_PS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		myShaders.back()->path = "HP_orb_3_PS";
		std::shared_ptr<Shader> shader = myShaders.back();
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *shader, BuiltIn_HP_orb_3_PS_ByteCode, sizeof(BuiltIn_HP_orb_3_PS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to load PS-Shader from memory, {}", myShaderNames.back());
		}
#pragma endregion
	}
	{
#pragma region VideoShader
#include "CompiledShaderHeaders/Video_PS.h"

		myShaderNames.emplace_back("Video_PS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		myShaders.back()->path = "Video_PS";
		std::shared_ptr<Shader> shader = myShaders.back();
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *shader, BuiltIn_Video_PS_ByteCode, sizeof(BuiltIn_Video_PS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to load PS-Shader from memory, {}", myShaderNames.back());
		}
#pragma endregion
	}
	{
#pragma region Skyboxshader
#include "CompiledShaderHeaders/Skybox_PS.h"

		myShaderNames.emplace_back("Skybox_PS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		myShaders.back()->path = "Skybox_PS";
		std::shared_ptr<Shader> shader = myShaders.back();
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *shader, BuiltIn_Skybox_PS_ByteCode, sizeof(BuiltIn_Skybox_PS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to load PS-Shader from memory, {}", myShaderNames.back());
		}
#pragma endregion
	}

	{
#pragma region SpiritOrbShader
#include "CompiledShaderHeaders/Spirit_orb_PS.h"

		myShaderNames.emplace_back("Spirit_orb_PS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		myShaders.back()->path = "Spirit_orb_PS";
		std::shared_ptr<Shader> shader = myShaders.back();
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *shader, BuiltIn_Spirit_orb_PS_ByteCode, sizeof(BuiltIn_Spirit_orb_PS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to load PS-Shader from memory, {}", myShaderNames.back());
		}
#pragma endregion
	}
	{
#pragma region CooldownShader
#include "CompiledShaderHeaders/Cooldown_timer_PS.h"

		myShaderNames.emplace_back("Cooldown_timer_PS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		myShaders.back()->path = "Cooldown_timer_PS";
		std::shared_ptr<Shader> shader = myShaders.back();
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *shader, BuiltIn_Cooldown_timer_PS_ByteCode, sizeof(BuiltIn_Cooldown_timer_PS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to load PS-Shader from memory, {}", myShaderNames.back());
		}
#pragma endregion
	}
	{
#pragma region TextShaders
#include "CompiledShaderHeaders/Text_VS.h"
#include "CompiledShaderHeaders/Text_PS.h"

		myShaderNames.emplace_back("Text_VS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<VertexShader>());
		myShaders.back()->path = "Text_VS";
		std::shared_ptr<VertexShader> vertexShader = std::dynamic_pointer_cast<VertexShader>(myShaders.back());
		vertexShader->inputLayout;
		if (vertexShader == nullptr)
		{
			GELOG(Error, "{} is not a vertex shader", myShaderNames.back());
		}
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *vertexShader, BuiltIn_Text_VS_ByteCode, sizeof(BuiltIn_Text_VS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to load VS-Shader from memory, {}", myShaderNames.back());
		}
		if (!myRHI->CreateInputLayout(vertexShader->inputLayout, SpriteVertex::InputLayerDefinition, BuiltIn_Text_VS_ByteCode, sizeof(BuiltIn_Text_VS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to create input layout for {}", myShaderNames.back());
		}
		vertexShader->VertexStride = sizeof(SpriteVertex);

		myShaderNames.emplace_back("Text_PS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		myShaders.back()->path = "Text_PS";
		std::shared_ptr<Shader> shader = myShaders.back();
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *shader, BuiltIn_Text_PS_ByteCode, sizeof(BuiltIn_Text_PS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to load PS-Shader from memory, {}", myShaderNames.back());
		}
#pragma endregion
	}
	{
#pragma region UIShaders
#include "CompiledShaderHeaders/UI_VS.h"
#include "CompiledShaderHeaders/UI_PS.h"

		myShaderNames.emplace_back("UI_VS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<VertexShader>());
		myShaders.back()->path = "UI_VS";
		std::shared_ptr<VertexShader> vertexShader = std::dynamic_pointer_cast<VertexShader>(myShaders.back());
		if (vertexShader == nullptr)
		{
			GELOG(Error, "{} is not a vertex shader", myShaderNames.back());
		}
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *vertexShader, BuiltIn_UI_VS_ByteCode, sizeof(BuiltIn_UI_VS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to load VS-Shader from memory, {}", myShaderNames.back());
		}
		if (!myRHI->CreateInputLayout(vertexShader->inputLayout, SpriteVertex::InputLayerDefinition, BuiltIn_UI_VS_ByteCode, sizeof(BuiltIn_UI_VS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to create input layout for {}", myShaderNames.back());
		}
		vertexShader->VertexStride = sizeof(SpriteVertex);

		myShaderNames.emplace_back("UI_PS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		myShaders.back()->path = "UI_PS";
		std::shared_ptr<Shader> shader = myShaders.back();
		shader->type = ShaderType::PixelShader;
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *shader, BuiltIn_UI_PS_ByteCode, sizeof(BuiltIn_UI_PS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to load PS-Shader from memory, {}", myShaderNames.back());
		}
#pragma endregion
	}

	{
#pragma region ParticleShaders
#include "CompiledShaderHeaders/Particle_VS.h"
#include "CompiledShaderHeaders/Particle_PS.h"
#include "CompiledShaderHeaders/Quad_GS.h"
		myShaderNames.emplace_back("Particle_VS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<VertexShader>());
		myShaders.back()->path = "Particle_VS";
		std::shared_ptr<VertexShader> vertexShader = std::dynamic_pointer_cast<VertexShader>(myShaders.back());
		vertexShader->inputLayout;
		if (vertexShader == nullptr)
		{
			GELOG(Error, "{} is not a vertex shader", myShaderNames.back());
		}
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *vertexShader, BuiltIn_Particle_VS_ByteCode, sizeof(BuiltIn_Particle_VS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to load VS-Shader from memory, {}", myShaderNames.back());
		}
		if (!myRHI->CreateInputLayout(vertexShader->inputLayout, ParticleVertex::InputLayoutDefinition, BuiltIn_Particle_VS_ByteCode, sizeof(BuiltIn_Particle_VS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to create input layout for {}", myShaderNames.back());
		}
		vertexShader->VertexStride = sizeof(SpriteVertex);

		myShaderNames.emplace_back("Particle_PS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		myShaders.back()->path = "Particle_PS";
		std::shared_ptr<Shader> shader = myShaders.back();
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *shader, BuiltIn_Particle_PS_ByteCode, sizeof(BuiltIn_Particle_PS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to load PS-Shader from memory, {}", myShaderNames.back());
		}

		myShaderNames.emplace_back("Quad_GS");
		myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
		myShaders.push_back(std::make_shared<Shader>());
		myShaders.back()->path = "Quad_GS";
		std::shared_ptr<Shader> geoShader = myShaders.back();
		if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *geoShader, BuiltIn_Quad_GS_ByteCode, sizeof(BuiltIn_Quad_GS_ByteCode)))
		{
			LOG(GraphicsEngineLog, Error, "Failed to load GS-Shader from memory, {}", myShaderNames.back());
		}

#pragma endregion
	}
}

void GraphicsEngine::CreateDebugShaders()
{
#pragma region DebugLineShader
#include "CompiledShaderHeaders/DebugLine_VS.h"

	myShaderNames.emplace_back("DebugLine_VS");
	myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
	myShaders.push_back(std::make_shared<VertexShader>());
	myShaders.back()->path = "DebugLine_VS";
	std::shared_ptr<VertexShader> debugLineShader = std::dynamic_pointer_cast<VertexShader>(myShaders.back());
	if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *debugLineShader, BuiltIn_DebugLine_VS_ByteCode, sizeof(BuiltIn_DebugLine_VS_ByteCode)))
	{
		LOG(GraphicsEngineLog, Error, "Failed to load PS-Shader from memory, {}", myShaderNames.back());
	}
	if (!myRHI->CreateInputLayout(debugLineShader->inputLayout, Vertex::InputLayoutDefinition, BuiltIn_DebugLine_VS_ByteCode, sizeof(BuiltIn_DebugLine_VS_ByteCode)))
	{
		LOG(GraphicsEngineLog, Error, "Failed to create input layout for {}", myShaderNames.back());
	}
	debugLineShader->VertexStride = sizeof(Vertex);
#pragma endregion

#pragma region DebugLineShader
#include "CompiledShaderHeaders/DebugLine_PS.h"

	myShaderNames.emplace_back("DebugLine_PS");
	myShaderMap.emplace(myShaderNames.back(), static_cast<unsigned>(myShaders.size()));
	myShaders.push_back(std::make_shared<Shader>());
	myShaders.back()->path = "DebugLine_PS";
	std::shared_ptr<Shader> debugPixelShader = myShaders.back();
	if (!myRHI->LoadShaderFromMemory(myShaderNames.back(), *debugPixelShader, BuiltIn_DebugLine_PS_ByteCode, sizeof(BuiltIn_DebugLine_PS_ByteCode)))
	{
		LOG(GraphicsEngineLog, Error, "Failed to load PS-Shader from memory, {}", myShaderNames.back());
	}
#pragma endregion
}
