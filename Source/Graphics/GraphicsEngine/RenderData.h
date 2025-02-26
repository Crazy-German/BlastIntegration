#pragma once
#include "AnimationBuffer.h"
#include "DebugBuffer.h"
#include "DebugColor.h"
#include "DebugLineObject.h"
#include "FrameBuffer.h"
#include "LightBuffer.h"
#include "Mesh.h"
#include "MiscBuffer.h"
#include "Shader.h"
#include "Sprite2D.h"
#include "Text/Text.h"

#define RENDERER MainSingleton::Get().GetRenderer();

enum class GraphicsCommandType
{
	BeginEvent,
	Bloom,
	ChangePipelineStage,
	ClearBackBuffer,
	DrawDebugLines,
	DrawQuad,
	EndEvent,
	Present,
	RenderInterface,
	RenderMesh,
	RenderSkyBox,
	RenderSpriteNoShaders,
	RenderText,
	RenderVideo,
	SetAnimationBuffer,
	SetCustomTextures,
	SetDebugBuffer,
	SetFrameBuffer,
	SetGBufferResource,
	SetLightBuffer,
	SetMarker,
	SetMiscBuffer,
	SetShaders,
	SetTextureResource,
	TAPostProcess,
	Tonemap
};

struct RenderData
{
	bool isEmpty = false;;
	GraphicsCommandType type;
};

struct SpriteRenderData : RenderData
{
	Sprite2D element;
	bool useShaders;
};

struct EventData : RenderData
{
	std::string eventName;
};

struct MarkerData : RenderData
{
	std::string markerName;
};

struct DebugBufferData : RenderData
{
	DebugBuffer debugBuffer;
};

struct TextRenderData : RenderData
{
	Text text;
};

struct MeshRenderData : RenderData
{
	Mesh mesh;
	CU::Matrix4x4f matrix;
	std::vector<std::shared_ptr<Material>> materials; // Only pointer in here, but should be fine, as the materials are stored in the assetmanager/GE and doesn't switch place in the memory
	bool isOutlinedA;
	bool isOutlinedB;
	float isOutlinedC;
};

struct SkyboxData : RenderData
{
	Mesh myMesh;
	CU::Matrix4x4f myTransform;
};

struct DebugObjectData : RenderData
{
	CU::Matrix4x4f transform;
	bool hasBones = false;
	DebugLineObject debugObject;

	Mesh mesh;
	DebugColor color = DebugColor::Red; // Default value from before
	std::string debugName = "";			// Default value from before
};
struct ShaderData : RenderData
{
	std::string shaderName;
	ShaderType shaderType;
};

struct PSOData : RenderData
{
	PipelineStateObject* PSO;
};

struct QuadData : RenderData
{
	CU::Vector2f quadSize = { -1, -1 };
};

struct CustomTextureData : RenderData
{
	PIPELINE_STAGE pipelineStage;
};

struct VideoRenderData : RenderData
{
	Texture texture;
};

struct AnimationBufferData : RenderData
{
	AnimationBuffer animationBuffer;
};

struct FrameBufferData : RenderData
{
	FrameBuffer frameBuffer;
};

struct LightBufferData : RenderData
{
	LightBuffer myLightBuffer;
};

struct MiscBufferData : RenderData
{
	MiscBuffer myMiscBuffer;
};

struct SetTextureData : RenderData
{
	unsigned myPipelineStages;
	unsigned mySlot;
	Texture myTexture;
};