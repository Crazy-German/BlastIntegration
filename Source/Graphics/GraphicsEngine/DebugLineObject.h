#pragma once

#include "wrl.h"
#include <vector>
#include "Vertex.h"
#include "DebugColor.h"

struct ID3D11Buffer;


class DebugLineObject
{
public:
	DebugLineObject();
	~DebugLineObject();

	void Initialize(const std::vector<Vertex>& aVertexList,const std::vector<unsigned>& aIndexList, DebugColor aColor = DebugColor::Red, std::string_view aName = "", bool aFromMesh = false);
	void InitializeFromPoints(const std::vector<CU::Vector3f>& aPointList, std::vector<unsigned>& anIndexList, DebugColor aColor = DebugColor::Red, std::string_view aName = "");

	void AddLine(const CU::Vector3f& aStart, const CU::Vector3f& anEnd);

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer() const;
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer() const;
	void SetTopology(const unsigned& aTopology);
	unsigned GetPrimitiveTopology() const;
	unsigned GetNumVertices() const;
	unsigned GetNumIndices()const;

	void SetName(std::string_view aDebugName);
	std::string_view GetName() const;
	void ResetLines();
private:
	void Init();
	void SetVertexColor(DebugColor aColor);

private:
	std::string myName = "";
	unsigned myNumVertices = 0;
	unsigned myNumIndices = 0;
	std::vector<Vertex> myVertices;
	std::vector<unsigned> myIndices;
	unsigned myPrimitiveTopology = 0;

	Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> myIndexBuffer;
};
