#pragma once
#include <filesystem>
#include <vector>
#include "Component.h"
#include "Utilities/CommonUtilities/AABB3D.hpp"
#include <Graphics/GraphicsEngine/ParticleEmitterSettings.h>
//#include <GraphicsEngine/ParticleSystem.h>
class ParticleEmitter;

#define PARTICLE_EMITTER_COMPONENT 50
class ParticleEmitterComponent : public Component
{
public:
	ParticleEmitterComponent();
	~ParticleEmitterComponent() override;

	void Awake() override;
	void Destroy() override;
	void Update(const float& aDeltaTime) override;
	void Render() override;
	void DebugRender() override;

	void LoadParticles(std::string aName);

	CU::AABB3Df GetBoundingBox();

private:

	bool myHasSettings;
	ParticleEmitter* myParticleEmitter;
	ParticleEmitterSettings myParticleSettings;
};