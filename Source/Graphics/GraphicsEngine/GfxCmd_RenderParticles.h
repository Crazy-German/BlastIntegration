#include "GraphicsCommandList.h"

class ParticleEmitter;

class GfxCmd_RenderParticles : public GraphicsCommandBase
{
public:
	GfxCmd_RenderParticles(ParticleEmitter* aParticleSystem);

	~GfxCmd_RenderParticles();

	void Execute();

	void Destroy();
private:
	ParticleEmitter* particleSystem;

};
