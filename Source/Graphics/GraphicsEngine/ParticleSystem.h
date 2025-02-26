#pragma once
#include "ParticleEmitterSettings.h"

class ParticleEmitter;

class ParticleSystem
{
public:
	virtual ~ParticleSystem();
	void Update(float aDeltaTime);

	std::vector<ParticleEmitter*>* GetEmitters();
	void AddEmitter(ParticleEmitterSettings* aSettings, CU::Vector3f aPosition = { 0.0f, 0.0f, 0.0f }, int aIndex = 0);
	void AddEmitter(ParticleEmitter* aEmitter);
private:
	std::vector<ParticleEmitter*> myEmitters;
};

