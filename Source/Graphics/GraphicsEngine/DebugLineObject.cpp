#include "GraphicsEngine.pch.h"
#include "DebugLineObject.h"

DebugLineObject::DebugLineObject()
{
}

DebugLineObject::~DebugLineObject()
{
	myVertexBuffer.Reset();
	myIndexBuffer.Reset();
	myIndices.clear();
	myVertices.clear();
}


void DebugLineObject::Initialize(const std::vector<Vertex>& aVertexList, const std::vector<unsigned>& aIndexList, DebugColor aColor, std::string_view aName, bool aFromMesh)
{
	myName = aName;
	myVertices.clear();
	myVertices = aVertexList;
	myIndices.clear();
	myIndices = aIndexList;
	if(aFromMesh) //inefficient
	{
		std::vector<unsigned> myIndexlist;
		for(int index= 0; index<myIndices.size()-3; index+=3)
		{
			myIndexlist.push_back(myIndices[index]);
			myIndexlist.push_back(myIndices[index+1]);
			myIndexlist.push_back(myIndices[index+1]);
			myIndexlist.push_back(myIndices[index+2]);
			myIndexlist.push_back(myIndices[index+2]);
			myIndexlist.push_back(myIndices[index]);
		}
		myIndices.clear();
		myIndices = myIndexlist;
	}
	myNumIndices = static_cast<unsigned>(myIndices.size());
	myNumVertices = static_cast<unsigned>(myVertices.size());
	myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	SetVertexColor(aColor);
	
	myVertexBuffer.Reset();
	myIndexBuffer.Reset();
	GraphicsEngine::Get().CreateVertexBuffer(myName + " vertex buffer", myVertices, myVertexBuffer);
	GraphicsEngine::Get().CreateIndexBuffer(myName + " index buffer", myIndices, myIndexBuffer);
}

void DebugLineObject::InitializeFromPoints(const std::vector<CU::Vector3f>& aPointList, std::vector<unsigned>& anIndexList, DebugColor aColor, std::string_view aName)
{
	myName = aName;
	for (auto& point : aPointList)
	{
		myVertices.emplace_back(point);
	}
	myIndices = anIndexList;
	myNumIndices = static_cast<unsigned>(myIndices.size());
	myNumVertices = static_cast<unsigned>(myVertices.size());
	myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	SetVertexColor(aColor);

	myVertexBuffer.Reset();
	myIndexBuffer.Reset();
	GraphicsEngine::Get().CreateVertexBuffer(myName + " vertex buffer", myVertices, myVertexBuffer);
	GraphicsEngine::Get().CreateIndexBuffer(myName + " index buffer", myIndices, myIndexBuffer);
}

void DebugLineObject::AddLine(const CU::Vector3f& aStart, const CU::Vector3f& anEnd)
{
	myIndices.emplace_back(static_cast<unsigned>(myVertices.size()));
	myVertices.emplace_back(aStart);
	myIndices.emplace_back(static_cast<unsigned>(myVertices.size()));
	myVertices.emplace_back(anEnd);

	myNumIndices = static_cast<unsigned>(myIndices.size());
	myNumVertices = static_cast<unsigned>(myVertices.size());

	if (myVertexBuffer == nullptr || myIndexBuffer == nullptr)
	{
		Init();
	}
}

Microsoft::WRL::ComPtr<ID3D11Buffer> DebugLineObject::GetVertexBuffer() const
{
	return myVertexBuffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> DebugLineObject::GetIndexBuffer() const
{
	return myIndexBuffer;
}

void DebugLineObject::SetTopology(const unsigned& aTopology)
{
	myPrimitiveTopology = aTopology;
}

unsigned DebugLineObject::GetPrimitiveTopology() const
{
	return myPrimitiveTopology;
}

unsigned DebugLineObject::GetNumVertices() const
{
	return myNumVertices;
}

unsigned DebugLineObject::GetNumIndices() const
{
	return myNumIndices;
}


void DebugLineObject::SetName(std::string_view aDebugName)
{
	myName = aDebugName;
}

std::string_view DebugLineObject::GetName() const
{
	return myName;
}

void DebugLineObject::ResetLines()
{
	myVertices.clear();
	myIndices.clear();
	myNumIndices = 0;
	myNumVertices = 0;
	if (myVertexBuffer == nullptr || myIndexBuffer == nullptr)
	{
		Init();
	}
}

void DebugLineObject::Init()
{
	myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

	GraphicsEngine::Get().CreateVertexBuffer(myName + " vertex buffer", myVertices, myVertexBuffer);
	GraphicsEngine::Get().CreateIndexBuffer(myName + " index buffer", myIndices, myIndexBuffer);
}

void DebugLineObject::SetVertexColor(DebugColor aColor)
{
	for (auto& vertex : myVertices)
	{
		switch (aColor)
		{
		case DebugColor::White:
			vertex.Color = { 1.0f, 1.0f, 1.0f, 1.0f };
			break;
		case DebugColor::Red:
			vertex.Color = { 1.0f, 0.0f, 0.0f, 1.0f };
			break;
		case DebugColor::Green:
			vertex.Color = { 0.0f, 1.0f, 0.0f, 1.0f };
			break;
		case DebugColor::Blue:
			vertex.Color = { 0.0f, 0.0f, 1.0f, 1.0f };
			break;
		case DebugColor::Yellow:
			vertex.Color = { 1.0f, 1.0f, 0.0f, 1.0f };
			break;
		case DebugColor::Orange:
			vertex.Color = { 1.0f, 0.3f, 0.0f, 1.0f };
			break;
		case DebugColor::Purple:
			vertex.Color = { 0.3f, 0.0f, 0.8f, 1.0f };
			break;
		case DebugColor::Pink:
			vertex.Color = { 1.0f, 0.0f, 1.0f, 1.0f };
			break;
		default:
			break;
		}
	}
}
