#pragma once
#include <array>

#include "Vector3.hpp"

namespace CommonUtilities
{
	template<class T>
	class Triangle
	{
	public:
		Triangle() = default;
		Triangle(const Triangle<T>& aTriangle);
		Triangle(const Vector3<T>& aFirstPoint, const  Vector3<T>& aSecondPoint, const  Vector3<T>& aThirdPoint);
		void InitWithPoints(const  Vector3<T>& aFirstPoint, const  Vector3<T>& aSecondPoint, const  Vector3<T>& aThirdPoint);
		const std::array< Vector3<T>,3>& GetPoints() const;
	private:
		std::array< Vector3<T>, 3> myPoints;
	};

	template <class T>
	Triangle<T>::Triangle(const Triangle<T>& aTriangle)
	{
		for(int i = 0; i<3; i++)
		{
			myPoints[i] = aTriangle.myPoints[i];
		}
	}

	template <class T>
	Triangle<T>::Triangle(const  Vector3<T>& aFirstPoint, const  Vector3<T>& aSecondPoint, const  Vector3<T>& aThirdPoint)
	{
		myPoints[0] = aFirstPoint;
		myPoints[1] = aSecondPoint;
		myPoints[2] = aThirdPoint;
	}

	template <class T>
	void Triangle<T>::InitWithPoints(const  Vector3<T>& aFirstPoint, const  Vector3<T>& aSecondPoint, const  Vector3<T>& aThirdPoint)
	{
		myPoints[0] = aFirstPoint;
		myPoints[1] = aSecondPoint;
		myPoints[2] = aThirdPoint;
	}

	template <class T>
	const std::array<Vector3<T>, 3>& Triangle<T>::GetPoints() const
	{
		return myPoints;
	}
}
