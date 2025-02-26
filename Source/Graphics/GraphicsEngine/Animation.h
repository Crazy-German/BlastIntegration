#pragma once
#include <ranges>
#include <unordered_map>
#include <string>
#include <CommonUtilities/Matrix4x4.hpp>
#include <CommonUtilities/TransformBase.h>
#include <CommonUtilities/Serializer.h>
#include <CommonUtilities/Serializable.h>

enum AnimationType
{
	Base,
	JointOverride,
	Addative,

};

struct Animation : public Serializable
{

	struct Frame : public Serializable
	{
		std::unordered_map<std::string, CU::Matrix4x4f> Transforms; // Swap out to transform for quaternions for rotation
		std::unordered_map<std::string, TransformBase> RealTransforms; // Swap out to transform for quaternions for rotation
		//std::unordered_map<std::string, > TriggeredEvents;
		std::vector<unsigned int> TriggeredEvents;
		
		void Serialize(Serializer& aSerializer) override
		{
			size_t mapSize = Transforms.size();
			aSerializer << mapSize;
			Transforms.reserve(mapSize);
			if (Transforms.empty())//read
			{
				for (size_t nr = 0; nr < mapSize; ++nr)
				{
					size_t keySize = 0;
					aSerializer << keySize;
					std::string key;
					key.resize(keySize);
					aSerializer.Serialize(key, keySize);
					CU::Matrix4x4f matrix;
					aSerializer << matrix;
					Transforms.emplace(key, matrix);
				}
			}
			else//write
			{
				for (auto& pair : Transforms)
				{
					size_t keySize = pair.first.size();
					aSerializer << keySize;
					aSerializer.Serialize(pair.first, keySize);
					aSerializer << pair.second;
				}
			}
		}
	};

	std::vector<Frame> Frames;
	std::unordered_map<std::string, unsigned int> EventNameToIndex;
	std::vector<std::string> EventNames;
	float Duration = 0;
	float myTime = 0;
	float FramesPerSecond = 0;
	bool myIsLooping = false;

	void Serialize(Serializer& aSerializer) override
	{
		size_t frameCount = Frames.size();
		aSerializer << frameCount;
		Frames.resize(frameCount);
		for(size_t f = 0; f<frameCount;++f)
		{
			Frame& frame = Frames[f];
			frame.Serialize(aSerializer);
		}
		aSerializer << Duration;
		aSerializer << FramesPerSecond;
	};
};

