#pragma once
#include "Vector3.hpp"
#include "CommonMath.hpp"
#include <array>

namespace CommonUtilities
{
	template <class T>
	class AABB3D
	{
	public:
		AABB3D();
		AABB3D(const Vector3<T>& aMin, const Vector3<T>& aMax);
		AABB3D(const AABB3D<T>& aAABB3D);

		void InitWithMinAndMax(const Vector3<T>& aMin, const Vector3<T>& aMax);
		void InitWithPositionAndHalfSize(const Vector3<T>& aPosition, const Vector3<T>& aHalfSize);
		bool IsInsideMeshCell(const Vector3<T>& aPosition) const;
		void SetCenterPoistion(const Vector3<T>& aPosition);

		const Vector3<T> GetMin() const;
		const Vector3<T> GetMax() const;
		const Vector3<T> GetCenterPoistion() const;
		const Vector3<T> GetClosestPoint(const CommonUtilities::Vector3<T>& aPoint) const;
		const std::array<Vector3<T>, 8> GetVerticies() const;
		const Interval<T> GetInterval(Vector3<T>& aAxis) const;


		Vector3<T>& GetMinRef();
		Vector3<T>& GetMaxRef();
	private:
		Vector3<T> myMin;
		Vector3<T> myMax;
	};

	template<class T>
	inline AABB3D<T>::AABB3D()
	{
		myMin = Vector3<T>();
		myMax = Vector3<T>();
	}

	template<class T>
	inline AABB3D<T>::AABB3D(const Vector3<T>& aMin, const Vector3<T>& aMax)
	{
		myMin = aMin;
		myMax = aMax;
	}

	template<class T>
	inline AABB3D<T>::AABB3D(const AABB3D<T>& aAABB3D)
	{
		myMin = aAABB3D.myMin;
		myMax = aAABB3D.myMax;
	}

	template<class T>
	inline void AABB3D<T>::InitWithMinAndMax(const Vector3<T>& aMin, const Vector3<T>& aMax)
	{
		myMin = aMin;
		myMax = aMax;
	}

	template<class T>
	inline void AABB3D<T>::InitWithPositionAndHalfSize(const Vector3<T>& aPosition, const Vector3<T>& aHalfSize)
	{
		myMin = aPosition - aHalfSize;
		myMax = aPosition + aHalfSize;
	}

	template<class T>
	inline bool AABB3D<T>::IsInsideMeshCell(const Vector3<T>& aPosition) const
	{
		if (myMin.x <= aPosition.x && myMax.x >= aPosition.x &&
			myMin.y <= aPosition.y && myMax.y >= aPosition.y &&
			myMin.z <= aPosition.z && myMax.z >= aPosition.z)
		{
			return true;
		}
		return false;
	}

	template<class T>
	inline void AABB3D<T>::SetCenterPoistion(const Vector3<T>& aPosition)
	{
		Vector3<T> halfSize = (myMax - myMin) / static_cast<T>(2);
		myMin = aPosition - halfSize;
		myMax = aPosition + halfSize;
	}

	template<class T>
	inline const Vector3<T> AABB3D<T>::GetMin() const
	{
		return myMin;
	}

	template<class T>
	inline const Vector3<T> AABB3D<T>::GetMax() const
	{
		return myMax;
	}

	template<class T>
	inline const Vector3<T> AABB3D<T>::GetCenterPoistion() const
	{
		return myMax - ((myMax - myMin) / static_cast<T>(2));
	}

	template <class T>
	const Vector3<T> AABB3D<T>::GetClosestPoint(const CommonUtilities::Vector3<T>& aPoint) const
	{
		return{ max(myMin.x, min(aPoint.x, myMax.x)), max(myMin.y, min(aPoint.y, myMax.y)) , max(myMin.z, min(aPoint.z, myMax.z)) };
	}

	template<class T>
	inline const Interval<T> AABB3D<T>::GetInterval(Vector3<T>& aAxis) const
	{
		Interval<T> returnValue;
		const std::array<Vector3<T>, 8> vertices = GetVerticies();
		returnValue.min = aAxis.Dot(vertices[0]);
		returnValue.max = returnValue.min;

		for (int i = 0; i < 8; i++)
		{
			T dot = aAxis.Dot(vertices[i]);
			returnValue.min = min(returnValue.min, dot);
			returnValue.max = max(returnValue.max, dot);
		}
		return returnValue;
	}

	template <class T>
	Vector3<T>& AABB3D<T>::GetMinRef()
	{
		return myMin;
	}

	template <class T>
	Vector3<T>& AABB3D<T>::GetMaxRef()
	{
		return myMax;
	}

	template<class T>
	inline const std::array<Vector3<T>, 8> AABB3D<T>::GetVerticies() const
	{
		const std::array<Vector3<T>, 8> vertices =
		{
			Vector3<T>(myMin.x, myMax.y, myMax.z),
			Vector3<T>(myMin.x, myMax.y, myMin.z),
			Vector3<T>(myMin.x, myMin.y, myMax.z),
			Vector3<T>(myMin.x, myMin.y, myMin.z),
			Vector3<T>(myMax.x, myMax.y, myMax.z),
			Vector3<T>(myMax.x, myMax.y, myMin.z),
			Vector3<T>(myMax.x, myMin.y, myMax.z),
			Vector3<T>(myMax.x, myMin.y, myMin.z)
		};
		return vertices;
	}
	typedef AABB3D<float> AABB3Df;
}
namespace CU = CommonUtilities;