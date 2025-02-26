#pragma once
#include "Vector.hpp"
#include "../../External/nlohmann/json.hpp"
//nlohman jsons it
using json = nlohmann::json;
namespace CommonUtilities 
{
	template <class T>
	void to_json(json& j, const Vector2<T>& aVector)
	{
		j = json{ {"x", aVector.x}, {"y", aVector.y} };
	}

	template <class T>
	void from_json(const json& j, Vector2<T>& aVector)
	{
		j.at("x").get_to(aVector.x);
		j.at("y").get_to(aVector.y);
	}

	template <class T>
	void to_json(json& j, const Vector3<T>& aVector)
	{
		j = json{ {"x", aVector.x}, {"y", aVector.y}, {"z", aVector.z} };
	}

	template <class T>
	void from_json(const json& j, Vector3<T>& aVector)
	{
		j.at("x").get_to(aVector.x);
		j.at("y").get_to(aVector.y);
		j.at("z").get_to(aVector.z);
	}

	template <class T>
	void to_json(json& j, const Vector4<T>& aVector)
	{
		j = json{ {"x", aVector.x}, {"y", aVector.y}, {"z", aVector.z}, {"w", aVector.w} };
	}

	template <class T>
	void from_json(const json& j, Vector4<T>& aVector)
	{
		j.at("x").get_to(aVector.x);
		j.at("y").get_to(aVector.y);
		j.at("z").get_to(aVector.z);
		j.at("w").get_to(aVector.w);
	}
}
