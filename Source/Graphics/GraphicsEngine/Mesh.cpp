#include "GraphicsEngine.pch.h"

#include <corecrt_math_defines.h>
#include <d3dcommon.h>
#include "Vertex.h"

#include <CommonUtilities/Serializer.h>

Mesh::Mesh() = default;
Mesh::~Mesh() = default;

void Mesh::Initialize(std::vector<Vertex>&& aVertexList, std::vector<unsigned>&& aIndexList,
	std::vector<Element>&& aElementList, Skeleton&& aSkeleton, const std::string& aName)
{
	myVertices = std::move(aVertexList);
	myIndices = std::move(aIndexList);
	myElements = std::move(aElementList);
	mySkeleton = std::move(aSkeleton);
	myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	GraphicsEngine::Get().CreateVertexBuffer(aName + " vertex buffer", myVertices, myVertexBuffer);
	GraphicsEngine::Get().CreateIndexBuffer(aName + " index buffer", myIndices, myIndexBuffer);
}
void Mesh::InitializeNoBuffer(std::vector<Vertex>&& aVertexList, std::vector<unsigned>&& aIndexList,
	std::vector<Element>&& aElementList, Skeleton&& aSkeleton, const std::string& aName)
{
	aName;
	myVertices = std::move(aVertexList);
	myIndices = std::move(aIndexList);
	myElements = std::move(aElementList);
	mySkeleton = std::move(aSkeleton);
}

void Mesh::GenerateBuffers(const std::string& aName)
{
	myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	GraphicsEngine::Get().CreateVertexBuffer(aName + " vertex buffer", myVertices, myVertexBuffer);
	GraphicsEngine::Get().CreateIndexBuffer(aName + " index buffer", myIndices, myIndexBuffer);
}

Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetVertexBuffer() const
{
	return myVertexBuffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetIndexBuffer() const
{
	return myIndexBuffer;
}

std::vector<Mesh::Element> Mesh::GetElements() const
{
	return myElements;
}

Skeleton& Mesh::GetSkeleton() 
{
	return mySkeleton;
}

unsigned Mesh::GetPrimitiveTopology() const
{
	return myPrimitiveTopology;
}

const std::vector<Vertex>& Mesh::GetVertices() const
{
	return myVertices;
}

const std::vector<unsigned>& Mesh::GetIndices() const
{
	return myIndices;
}

void Mesh::GenerateCube(const float aSize)
{
	float halfSize = aSize / 2.f;
	std::vector<Vertex> vertices = {
		// Front
		{
			CU::Vector4f(halfSize, halfSize, -halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 0.f),
			CU::Vector3f(0.f, 0.f, -1.f),
			CU::Vector3f(1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(halfSize, -halfSize, -halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 1.f),
			CU::Vector3f(0.f, 0.f, -1.f),
			CU::Vector3f(1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(-halfSize, halfSize, -halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 0.f),
			CU::Vector3f(0.f, 0.f, -1.f),
			CU::Vector3f(1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(-halfSize, -halfSize, -halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 1.f),
			CU::Vector3f(0.f, 0.f, -1.f),
			CU::Vector3f(1.f, 0.f, 0.f)
		},

		// Left
		{
			CU::Vector4f(-halfSize, halfSize, halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 0.f),
			CU::Vector3f(-1.f, 0.f, 0.f),
			CU::Vector3f(0.f, 0.f, -1.f)
		},
		{
			CU::Vector4f(-halfSize, halfSize, -halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 0.f),
			CU::Vector3f(-1.f, 0.f, 0.f),
			CU::Vector3f(0.f, 0.f,-1.f)
		},
		{
			CU::Vector4f(-halfSize, -halfSize, halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 1.f),
			CU::Vector3f(-1.f, 0.f, 0.f),
			CU::Vector3f(0.f, 0.f, -1.f)
		},
		{
			CU::Vector4f(-halfSize, -halfSize, -halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 1.f),
			CU::Vector3f(-1.f, 0.f, 0.f),
			CU::Vector3f(0.f, 0.f, -1.f)
		},

		// Top
		{
			CU::Vector4f(halfSize, halfSize, halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 1.f),
			CU::Vector3f(0.f, 1.f, 0.f),
			CU::Vector3f(-1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(halfSize, halfSize, -halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 0.f),
			CU::Vector3f(0.f, 1.f, 0.f),
			CU::Vector3f(-1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(-halfSize, halfSize, halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 1.f),
			CU::Vector3f(0.f, 1.f, 0.f),
			CU::Vector3f(-1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(-halfSize, halfSize, -halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 0.f),
			CU::Vector3f(0.f, 1.f, 0.f),
			CU::Vector3f(-1.f, 0.f, 0.f)
		},

		// Back
		{
			CU::Vector4f(halfSize, halfSize, halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 0.f),
			CU::Vector3f(0.f, 0.f, 1.f),
			CU::Vector3f(-1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(halfSize, -halfSize, halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 1.f),
			CU::Vector3f(0.f, 0.f, 1.f),
			CU::Vector3f(-1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(-halfSize, halfSize, halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 0.f),
			CU::Vector3f(0.f, 0.f, 1.f),
			CU::Vector3f(-1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(-halfSize, -halfSize, halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 1.f),
			CU::Vector3f(0.f, 0.f, 1.f),
			CU::Vector3f(-1.f, 0.f, 0.f)
		},

		// Right
		{
			CU::Vector4f(halfSize,halfSize, halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 0.f),
			CU::Vector3f(1.f, 0.f, 0.f),
			CU::Vector3f(0.f, 0.f, 1.f)
		},
		{
			CU::Vector4f(halfSize, halfSize, -halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 0.f),
			CU::Vector3f(1.f, 0.f, 0.f),
			CU::Vector3f(0.f, 0.f, 1.f)
		},
		{
			CU::Vector4f(halfSize, -halfSize, halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 1.f),
			CU::Vector3f(1.f, 0.f, 0.f),
			CU::Vector3f(0.f, 0.f, 1.f)
		},
		{
			CU::Vector4f(halfSize, -halfSize, -halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 1.f),
			CU::Vector3f(1.f, 0.f, 0.f),
			CU::Vector3f(0.f, 0.f, 1.f)
		},

		// Bottom
		{
			CU::Vector4f(halfSize, -halfSize, halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 1.f),
			CU::Vector3f(0.f, -1.f, 0.f),
			CU::Vector3f(1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(halfSize, -halfSize, -halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 0.f),
			CU::Vector3f(0.f, -1.f, 0.f),
			CU::Vector3f(1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(-halfSize, -halfSize, halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 1.f),
			CU::Vector3f(0.f, -1.f, 0.f),
			CU::Vector3f(1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(-halfSize, -halfSize, -halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 0.f),
			CU::Vector3f(0.f, -1.f, 0.f),
			CU::Vector3f(1.f, 0.f, 0.f)
		}
	};

	std::vector<unsigned> indices;
	indices.reserve(sizeof(unsigned) * 36);
	indices = { };
	for (int i = 0; i < 12; i += 4)
	{
		indices.push_back(i);
		indices.push_back(i + 1);
		indices.push_back(i + 2);

		indices.push_back(i + 3);
		indices.push_back(i + 2);
		indices.push_back(i + 1);
	}
	for (int i = 12; i < 24; i += 4)
	{
		indices.push_back(i + 2);
		indices.push_back(i + 1);
		indices.push_back(i);

		indices.push_back(i + 1);
		indices.push_back(i + 2);
		indices.push_back(i + 3);
	}

	Element element;
	element.VertexOffset = 0;
	element.IndexOffset = 0;
	element.NumVertices = static_cast<unsigned>(vertices.size());
	element.NumIndices = static_cast<unsigned>(indices.size());

	std::vector<Element> elements = { element };

	Initialize(std::move(vertices), std::move(indices), std::move(elements), Skeleton(), "Cube");
}

void Mesh::GenerateCube(const CU::Vector3f aSize)
{
	CU::Vector3f halfSize = aSize;
	std::vector<Vertex> vertices = {
		// Front
		{
			CU::Vector4f(halfSize.x, halfSize.y, -halfSize.z, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 0.f),
			CU::Vector3f(0.f, 0.f, -1.f),
			CU::Vector3f(1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(halfSize.x, -halfSize.y, -halfSize.z, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 1.f),
			CU::Vector3f(0.f, 0.f, -1.f),
			CU::Vector3f(1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(-halfSize.x, halfSize.y, -halfSize.z, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 0.f),
			CU::Vector3f(0.f, 0.f, -1.f),
			CU::Vector3f(1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(-halfSize.x, -halfSize.y, -halfSize.z, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 1.f),
			CU::Vector3f(0.f, 0.f, -1.f),
			CU::Vector3f(1.f, 0.f, 0.f)
		},

		// Left
		{
			CU::Vector4f(-halfSize.x, halfSize.y, halfSize.z, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 0.f),
			CU::Vector3f(-1.f, 0.f, 0.f),
			CU::Vector3f(0.f, 0.f, -1.f)
		},
		{
			CU::Vector4f(-halfSize.x, halfSize.y, -halfSize.z, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 0.f),
			CU::Vector3f(-1.f, 0.f, 0.f),
			CU::Vector3f(0.f, 0.f,-1.f)
		},
		{
			CU::Vector4f(-halfSize.x, -halfSize.y, halfSize.z, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 1.f),
			CU::Vector3f(-1.f, 0.f, 0.f),
			CU::Vector3f(0.f, 0.f, -1.f)
		},
		{
			CU::Vector4f(-halfSize.x, -halfSize.y, -halfSize.z, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 1.f),
			CU::Vector3f(-1.f, 0.f, 0.f),
			CU::Vector3f(0.f, 0.f, -1.f)
		},

		// Top
		{
			CU::Vector4f(halfSize.x, halfSize.y, halfSize.z, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 1.f),
			CU::Vector3f(0.f, 1.f, 0.f),
			CU::Vector3f(-1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(halfSize.x, halfSize.y, -halfSize.z, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 0.f),
			CU::Vector3f(0.f, 1.f, 0.f),
			CU::Vector3f(-1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(-halfSize.x, halfSize.y, halfSize.z, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 1.f),
			CU::Vector3f(0.f, 1.f, 0.f),
			CU::Vector3f(-1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(-halfSize.x, halfSize.y, -halfSize.z, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 0.f),
			CU::Vector3f(0.f, 1.f, 0.f),
			CU::Vector3f(-1.f, 0.f, 0.f)
		},

		// Back
		{
			CU::Vector4f(halfSize.x, halfSize.y, halfSize.z, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 0.f),
			CU::Vector3f(0.f, 0.f, 1.f),
			CU::Vector3f(-1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(halfSize.x, -halfSize.y, halfSize.z, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 1.f),
			CU::Vector3f(0.f, 0.f, 1.f),
			CU::Vector3f(-1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(-halfSize.x, halfSize.y, halfSize.z, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 0.f),
			CU::Vector3f(0.f, 0.f, 1.f),
			CU::Vector3f(-1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(-halfSize.x, -halfSize.y, halfSize.z, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 1.f),
			CU::Vector3f(0.f, 0.f, 1.f),
			CU::Vector3f(-1.f, 0.f, 0.f)
		},

		// Right
		{
			CU::Vector4f(halfSize.x,halfSize.y, halfSize.z, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 0.f),
			CU::Vector3f(1.f, 0.f, 0.f),
			CU::Vector3f(0.f, 0.f, 1.f)
		},
		{
			CU::Vector4f(halfSize.x, halfSize.y, -halfSize.z, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 0.f),
			CU::Vector3f(1.f, 0.f, 0.f),
			CU::Vector3f(0.f, 0.f, 1.f)
		},
		{
			CU::Vector4f(halfSize.x, -halfSize.y, halfSize.z, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 1.f),
			CU::Vector3f(1.f, 0.f, 0.f),
			CU::Vector3f(0.f, 0.f, 1.f)
		},
		{
			CU::Vector4f(halfSize.x, -halfSize.y, -halfSize.z, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 1.f),
			CU::Vector3f(1.f, 0.f, 0.f),
			CU::Vector3f(0.f, 0.f, 1.f)
		},

		// Bottom
		{
			CU::Vector4f(halfSize.x, -halfSize.y, halfSize.z, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 1.f),
			CU::Vector3f(0.f, -1.f, 0.f),
			CU::Vector3f(1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(halfSize.x, -halfSize.y, -halfSize.z, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 0.f),
			CU::Vector3f(0.f, -1.f, 0.f),
			CU::Vector3f(1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(-halfSize.x, -halfSize.y, halfSize.z, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 1.f),
			CU::Vector3f(0.f, -1.f, 0.f),
			CU::Vector3f(1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(-halfSize.x, -halfSize.y, -halfSize.z, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 0.f),
			CU::Vector3f(0.f, -1.f, 0.f),
			CU::Vector3f(1.f, 0.f, 0.f)
		}
	};

	std::vector<unsigned> indices;
	indices.reserve(sizeof(unsigned) * 36);
	indices = { };
	for (int i = 0; i < 12; i += 4)
	{
		indices.push_back(i);
		indices.push_back(i + 1);
		indices.push_back(i + 2);

		indices.push_back(i + 3);
		indices.push_back(i + 2);
		indices.push_back(i + 1);
	}
	for (int i = 12; i < 24; i += 4)
	{
		indices.push_back(i + 2);
		indices.push_back(i + 1);
		indices.push_back(i);

		indices.push_back(i + 1);
		indices.push_back(i + 2);
		indices.push_back(i + 3);
	}

	Element element;
	element.VertexOffset = 0;
	element.IndexOffset = 0;
	element.NumVertices = static_cast<unsigned>(vertices.size());
	element.NumIndices = static_cast<unsigned>(indices.size());

	std::vector<Element> elements = { element };

	Initialize(std::move(vertices), std::move(indices), std::move(elements), Skeleton(), "Cube");
}

void Mesh::GenerateHorizontalPlane(const float aSize)
{
	float halfSize = aSize / 2.f;
	std::vector<Vertex> vertices = {
		{
			CU::Vector4f(halfSize, 0.f, halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 1.f),
			CU::Vector3f(0.f, 1.f, 0.f),
			CU::Vector3f(-1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(halfSize, 0.f, -halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 0.f),
			CU::Vector3f(0.f, 1.f, 0.f),
			CU::Vector3f(-1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(-halfSize, 0.f, halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 1.f),
			CU::Vector3f(0.f, 1.f, 0.f),
			CU::Vector3f(-1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(-halfSize, 0.f, -halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 0.f),
			CU::Vector3f(0.f, 1.f, 0.f),
			CU::Vector3f(-1.f, 0.f, 0.f)
		},
	};

	std::vector<unsigned> indices = {
		0, 1, 2,
		3, 2, 1
	};

	Element element;
	element.VertexOffset = 0;
	element.IndexOffset = 0;
	element.NumVertices = static_cast<unsigned>(vertices.size());
	element.NumIndices = static_cast<unsigned>(indices.size());

	std::vector<Element> elements = { element };

	Initialize(std::move(vertices), std::move(indices), std::move(elements), Skeleton(), "Horizontal plane");
}

void Mesh::GenerateVerticalPlane(const float aSize, bool aVisible)
{
	float halfSize = aSize / 2.f;
	CU::Vector4f color = {};
	if (aVisible) 
	{
		color = { 1.f, 1.f, 1.f, 1.f };
	}
	std::vector<Vertex> vertices = {
		{
		CU::Vector4f(halfSize, halfSize, 0.f, 1.f),
			color,
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 0.f),
			CU::Vector3f(0.f, 0.f, -1.f),
			CU::Vector3f(1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(halfSize, -halfSize, 0.f, 1.f),
			color,
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 1.f),
			CU::Vector3f(0.f, 0.f, -1.f),
			CU::Vector3f(1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(-halfSize, halfSize, 0.f, 1.f),
			color,
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 0.f),
			CU::Vector3f(0.f, 0.f, -1.f),
			CU::Vector3f(1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(-halfSize, -halfSize, 0.f, 1.f),
			color,
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 1.f),
			CU::Vector3f(0.f, 0.f, -1.f),
			CU::Vector3f(1.f, 0.f, 0.f)
		},
	};

	std::vector<unsigned> indices = {0, 1, 2, 1, 3, 2};

	Element element;
	element.VertexOffset = 0;
	element.IndexOffset = 0;
	element.NumVertices = static_cast<unsigned>(vertices.size());
	element.NumIndices = static_cast<unsigned>(indices.size());

	std::vector<Element> elements = { element };

	Initialize(std::move(vertices), std::move(indices), std::move(elements), Skeleton(), "Vertical plane");
}

void Mesh::GenerateSphere(const float aRadius, const float aVertexCount)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;
	unsigned int stacks = 20;
	unsigned int slices = 20;
    for (unsigned int stack = 0; stack <= stacks; ++stack) {
        float phi = static_cast<float>(stack) * static_cast<float>(M_PI / stacks);
        float cosPhi = cos(phi);
        float sinPhi = sin(phi);

        for (unsigned int slice = 0; slice <= slices; ++slice) {
            float theta = static_cast<float>(slice) * 2.0f * static_cast<float>(M_PI / slices);
            float cosTheta = cos(theta);
            float sinTheta = sin(theta);
            float x = aRadius * sinPhi * cosTheta;
            float y = aRadius * cosPhi;
            float z = aRadius * sinPhi * sinTheta;
            float w = 1.0f;

            float nx = sinPhi * cosTheta;
            float ny = cosPhi;
            float nz = sinPhi * sinTheta;
            float nw = 0.0f;
            //Tangent bs
            CommonUtilities::Vector3<float> tangent1 = CommonUtilities::Vector3<float>(nx, ny, nz).Cross(CommonUtilities::Vector3<float>(0, 0, 1));
            CommonUtilities::Vector3<float> tangent2 = CommonUtilities::Vector3<float>(nx, ny, nz).Cross(CommonUtilities::Vector3<float>(0, 1, 0));

            if (tangent1.LengthSqr() > tangent2.LengthSqr())
            {
                tangent2 = tangent1;
            }

            float u = static_cast<float>(slice) / slices;
            float v = static_cast<float>(stack) / stacks;
			Vertex vertex = Vertex{CU::Vector4f(x,y,z,1), CU::Vector4f(nx,ny,nz,1), CU::Vector4f(tangent2.x, tangent2.y, tangent2.z, 1), CU::Vector2f(u,v)};


            vertices.push_back(vertex);
        }
    }

	for (unsigned int stack = 0; stack < stacks; ++stack) {
        for (unsigned int slice = 0; slice < slices; ++slice) {
            unsigned int first = (stack * (slices + 1)) + slice;
            unsigned int second = first + slices + 1;
                indices.push_back(first + 1);
                indices.push_back(second);
                indices.push_back(first);

                indices.push_back(first + 1);
                indices.push_back(second + 1);
                indices.push_back(second);
            
        }
    }
	Element element;
    element.VertexOffset = 0;
    element.IndexOffset = 0;
    element.NumVertices = static_cast<unsigned>(vertices.size());
    element.NumIndices = static_cast<unsigned>(indices.size());

    std::vector<Element> sphereElements = { element };
    Initialize(std::move(vertices),std::move(indices), std::move(sphereElements), Skeleton(),"Sphere");
}

void Mesh::GenerateTube(const CU::Vector3f& aStartPos, const CU::Vector3f& aEndPos, const float aRadius, const int aSegments)
{
    std::vector<Vertex> cylinderVertexList;
    std::vector<unsigned> cylinderIndices;

    // Compute height and axis of the cylinder
    CU::Vector3f axis = aEndPos - aStartPos;
    float height = axis.Length();
	height;
    axis.Normalize();

    // Calculate the midpoint to center the cylinder around the origin
    CU::Vector3f midpoint = (aStartPos + aEndPos) * 0.5f;

    // Generate vertices for top and bottom circles
    float angleStep = 2.0f * 3.14159f / static_cast<float>(aSegments);

    CU::Vector3f right = (fabsf(axis.y) < 0.999f) ? CU::Vector3f(0.0f, 1.0f, 0.0f) : CU::Vector3f(1.0f, 0.0f, 0.0f);
    right = right.Cross(axis).GetNormalized();  // Cross product to get perpendicular
    CU::Vector3f forward = axis.Cross(right).GetNormalized();

    for (int i = 0; i <= aSegments; ++i)
    {
        float angle = i * angleStep;
        float x = cos(angle) * aRadius;
        float y = sin(angle) * aRadius;
        float u = static_cast<float>(i) / static_cast<float>(aSegments);

        // Bottom circle vertices, centered around the origin
        CU::Vector3f bottomVertexPos3d = (aStartPos - midpoint) + (x * right) + (y * forward);
        CU::Vector4f bottomVertexPos = {bottomVertexPos3d.x, bottomVertexPos3d.y, bottomVertexPos3d.z, 1.0f};
        CU::Vector4f bottomNormal = CU::Vector4f(0, -1, 0, 0);  // Y-axis downwards for the bottom
        CU::Vector4f bottomTangent = CU::Vector4f(1, 0, 0, 0);  // Tangent pointing along X
        CU::Vector2f bottomUV = {u, 0};
        cylinderVertexList.push_back(Vertex{ bottomVertexPos, bottomNormal, bottomTangent, bottomUV});

        // Top circle vertices, centered around the origin
        CU::Vector3f topVertexPos3d = (aEndPos - midpoint) + (x * right) + (y * forward);
        CU::Vector4f topVertexPos = { topVertexPos3d.x, topVertexPos3d.y, topVertexPos3d.z, 1.0f };
        CU::Vector4f topNormal = CU::Vector4f(0, 1, 0, 0);  // Y-axis upwards for the top
        CU::Vector4f topTangent = CU::Vector4f(1, 0, 0, 0);  // Tangent pointing along X
        CU::Vector2f topUV = {u, 1.0f};

        cylinderVertexList.push_back(Vertex{ topVertexPos, topNormal, topTangent, topUV});
    }

    // Generate indices for the sides
    for (int i = 0; i < aSegments; ++i)
    {
        int bottomIdx0 = 2 * i;
        int bottomIdx1 = 2 * ((i + 1) % aSegments);

        int topIdx0 = bottomIdx0 + 1;
        int topIdx1 = bottomIdx1 + 1;

        // Side triangles
        cylinderIndices.push_back(bottomIdx0);
        cylinderIndices.push_back(topIdx0);
        cylinderIndices.push_back(bottomIdx1);

        cylinderIndices.push_back(topIdx0);
        cylinderIndices.push_back(topIdx1);
        cylinderIndices.push_back(bottomIdx1);
    }

    // Bottom face indices
    for (int i = 0; i < aSegments - 1; ++i)
    {
        cylinderIndices.push_back(0); // Center vertex at bottom
        cylinderIndices.push_back(2 * (i + 1));
        cylinderIndices.push_back(2 * i);
    }

    // Top face indices
    int topCenterIndex = static_cast<int>(cylinderVertexList.size() - 1);
    for (int i = 0; i < aSegments - 1; ++i)
    {
        cylinderIndices.push_back(topCenterIndex); // Center vertex at top
        cylinderIndices.push_back(2 * i + 1);
        cylinderIndices.push_back(2 * (i + 1) + 1);
    }

    Element element;
    element.VertexOffset = 0;
    element.IndexOffset = 0;
    element.NumVertices = static_cast<unsigned>(cylinderVertexList.size());
    element.NumIndices = static_cast<unsigned>(cylinderIndices.size());

    std::vector<Element> cylinderElements = { element };

    Initialize(std::move(cylinderVertexList), std::move(cylinderIndices), std::move(cylinderElements), Skeleton(), "Tuuuube");
}
void Mesh::GenerateCapsule(const float aRadius, const float aHeight)
{
   std::vector<Vertex> vertices;
    std::vector<unsigned> indices;
	unsigned int stacks = 20;
	unsigned int slices = 20;
    for (unsigned int stack = 0; stack <= stacks; ++stack) {
        float phi = static_cast<float>(stack) * static_cast<float>(M_PI / stacks);
        float cosPhi = cos(phi);
        float sinPhi = sin(phi);

        for (unsigned int slice = 0; slice <= slices; ++slice) {
            float theta = static_cast<float>(slice) * 2.0f * static_cast<float>(M_PI / slices);
            float cosTheta = cos(theta);
            float sinTheta = sin(theta);
            float x = aRadius * sinPhi * cosTheta;
            float y = aRadius * cosPhi;
			if(stack<stacks/2)
			{
				y+=aHeight;
			}
            float z = aRadius * sinPhi * sinTheta;
            float w = 1.0f;

            float nx = sinPhi * cosTheta;
            float ny = cosPhi;
            float nz = sinPhi * sinTheta;
            float nw = 0.0f;
            //Tangent bs
            CommonUtilities::Vector3<float> tangent1 = CommonUtilities::Vector3<float>(nx, ny, nz).Cross(CommonUtilities::Vector3<float>(0, 0, 1));
            CommonUtilities::Vector3<float> tangent2 = CommonUtilities::Vector3<float>(nx, ny, nz).Cross(CommonUtilities::Vector3<float>(0, 1, 0));

            if (tangent1.LengthSqr() > tangent2.LengthSqr())
            {
                tangent2 = tangent1;
            }

            float u = static_cast<float>(slice) / slices;
            float v = static_cast<float>(stack) / stacks;
			Vertex vertex = Vertex{CU::Vector4f(x,y,z,1), CU::Vector4f(nx,ny,nz,1), CU::Vector4f(tangent2.x, tangent2.y, tangent2.z, 1), CU::Vector2f(u,v)};


            vertices.push_back(vertex);
        }
    }

	for (unsigned int stack = 0; stack < stacks; ++stack) {
        for (unsigned int slice = 0; slice < slices; ++slice) {
            unsigned int first = (stack * (slices + 1)) + slice;
            unsigned int second = first + slices + 1;
                indices.push_back(first + 1);
                indices.push_back(second);
                indices.push_back(first);

                indices.push_back(first + 1);
                indices.push_back(second + 1);
                indices.push_back(second);
            
        }
    }
	Element element;
    element.VertexOffset = 0;
    element.IndexOffset = 0;
    element.NumVertices = static_cast<unsigned>(vertices.size());
    element.NumIndices = static_cast<unsigned>(indices.size());

    std::vector<Element> sphereElements = { element };
    Initialize(std::move(vertices),std::move(indices), std::move(sphereElements), Skeleton(),"Capsule");
}


void Mesh::GenerateHorizontalTriangle(float aSize)
{
	float halfSize = aSize / 2.f;
	std::vector<Vertex> vertices = {
		{
			CU::Vector4f(halfSize, 0.f, halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 1.f),
			CU::Vector3f(0.f, 1.f, 0.f),
			CU::Vector3f(-1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(halfSize, 0.f, -halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(0.f, 0.f),
			CU::Vector3f(0.f, 1.f, 0.f),
			CU::Vector3f(-1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(-halfSize, 0.f, halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 1.f),
			CU::Vector3f(0.f, 1.f, 0.f),
			CU::Vector3f(-1.f, 0.f, 0.f)
		},
		{
			CU::Vector4f(-halfSize, 0.f, -halfSize, 1.f),
			CU::Vector4f(1.f, 1.f, 1.f, 1.f),
			CU::Vector4<unsigned>(0, 0, 0, 0),
			CU::Vector4f(0.f, 0.f, 0.f, 0.f),
			CU::Vector2f(1.f, 0.f),
			CU::Vector3f(0.f, 1.f, 0.f),
			CU::Vector3f(-1.f, 0.f, 0.f)
		},
	};

	std::vector<unsigned> indices = { 0, 1, 2 };

	Element element;
	element.VertexOffset = 0;
	element.IndexOffset = 0;
	element.NumVertices = static_cast<unsigned>(vertices.size());
	element.NumIndices = static_cast<unsigned>(indices.size());

	std::vector<Element> elements = { element };

	Initialize(std::move(vertices), std::move(indices), std::move(elements), Skeleton(), "Horizontal plane");
}

CU::AABB3Df& Mesh::GetBoundingBox()
{
	if(!myHasBoundingBox)
	{
		GenerateBoundingBox();
	}
	return myBoundingBox;
}

void Mesh::ResizeBoundingBox(CommonUtilities::Vector3f aSize)
{
	CommonUtilities::Vector3f pos = myBoundingBox.GetCenterPoistion();
	myBoundingBox = {pos-aSize/2.f, pos+aSize/2.f};
	myHasBoundingBox = true;
}


void Mesh::GenerateBoundingBox()
{
	CU::Vector3f minAABB3D(static_cast<float>(INT_MAX), static_cast<float>(INT_MAX), static_cast<float>(INT_MAX));
	CU::Vector3f maxAABB3D(static_cast<float>(INT_MIN), static_cast<float>(INT_MIN), static_cast<float>(INT_MIN));

	for (auto& vertex : myVertices)
	{
		if (vertex.Position.x < minAABB3D.x)
		{
			minAABB3D.x = vertex.Position.x;
		}
		if (vertex.Position.x > maxAABB3D.x)
		{
			maxAABB3D.x = vertex.Position.x;
		}
		if (vertex.Position.y < minAABB3D.y)
		{
			minAABB3D.y = vertex.Position.y;
		}
		if (vertex.Position.y > maxAABB3D.y)
		{
			maxAABB3D.y = vertex.Position.y;
		}
		if (vertex.Position.z < minAABB3D.z)
		{
			minAABB3D.z = vertex.Position.z;
		}
		if (vertex.Position.z > maxAABB3D.z)
		{
			maxAABB3D.z = vertex.Position.z;
		}
	}
	myBoundingBox = { minAABB3D, maxAABB3D };
	myHasBoundingBox = true;
}

void Mesh::Serialize(Serializer& aSerializer)
{
	//MEsh
	size_t numElements = myElements.size();
	aSerializer << numElements;
	myElements.resize(numElements);
	for (size_t element = 0; element < numElements; ++element)
	{
		const Element& current = myElements.at(element);
		aSerializer << current.NumVertices;
		aSerializer << current.VertexOffset;
		aSerializer << current.NumIndices;
		aSerializer << current.IndexOffset;
		aSerializer << current.MaterialIndex;
	}
	size_t numVerticies = myVertices.size();
	aSerializer << numVerticies;
	myVertices.resize(numVerticies);
	for (size_t v = 0; v < numVerticies; ++v)
	{
		const Vertex& current = myVertices[v];
		aSerializer << current.Position;
		aSerializer << current.Color;
		aSerializer << current.BoneIDs;
		aSerializer << current.SkinWeights;
		aSerializer << current.UV;
		aSerializer << current.Normal;
		aSerializer << current.Tangent;
	}
	size_t indexAmount = myIndices.size();
	aSerializer << indexAmount;
	myIndices.resize(indexAmount);
	for (size_t i = 0; i < indexAmount; ++i)
	{
		const unsigned& current = myIndices[i];
		aSerializer << current;
	}
	//Skeleton
	bool hasSkeleton = !mySkeleton.Joints.empty();
	aSerializer << hasSkeleton;
	if(hasSkeleton)
	{
		//Joints
		size_t numJoints = mySkeleton.Joints.size();
		aSerializer << numJoints;
		mySkeleton.Joints.resize(numJoints);
		for(size_t j = 0; j<numJoints; ++j)
		{
			Skeleton::Joint& joint = mySkeleton.Joints[j];
			aSerializer << joint.BindPoseInverse;
			aSerializer << joint.Parent;
			size_t numChildren = joint.Children.size();
			aSerializer << numChildren;
			joint.Children.resize(numChildren);
			for(size_t c = 0; c<numChildren; ++c)
			{
				aSerializer << joint.Children[c];
			}
			size_t nameSize = joint.Name.size();
			aSerializer << nameSize;
			joint.Name.resize(nameSize);
			aSerializer.Serialize(joint.Name, nameSize);
		}
		//Map Serialization Serialize one and create the other
		size_t mapSize = mySkeleton.IndexToJointName.size();
		aSerializer << mapSize;
		mySkeleton.IndexToJointName.reserve(mapSize);
		if(mySkeleton.IndexToJointName.empty())//read
		{
			for(size_t nr = 0; nr<mapSize; ++nr)
			{
				unsigned index = 0;
				aSerializer << index;
				size_t nameSize = 0;
				aSerializer << nameSize;
				std::string jointName;
				jointName.resize(nameSize);
				aSerializer.Serialize(jointName, nameSize);
				mySkeleton.IndexToJointName.emplace(index, jointName);
			}
			for(auto& pair : mySkeleton.IndexToJointName)
			{
				mySkeleton.JointNameToIndex.emplace(pair.second, pair.first);
			}
		}
		else//write
		{
			for(auto& pair : mySkeleton.IndexToJointName)
			{
				aSerializer << pair.first;
				size_t nameSize = pair.second.size();
				aSerializer << nameSize;
				aSerializer.Serialize(pair.second, nameSize);
			}
		}
	}
	else
	{
		mySkeleton = Skeleton();
	}
	//Topology
	aSerializer << myPrimitiveTopology;
	//Bounding Box
	aSerializer << myBoundingBox;
}
