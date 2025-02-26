#pragma once
#include <filesystem>
#include <vector>
#include "Component.h"
#include "Utilities/CommonUtilities/AABB3D.hpp"

class Material;
class Mesh;

#define MESH_COMPONENT 20000
class MeshComponent : public Component
{
public:
	MeshComponent();
	~MeshComponent() override;

	void Awake() override;
	void Destroy() override;
	void Update(const float& aDeltaTime) override;
	void Render() override;
	void DebugRender() override;

	void SetMesh(const std::filesystem::path& aMeshName);
	void SetMesh(const std::shared_ptr<Mesh>& aMesh);
	std::shared_ptr<Mesh> GetMesh();

	void SetVSShader(std::string_view aShaderName);
	void SetPSShader(std::string_view aShaderName);

	std::string_view GetVertexShader() const;
	std::string_view GetPixelShader() const;

	CU::AABB3Df GetBoundingBox();

	void SetMaterials(const std::vector<std::filesystem::path>& aMaterialNameList);
	void SetMaterials(std::filesystem::path aMaterialName);
	void SetMaterials(std::vector<std::shared_ptr<Material>> aMaterialVector);
	std::vector<std::shared_ptr<Material>> GetMaterials();

	int GetJoint(const std::string& aJointName) const;

	void SetElite();
	void SetEnemy();
	bool IsElite();
private:
	void ResetMaterialListToDefault();
	bool myIsOutlinedA;
	bool myIsOutlinedB;
	float myIsOutlinedC; // Value between 0 and 1 to be able to be rendered on texture
	std::string myVertexShader;
	std::string myPixelShader;
	std::shared_ptr<Mesh> myMesh;
	std::vector<std::shared_ptr<Material>> myMaterials;
};