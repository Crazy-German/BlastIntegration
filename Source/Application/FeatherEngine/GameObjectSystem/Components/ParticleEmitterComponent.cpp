#include "ParticleEmitterComponent.h"
#include <Graphics/GraphicsEngine/ParticleEmitter.h>
#include "MainSingleton.h"
#include <GraphicsEngine/GfxCmd_RenderParticles.h>
#include <Renderer.h>
#include <CommonUtilities/VectorJsonCompatibility.hpp>
#include <GraphicsEngine/GfxCmd_BeginEvent.h>
#include <GraphicsEngine/GfxCmd_EndEvent.h>
#include <AssetManagement/AssetTypes/TextureAsset.h>

ParticleEmitterComponent::ParticleEmitterComponent() = default;

ParticleEmitterComponent::~ParticleEmitterComponent()
{
}

void ParticleEmitterComponent::Awake()
{
}

void ParticleEmitterComponent::Destroy()
{
	myParticleEmitter = nullptr;
	myIsMarkedForDestroy = true;
}

void ParticleEmitterComponent::Update(const float& aDeltaTime)
{
	if (myIsEnabled && myHasSettings)
	{
		myParticleEmitter->Update(aDeltaTime);
	}
}

void ParticleEmitterComponent::Render()
{
	
	if (myIsEnabled && myHasSettings)
	{
		auto& renderer = RENDERER;
		renderer.Enqueue<GfxCmd_RenderParticles>(myParticleEmitter);
	}
}

void ParticleEmitterComponent::DebugRender()
{
}

void ParticleEmitterComponent::LoadParticles(std::string aName)
{
	myParticleEmitter = new ParticleEmitter;
	ParticleEmitterSettings* settings = &MainSingleton::Get().GetParticleLoader().LoadParticleSettings(aName);
	CU::Vector3f position = myGameObject->GetTransform()->GetPosition();
	myParticleEmitter->Init(settings, position, -1.0f);
	myHasSettings = true;
}

CU::AABB3Df ParticleEmitterComponent::GetBoundingBox()
{
	return CU::AABB3Df();
}
