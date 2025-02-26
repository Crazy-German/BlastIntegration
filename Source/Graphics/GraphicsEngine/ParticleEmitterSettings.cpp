#include "GraphicsEngine.pch.h"
#include "ParticleEmitterSettings.h"

ParticleEmitterSettings::ParticleEmitterSettings()
{
	Color.AddKey(0.0f, { 1,1,1,1 });
	Color.AddKey(1.0f, { 1,1,1,1 });

	Size.AddKey(0.0f, { 1.0f, 1.0f });
	Size.AddKey(1.0f, { 1.0f, 1.0f });

	Gravity.AddKey(0.0f, { 0.0f, 0.0f, 0.0f });
	Gravity.AddKey(1.0f, { 0.0f, 0.0f, 0.0f });

	Acceleration.AddKey(0.0f, { 1.0f, 1.0f, 1.0f });
	Acceleration.AddKey(1.0f, { 1.0f, 1.0f, 1.0f });

	Spin.AddKey(0.0f, 0.0f);
	Spin.AddKey(1.0f, 0.0f);

}

ParticleEmitterSettings::~ParticleEmitterSettings()
{
}
