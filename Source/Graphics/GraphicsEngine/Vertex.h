#pragma once
#include "RHIStructs.h"
#include "CommonUtilities/Vector.hpp"

struct Vertex
{
	CU::Vector4f Position = { 0.f, 0.f, 0.f, 1.f };
	CU::Vector4f Color = { 0.f, 0.f, 0.f, 1.f };
	CU::Vector4<unsigned> BoneIDs = {};
	CU::Vector4f SkinWeights = {};
	CU::Vector2f UV = {};
	CU::Vector3f Normal = {};
	CU::Vector3f Tangent = {};

	Vertex() = default;
	~Vertex() = default;

	Vertex(CU::Vector3<float> aPosition)
	{
		Position = { aPosition.x, aPosition.y, aPosition.z, 1.0f };
		Color = { 1.0f, 1.0f, 0.0f, 1.0f };
	}

	Vertex(float aX, float aY, float aZ)
	{
		Position = { aX, aY, aZ, 1.f };
		Color = { 1.0f, 1.0f, 0.0f, 1.0f };
	}
	Vertex(float aX, float aY, float aZ, float aRed, float aGreen, float aBlue, float aAlpha)
	{
		Position = { aX, aY, aZ, 1.f };
		Color = { aRed, aBlue, aGreen, aAlpha };
	}
	Vertex(CU::Vector4f aPosition, CU::Vector4f aColor)
	{
		Position = aPosition;
		Color = aColor;
	}
	Vertex(CU::Vector4f aPosition, CU::Vector4f aColor, CU::Vector4<unsigned> someBoneIDs, CU::Vector4f someSkinWeights)
	{
		Position = aPosition;
		Color = aColor;
		BoneIDs = someBoneIDs;
		SkinWeights = someSkinWeights;
	}
	Vertex(CU::Vector4f aPosition, CU::Vector4f aColor, CU::Vector4<unsigned> someBoneIDs,
		CU::Vector4f someSkinWeights, CU::Vector2f aUV)
	{
		Position = aPosition;
		Color = aColor;
		BoneIDs = someBoneIDs;
		SkinWeights = someSkinWeights;
		UV = aUV;
	}
	Vertex(CU::Vector4f aPosition, CU::Vector4f aColor, CU::Vector4<unsigned> someBoneIDs,
		CU::Vector4f someSkinWeights, CU::Vector2f aUV, CU::Vector3f aNormal, CU::Vector3f aTangent)
	{
		Position = aPosition;
		Color = aColor;
		BoneIDs = someBoneIDs;
		SkinWeights = someSkinWeights;
		UV = aUV;
		Normal = aNormal;
		Tangent = aTangent;
	}

	Vertex(CU::Vector4f aPosition, CU::Vector4f aNormal, CU::Vector4f aTangent, CU::Vector2f aUV)
	{
		Position = aPosition;
		Normal = { aNormal.x, aNormal.y, aNormal.z };
		Tangent = { aTangent.x, aTangent.y, aTangent.z };
		Color = { 1.0f, 0.5f, 0.5f, 1.0f };
		UV = aUV;
	}

	static const std::vector<VertexElementDesc> InputLayoutDefinition;
};