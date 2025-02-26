#pragma once
#include <filesystem>
#include <vector>
#include "Component.h"
#include "Utilities/CommonUtilities/AABB3D.hpp"
//#include <GraphicsEngine/ParticleSystem.h>
class ParticleSystem;

#define PARTICLE_SYSTEM_COMPONENT 1
class ParticleSystemComponent : public Component
{
public:
	ParticleSystemComponent();
	~ParticleSystemComponent() override;
	
	void Put(Letter aLetter) override;
	void Awake() override;
	void Destroy() override;
	void Update(const float& aDeltaTime) override;
	void Render() override;
	void DebugRender() override;

	CU::AABB3Df GetBoundingBox();

private:
	ParticleSystem* myParticleSystem = nullptr;
};