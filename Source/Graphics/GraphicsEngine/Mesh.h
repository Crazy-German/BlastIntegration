#pragma once
#include "wrl.h"
#include <vector>
#include "Vertex.h"
#include "Skeleton.h"
#include "CommonUtilities/AABB3D.hpp"
#include "CommonUtilities/Serializable.h"

struct ID3D11Buffer;

class Mesh : public Serializable
{
public:
	struct Element
	{
		unsigned VertexOffset = 0;
		unsigned IndexOffset = 0;
		unsigned NumVertices = 0;
		unsigned NumIndices = 0;
		unsigned MaterialIndex = 0;
	};

	Mesh();
	~Mesh() override;

	void Initialize(std::vector<Vertex>&& aVertexList, std::vector<unsigned>&& aIndexList, std::vector<Element>&& aElementList, Skeleton&& aSkeleton, const std::string& aName);
	void InitializeNoBuffer(std::vector<Vertex>&& aVertexList, std::vector<unsigned>&& aIndexList,
	                        std::vector<Element>&& aElementList, Skeleton&& aSkeleton, const std::string& aName);
	void Initialize(const std::vector<CommonUtilities::Vector3f>& aPosVector, const std::vector<CommonUtilities::Vector3f>& aNormalVector, const std::vector<CommonUtilities::Vector2f>& aUvVector, const std::vector<uint32_t> aIndexVector);
	void GenerateBuffers(const std::string& aName);
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer() const;
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer() const;
	std::vector<Element> GetElements() const;
	Skeleton& GetSkeleton();
	Skeleton& GetSkeletonRef() { return mySkeleton; }
	unsigned GetPrimitiveTopology() const;
	const std::vector<Vertex>& GetVertices() const;
	const std::vector<unsigned>& GetIndices() const;

	//TODO: Move and make static
	void GenerateCube(const float aSize);
	void GenerateCube(const CU::Vector3f aSize);
	void GenerateHorizontalPlane(const float aSize);
	void GenerateVerticalPlane(const float aSize, bool aVisible = false);
	void GenerateSphere(const float aRadius, const float aVertexCount);
	void GenerateTube(const CU::Vector3f& startPos, const CU::Vector3f& endPos, const float radius, const int segments);
	void GenerateCapsule(const float aRadius, float aHeight);
	void GenerateHorizontalTriangle(float aSize);


	CU::AABB3Df& GetBoundingBox();
	void ResizeBoundingBox(CommonUtilities::Vector3f aSize);

	void Serialize(Serializer& aSerializer) override;
	void GenerateBoundingBox();
private:

	std::vector<Vertex> myVertices;
	std::vector<unsigned> myIndices;
	std::vector<Element> myElements;
	Skeleton mySkeleton;

	unsigned myPrimitiveTopology = 0;

	Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> myIndexBuffer;

	CU::AABB3Df myBoundingBox;
	bool myHasBoundingBox = false;
};

