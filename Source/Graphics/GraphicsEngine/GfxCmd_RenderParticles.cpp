#include "GfxCmd_RenderParticles.h"
#include <ObjectBuffer.h>
#include "ParticleEmitter.h"

GfxCmd_RenderParticles::GfxCmd_RenderParticles(ParticleEmitter* aParticleSystem)
{
	particleSystem = aParticleSystem;
}

GfxCmd_RenderParticles::~GfxCmd_RenderParticles()
{
}

void GfxCmd_RenderParticles::Execute()
{
	auto& ge = GraphicsEngine::Get();

	ParticleEmitter* emitter = particleSystem;
	const ParticleEmitterSettings* settings = emitter->GetSettings();

	ObjectBuffer ob;
	ob.HasSkinning = false;
	if (settings->AnchorToEmitter)
	{
		ob.World(4, 1) = emitter->GetPosition().x; ob.World(4, 2) = emitter->GetPosition().y; ob.World(4, 3) = emitter->GetPosition().z; ob.World(4, 4) = 1;
	}
	ge.ChangePipelineState(*ge.GetParticlePSO());

	ge.SetVertexShader("Particle_VS");
	ge.SetGeometryShader("Quad_GS");
	ge.SetPixelShader("Particle_PS");

	ge.UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, ob);
	ge.SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 0, *settings->Texture);
	ge.PrepareParticleEmitter(emitter);
		
	ge.SetIndexBuffer(nullptr);
	ge.SetVertexBuffer(emitter->GetVertexBuffer(), sizeof(ParticleVertex)); //vertex offset???????????????????????
	ge.RenderPoints(static_cast<unsigned>(emitter->GetParticleAmount()));
	
	ge.SetGeometryShader("Empty");
}

void GfxCmd_RenderParticles::Destroy()
{
}
