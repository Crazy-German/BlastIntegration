#include "ParticleSystemComponent.h"
#include <Graphics/GraphicsEngine/ParticleSystem.h>
#include <Graphics/GraphicsEngine/ParticleEmitter.h>
#include <Renderer.h>
#include "MainSingleton.h"
#include <GraphicsEngine/GfxCmd_RenderParticles.h>
#include "ParticleMessageStruct.h"

ParticleSystemComponent::ParticleSystemComponent()
{
	myParticleSystem = new ParticleSystem;
}

ParticleSystemComponent::~ParticleSystemComponent()
{
	
}

void ParticleSystemComponent::Put(Letter aLetter)
{
	ParticleMessage message = *static_cast<ParticleMessage*>(aLetter.myData);

	ParticleEmitter* emitter = new ParticleEmitter;
	emitter->Init(&MainSingleton::Get().GetParticleLoader().LoadParticleSettings(message.Name), message.Position, message.Lifetime);
	
	myParticleSystem->AddEmitter(emitter);
}

void ParticleSystemComponent::Awake()
{
}

void ParticleSystemComponent::Destroy()
{
	delete myParticleSystem;
}

void ParticleSystemComponent::Update(const float& aDeltaTime)
{
	myParticleSystem->Update(aDeltaTime);
	//should also sort out emitters based on lifetime and delete
}

void ParticleSystemComponent::Render()
{
	//Rendering
	for (int i = 0; i < myParticleSystem->GetEmitters()->size(); i++)
	{
		auto& renderer = RENDERER;
		renderer.Enqueue<GfxCmd_RenderParticles>(myParticleSystem->GetEmitters()->at(i));
	}
}

void ParticleSystemComponent::DebugRender()
{
}

CU::AABB3Df ParticleSystemComponent::GetBoundingBox()
{
	return CU::AABB3Df();
}
