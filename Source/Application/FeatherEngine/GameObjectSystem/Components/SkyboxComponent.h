#pragma once
#include <filesystem>
#include <vector>
#include "Component.h"
#include "Utilities/CommonUtilities/AABB3D.hpp"

class Material;
class Mesh;

#define SKYBOX_COMPONENT 1
class SkyboxComponent : public Component
{
public:
	SkyboxComponent();
	~SkyboxComponent() override;

	void Awake() override;
	void Destroy() override;
	void Update(const float& aDeltaTime) override;
	void Render() override;

	void SetMesh(const std::filesystem::path& aMeshName);
	void SetMesh(const std::shared_ptr<Mesh>& aMesh);
	std::shared_ptr<Mesh> GetMesh();

	CU::AABB3Df GetBoundingBox();
private:
	std::shared_ptr<Mesh> myMesh;
};