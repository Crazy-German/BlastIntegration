#include "GraphicsEngine.pch.h"
#include "ParticleEmitter.h"
#include <d3d11.h>

std::mt19937 ParticleEmitter::ourRandomEngine;
std::uniform_real_distribution<float> ParticleEmitter::ourRandomDistribution(-1.0f, 1.0f);

ParticleEmitter::ParticleEmitter(/*PipelineStateObject& aPSO*/) /*: myPSO(aPSO)*/
{
	myVertexBuffer = nullptr;
	myPosition = { 0.0f,0.0f,0.0f, 1.0f };
	ourRandomEngine.seed(451); //magic number
	myParent = nullptr;
	myAmDying = false;
	myAmDead = false;
}

ParticleEmitter::~ParticleEmitter()
{
}

void ParticleEmitter::Init(ParticleEmitterSettings* aSettings, CU::Vector3f& aPosition, float aLifeTime)
{
	mySettings = aSettings;
	myPosition = (aPosition.ToVector4One());
	myPosition.w = 1.0f;
	myLifeTime = aLifeTime;

	UpdateTransform();
	InitInternal();
}

void ParticleEmitter::Init(ParticleEmitterSettings* aSettings, CU::Matrix4x4f& aTransform, float aLifeTime)
{
	mySettings = aSettings;
	myTransform = aTransform;
	myLifeTime = aLifeTime;
	InitInternal();
}

void ParticleEmitter::SetParent(CU::Matrix4x4f* aParentTransform)
{
	myParent = aParentTransform;
}

void ParticleEmitter::SetPosition(CU::Vector3f aPosition)
{
	myPosition = (aPosition.ToVector4One());
	myPosition.w = 1.0f;
	myLocalTransformChanged = true;
}

void ParticleEmitter::SetRotation(CU::Vector3f aRotation)
{
	myRotation = aRotation;
	myLocalTransformChanged = true;
}

CU::Vector3f ParticleEmitter::GetPosition()
{
	CU::Vector3f skibidi = { myPosition.x, myPosition.y, myPosition.z };
	return skibidi;
}

CU::Vector3f ParticleEmitter::GetRotation()
{
	return myRotation;
}

void ParticleEmitter::Update(float aDeltaTime)
{
	if (!myAmDead)
	{
		unsigned liveParticles = 0;
		if (myLifeTime != -1.0f)
		{
			myLifeTime -= aDeltaTime;
			if (myLifeTime <= 0.0f)
			{
				myAmDying = true;
			}
		}
		for (size_t i = 0; i < myParticles.size(); ++i)
		{
			myParticles[i].lifetime += aDeltaTime;
			if (myParticles[i].lifetime < 0.0f)
			{
				continue;
			}
			else if (myParticles[i].lifetime >= mySettings->LifeTime && mySettings->Looping)
			{
				if (!myAmDying)
				{
					InitParticle(myParticles[i], i);
				}
			}
			liveParticles++;
			UpdateParticle(myParticles[i], i, aDeltaTime);
		}

		if (liveParticles == 0 && myAmDying)
		{
			myAmDead = true;
		}

		UpdateTransform();
	}
}

void ParticleEmitter::Burst()
{
	InitInternal();
}

ParticleEmitterSettings* ParticleEmitter::GetSettings()
{
	return mySettings;
}

CU::Matrix4x4f* ParticleEmitter::GetTransform()
{
	return &myTransform;
}

const bool ParticleEmitter::GetIsDead() const
{
	return myAmDead;
}

const size_t ParticleEmitter::GetParticleAmount() const
{
	return myParticles.size();
}

void ParticleEmitter::UpdateTransform()
{
	if (myLocalTransformChanged)
	{
		CU::Matrix4x4<float> translation;
		CU::Matrix4x4<float> rotation;
		CU::Matrix4x4<float> scale;

		translation(4, 1) = myPosition.x; translation(4, 2) = myPosition.y; translation(4, 3) = myPosition.z; translation(4, 4) = 1;

		rotation = rotation.CreateRotationAroundZ(myRotation.z) * rotation.CreateRotationAroundX(myRotation.x) * rotation.CreateRotationAroundY(myRotation.y);

		scale(1, 1) = 1; scale(2, 2) = 1; scale(3, 3) = 1;

		myLocalTransform = scale * rotation * translation;
		myLocalTransformChanged = false;
	}

	if (myParent != nullptr)
	{
		myTransform = myLocalTransform * *myParent;
	}
	else 
	{
		myTransform = myLocalTransform;
	}
}

void ParticleEmitter::InitParticle(ParticleVertex& aVertex, size_t aIndex)
{
	const float spawnRateInverse = 1.0f / mySettings->SpawnRate;
 	aVertex.lifetime = 0.0f - static_cast<float>(aIndex) * spawnRateInverse;
	aVertex.channelmask = mySettings->channelMask;

	if (mySettings->SpawnChance == 0.0f)
	{
		aVertex.channelmask = { 0,0,0,0 };
		return;
	}
	if (mySettings->SpawnChance > 0.0f && mySettings->SpawnChance < 1.0f)
	{
		
		ourRandomDistribution = std::uniform_real_distribution<float>(0.0f, 1.0f);

		float chance = ourRandomDistribution(ourRandomEngine);
		if (mySettings->SpawnChance < chance)
		{
			aVertex.channelmask = { 0,0,0,0 };
			return;
		}
	}

	if (mySettings->MirrorChance.x > 0.0f)
	{
		if (mySettings->MirrorChance.x == 1.0f)
		{
			aVertex.mirrorX = true;
		}

		ourRandomDistribution = std::uniform_real_distribution<float>(0.0f, 1.0f);

		float chance = ourRandomDistribution(ourRandomEngine);
		if (mySettings->MirrorChance.x < chance)
		{
			aVertex.mirrorX = true;
		}
	}

	if (mySettings->MirrorChance.y > 0.0f)
	{
		if (mySettings->MirrorChance.y == 1.0f)
		{
			aVertex.mirrorY = true;
		}

		ourRandomDistribution = std::uniform_real_distribution<float>(0.0f, 1.0f);

		float chance = ourRandomDistribution(ourRandomEngine);
		if (mySettings->MirrorChance.y < chance)
		{
			aVertex.mirrorY = true;
		}
	}
	

	

	switch (mySettings->Shape)
	{
	case ParticleEmitterSettings::EmitterShape::Point:
		aVertex.position = myPosition;
		break;
	case ParticleEmitterSettings::EmitterShape::Plane:
	{
		ourRandomDistribution = std::uniform_real_distribution<float>(-1.0f, 1.0f);
		float x, y, z;
		y = 0;

		x = ourRandomDistribution(ourRandomEngine) * ((mySettings->PlaneSize.x) * 0.5f);
		z = ourRandomDistribution(ourRandomEngine) * ((mySettings->PlaneSize.y) * 0.5f);

		aVertex.position = myPosition + CU::Vector4f(x, y, z, 0);
	}
		break;
	case ParticleEmitterSettings::EmitterShape::Sphere:
		{
		ourRandomDistribution = std::uniform_real_distribution<float>(-1.0f, 1.0f);
			float distance = mySettings->Radius + mySettings->Radius;
			float x, y, z;
			do
			{
				x = ourRandomDistribution(ourRandomEngine) * mySettings->Radius;
				y = ourRandomDistribution(ourRandomEngine) * mySettings->Radius;
				z = ourRandomDistribution(ourRandomEngine) * mySettings->Radius;
				distance = x * x + y * y + z * z;
			} while (distance > mySettings->Radius * mySettings->Radius);
			aVertex.position = myPosition + CU::Vector4f(x, y, z, 0);
		}
		
		break;
	case ParticleEmitterSettings::EmitterShape::Box:
		ourRandomDistribution = std::uniform_real_distribution<float>(-1.0f, 1.0f);
		float x, y, z;

		x = ourRandomDistribution(ourRandomEngine) * ((mySettings->BoxSize.x) * 0.5f);
		y = ourRandomDistribution(ourRandomEngine) * ((mySettings->BoxSize.y) * 0.5f);
		z = ourRandomDistribution(ourRandomEngine) * ((mySettings->BoxSize.z) * 0.5f);

		aVertex.position = myPosition + CU::Vector4f(x, y, z, 0);
		break;
	}
	
	ourRandomDistribution = std::uniform_real_distribution<float>(-1.0f, 1.0f);
	
	float x = ourRandomDistribution(ourRandomEngine);
	float y = ourRandomDistribution(ourRandomEngine);
	float z = ourRandomDistribution(ourRandomEngine);

	CU::Vector3f randomAngleStart =	{	std::clamp(x, mySettings->MinSpawnAngle.x, mySettings->MaxSpawnAngle.x),
										std::clamp(y, mySettings->MinSpawnAngle.y, mySettings->MaxSpawnAngle.y),
										std::clamp(z, mySettings->MinSpawnAngle.z, mySettings->MaxSpawnAngle.z) };
	randomAngleStart.Normalize();


	ourRandomDistribution = std::uniform_real_distribution<float>(mySettings->MinSpawnSpeedMult.x, mySettings->MaxSpawnSpeedMult.x);
	x = ourRandomDistribution(ourRandomEngine);
	ourRandomDistribution = std::uniform_real_distribution<float>(mySettings->MinSpawnSpeedMult.y, mySettings->MaxSpawnSpeedMult.y);
	y = ourRandomDistribution(ourRandomEngine);
	ourRandomDistribution = std::uniform_real_distribution<float>(mySettings->MinSpawnSpeedMult.z, mySettings->MaxSpawnSpeedMult.z);
	z = ourRandomDistribution(ourRandomEngine);

	CU::Vector3f randomVelocityStart = { x,y,z };

	aVertex.velocity = mySettings->Velocity * randomAngleStart * randomVelocityStart;
	aVertex.angle = std::clamp(ourRandomDistribution(ourRandomEngine), mySettings->MinSpawnSpin, mySettings->MaxSpawnSpin) * 360.f;
}

void ParticleEmitter::UpdateParticle(ParticleVertex& aParticle, size_t aIndex, float aDeltaTime)
{
	aIndex;
	float t = aParticle.lifetime / mySettings->LifeTime;
	
	CU::Vector3f gravityModifier = mySettings->Gravity.Get(t);

	aParticle.velocity += CU::Vector3f(-9.81f, -9.81f, -9.81f) * gravityModifier * aDeltaTime;
	aParticle.angle += std::clamp(mySettings->Spin.Get(t) * aDeltaTime, -359.8f, 359.8f);
	while (aParticle.angle >= 360.0f)
	{
		aParticle.angle -= 359.9f;
	}
	while (aParticle.angle < 0.0f)
	{
		aParticle.angle += 359.9f;
	}
	aParticle.position += CU::Vector4f(aParticle.velocity.x, aParticle.velocity.y, aParticle.velocity.z, 0) * aDeltaTime;
	aParticle.size = mySettings->Size.Get(t);
	aParticle.color = mySettings->Color.Get(t);
	//aParticle.velocity = mySettings->Velocity.Get(t);
	
}

void ParticleEmitter::InitInternal()
{
	if (mySettings->Looping)
	{
		const size_t particleAmount = static_cast<size_t>(ceilf(mySettings->SpawnRate * mySettings->LifeTime));
		myParticles.resize(particleAmount);
		for (size_t i = 0; i < particleAmount; ++i)
		{
			ParticleVertex& p = myParticles[i];
			InitParticle(p, i);
		}
		GraphicsEngine::Get().CreateVertexBuffer("particleVxBuffer", myParticles, myVertexBuffer, true);
	}
	else
	{
		const size_t particleAmount = static_cast<size_t>(mySettings->MaxParticles);
		myParticles.resize(particleAmount);
		for (size_t i = 0; i < particleAmount; ++i)
		{
			ParticleVertex& p = myParticles[i];
			InitParticle(p, i);
		}
		GraphicsEngine::Get().CreateVertexBuffer("particleVxBuffer", myParticles, myVertexBuffer, true);
	}
}
