#pragma once
#include <unordered_map>
#include <vector>
#include <CommonUtilities/Matrix4x4.hpp>

struct Skeleton
{
	struct Joint
	{
		CU::Matrix4x4f BindPoseInverse;
		int Parent {};
		std::vector<unsigned> Children;
		std::string Name;
	};

	std::vector<Joint> Joints;
	std::unordered_map<std::string, unsigned> JointNameToIndex;
	std::unordered_map<unsigned, std::string> IndexToJointName;
};