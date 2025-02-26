#include "GraphicsEngine.pch.h"
#include "SpriteVertex.h"

const std::vector<VertexElementDesc> SpriteVertex::InputLayerDefinition =
{
	{"WORLDPOS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT},
	{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT },
};