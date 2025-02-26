#pragma once
#include <CommonUtilities/Vector.hpp>
#include <unordered_map>
#include "Texture.h"
#include <memory>


template<typename T>
class Curve
{
public:

	bool AddKey(float aPosition, const T& aValue)
	{
		//ensure(aPosition >= 0.0f && aPosition <= 1.0f);
		const unsigned realValue = static_cast<unsigned>(aPosition * ourPrecision);

		bool posFound = false;
		for (unsigned pos : myKeyPositions)
		{
			if (pos == realValue)
			{
				posFound = true;
				break;
			}
		}

		if (!posFound)
			myKeyPositions.emplace_back(realValue);

		myKeys[realValue] = aValue;

		return true;
	}

	void Clear()
	{
		myKeyPositions.clear();
		myKeys.clear();
	}

	T Get(float aPosition) const
	{
		const unsigned realValue = static_cast<unsigned>(aPosition * ourPrecision);

		T startValue, endValue;

		for (auto it = myKeyPositions.begin(); it != myKeyPositions.end(); ++it)
		{
			if (*it == realValue)
			{
				return myKeys.at(*it);
			}

			if (*it < realValue)
			{
				if (it + 1 != myKeyPositions.end())
				{
					if (*(it + 1) > realValue)
					{
						const float startPos = static_cast<float>(*it) / ourPrecision;
						const float endPos = static_cast<float>(*(it + 1)) / ourPrecision;

						startValue = myKeys.at(*it);
						endValue = myKeys.at(*(it + 1));

						const float a = Remap(aPosition, startPos, endPos, 0.0f, 1.0f);

						 //TODO: Add your own LERP here.
						return startValue + (endValue - startValue) * a;//T::Lerp(startValue, endValue, a);
					}
				}
				else
				{
					return myKeys.at(*it);
				}
			}
			else if (it == myKeyPositions.begin() && realValue < *it)
			{
				return myKeys.at(*it);
			}
		}

		return T();
	}

private:

	// TODO: Take this for your CU if you don't have it already.
	template<class T>
	static constexpr FORCEINLINE T Remap(const T aValue, const T aInMin, const T aInMax, const T aOutMin, const T aOutMax)
	{
		const float p = (aValue - aInMin) / (aInMax - aInMin);
		const float r = aOutMin + p * (aOutMax - aOutMin);
		return r;
	}

	float ourPrecision = 100000.0f;

	std::vector<unsigned> myKeyPositions;
	std::unordered_map<unsigned, T> myKeys;
};

class ParticleEmitterSettings
{
public:
	enum class EmitterShape
	{
		Point, //Particles are emitted from a single point
		Plane,  //Particles are emitted from a random point within a plane along the XZ axes
		Sphere, //Particles are emitted from a random point within a sphere
		Box		//Particles are emitted form a random point within an axis aligned bounding box
	};
	//Shape in which particles are emitted - additional settings need to be set for anything other than point
	EmitterShape Shape = EmitterShape::Point;
	//EMITTER SHAPE SPHERE ONLY: Radius of emitter volume
	float Radius = 1.0f;
	//EMITTER SHAPE PLANE ONLY: Width and depth of emitter plane
	CU::Vector2f PlaneSize = { 1.0f, 1.0f };
	//EMITTER SHAPE BOX ONLY: Width, height and depth of emitter box
	CU::Vector3f BoxSize = { 1.0f, 1.0f, 1.0f };

	//If particle positions are always relative to emitter position (e.g. move with the emitter when moved). Set to false to let particles move independently of the emitter.
	bool AnchorToEmitter = true;
	//If emitter loops or only emits one burst (no new particles once limit has been reached) of particles
	bool Looping = true;
	//Max particles: only applicable if not looping
	int MaxParticles;
	//Particles spawned per second
	float SpawnRate = 100.0f;
	//Color over lifespan
	Curve<CU::Vector4f> Color;
	//Size over lifespan
	Curve<CU::Vector2f> Size;
	//Spawn velocity 
	CU::Vector3f Velocity = { 0.0f, 0.0f, 0.0f };
	//Lifetime in seconds
	float LifeTime = 20.0f;
	//Effect of "gravity" in any axis
	Curve<CU::Vector3f> Gravity;
	//Rotation speed (in degrees per second) over lifetime
	Curve<float> Spin;
	//Minimum starting rotation (-1.0 to 1.0)
	float MinSpawnSpin = 0.0f;
	//Maximum starting rotation (-1.0 to 1.0)
	float MaxSpawnSpin = 0.0f;
	//Used channels of texture
	CU::Vector4f channelMask = { 1,1,1,1 };
	//Particle texture
	std::shared_ptr<Texture> Texture;
	//Minimum spawn angle (-1.0 to 1.0) for full circle
	CU::Vector3f MinSpawnAngle = { 0.0f, 0.0f, 0.0f };
	//Maximum spawn angle (-1.0 to 1.0) for full circle
	CU::Vector3f MaxSpawnAngle = { 0.0f, 0.0f, 0.0f };
	//Acceleration over lifetime
	Curve<CU::Vector3f> Acceleration;
	//Minimum spawn velocity multiplier
	CU::Vector3f MinSpawnSpeedMult = { 1.0f, 1.0f, 1.0f };
	//Maximum spawn velocity multiplier
	CU::Vector3f MaxSpawnSpeedMult = { 1.0f, 1.0f, 1.0f };

	//Spawn chance where 0 is no chance and 1.0 is 100% chance
	float SpawnChance = 1.0f;
	
	CU::Vector2f MirrorChance = { 0.0f, 0.0f };

	//std::unordered_map<unsigned, std::shared_ptr<Texture>> textures;

	ParticleEmitterSettings();
	virtual ~ParticleEmitterSettings();

};

