#pragma once
#include "Vector3.hpp"

namespace CommonUtilities
{
	template <class T>
	class Sphere
	{
	public:
		// Default constructor: there is no sphere, the radius is zero and the position is
		// the zero vector.
		Sphere();
		// Copy constructor.
		Sphere(const Sphere<T>& aSphere);
		// Constructor that takes the center position and radius of the sphere.
		Sphere(const Vector3<T>& aCenter, T aRadius);
		// Init the sphere with a center and a radius, the same as the constructor above.
		void InitWithCenterAndRadius(const Vector3<T>& aCenter, T aRadius);
		// Returns whether a point is inside the sphere: it is inside when the point is on the
		// sphere surface or inside of the sphere.
		bool IsInsideMeshCell(const Vector3<T>& aPosition) const;

		void SetPosition(const Vector3<T>& aPosition);

		T GetRadius() const;
		Vector3<T> GetPosition() const;

	private:
		T myRadius;
		Vector3<T> myPosition;
	};

	template<class T>
	inline CommonUtilities::Sphere<T>::Sphere()
	{
		myRadius = 0;
		myPosition = Vector3<T>();
	}

	template<class T>
	inline Sphere<T>::Sphere(const Sphere<T>& aSphere)
	{
		myRadius = aSphere.myRadius;
		myPosition = aSphere.myPosition;
	}

	template<class T>
	inline Sphere<T>::Sphere(const Vector3<T>& aCenter, T aRadius)
	{
		myRadius = aRadius;
		myPosition = aCenter;
	}

	template<class T>
	inline void Sphere<T>::InitWithCenterAndRadius(const Vector3<T>& aCenter, T aRadius)
	{
		myRadius = aRadius;
		myPosition = aCenter;
	}

	template<class T>
	inline bool Sphere<T>::IsInsideMeshCell(const Vector3<T>& aPosition) const
	{
		if ((aPosition - myPosition).LengthSqr() <= myRadius * myRadius)
		{
			return true;
		}
		return false;
	}

	template<class T>
	inline void Sphere<T>::SetPosition(const Vector3<T>& aPosition)
	{
		myPosition = aPosition;
	}

	template<class T>
	inline T Sphere<T>::GetRadius() const
	{
		return myRadius;
	}

	template<class T>
	inline Vector3<T> Sphere<T>::GetPosition() const
	{
		return myPosition;
	}
}