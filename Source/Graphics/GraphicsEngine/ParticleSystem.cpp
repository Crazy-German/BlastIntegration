#include "GraphicsEngine.pch.h"
#include "ParticleSystem.h"
#include "ParticleEmitter.h"

ParticleSystem::~ParticleSystem()
{

}

void ParticleSystem::Update(float aDeltaTime)
{
	int indexToKill = -1;
	for (size_t i = 0; i < myEmitters.size(); i++)
	{
		myEmitters[i]->Update(aDeltaTime);
		if (myEmitters[i]->GetIsDead())
		{
			indexToKill = i;
		}
	}
	if (indexToKill >= 0)
	{
		delete myEmitters[indexToKill];
		myEmitters.erase(myEmitters.begin() + indexToKill);
	}
}

std::vector<ParticleEmitter*>* ParticleSystem::GetEmitters()
{
	return &myEmitters;
}

void ParticleSystem::AddEmitter(ParticleEmitterSettings* aSettings, CU::Vector3f aPosition, int aIndex)
{
	if (myEmitters.size() <= aIndex)
	{
		myEmitters.resize(aIndex + 1);
	}
	ParticleEmitter* funky = new ParticleEmitter(/*aPSO*/);
	funky->Init(aSettings, aPosition);
	myEmitters[aIndex] = funky;
	
	
}

void ParticleSystem::AddEmitter(ParticleEmitter* aEmitter)
{
	myEmitters.push_back(aEmitter);
}
