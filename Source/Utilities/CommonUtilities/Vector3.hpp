#pragma once
#include "Vector4.hpp"
#include <cassert>
#include <cmath>

namespace CommonUtilities
{
	template<class T>
	class Vector2;

	template <class T>
	class Vector3
	{
	public:
		T x;
		T y;
		T z;

		//Creates a null-vector
		Vector3<T>();

		//Creates a vector (aX, aY, aZ)
		Vector3<T>(const T& aX, const T& aY, const T& aZ);

		//Copy constructor (compiler generated)
		Vector3<T>(const Vector3<T>& aVector) = default;

		//Assignment operator (compiler generated)
		Vector3<T>& operator=(const Vector3<T>& aVector3) = default;

		//ITerator operators
		inline T& operator[](const int anIndex);
		inline const T& operator[](const int anIndex) const;

		//Destructor (compiler generated)
		~Vector3<T>() = default;

		//Explicit Type operator, create a different vector with the same values.
		//Example creates a Tga::Vector3<T> from this CommonUtillities::Vector3<T>
		template <class OtherVectorClass>
		explicit operator OtherVectorClass() const;

		//Returns a negated copy of the vector
		Vector3<T> operator-() const;
		bool operator<(const Vector3<T>& aVector3) const;

		//Returns the squared length of the vector
		T LengthSqr() const;

		//Returns the length of the vector
		T Length() const;

		//Returns a normalized copy of this vector
		Vector3<T> GetNormalized() const;

		//Normalizes the vector
		void Normalize();
		static Vector3<T> Lerp(Vector3<T> aVector1, Vector3<T> aVector2, float aTime);
		//Returns the dot product of this and aVector
		T Dot(const Vector3<T>& aVector) const;

		//Returns the cross product of this and aVector
		Vector3<T> Cross(const Vector3<T>& aVector) const;

		Vector2<T> ToVector2() const;
		Vector4<T> ToVector4Zero() const;
		Vector4<T> ToVector4One() const;

		T* GetArray();
	};
	
	template<class T>
	template<class OtherVectorClass>
	inline Vector3<T>::operator OtherVectorClass() const
	{
		return OtherVectorClass(x, y, z);
	}

	template<class T>
	inline CommonUtilities::Vector3<T>::Vector3()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	template<class T>
	inline CommonUtilities::Vector3<T>::Vector3(const T& aX, const T& aY, const T& aZ)
	{
		x = aX;
		y = aY;
		z = aZ;
	}

	template<class T>
	inline Vector3<T> CommonUtilities::Vector3<T>::operator-() const
	{
		Vector3<T> negative;
		negative.x = -x;
		negative.y = -y;
		negative.z = -z;

		return negative;
	}

	template <class T>
	bool Vector3<T>::operator<(const Vector3<T>& aVector3) const
	{
			if (x != aVector3.x) return x < aVector3.x;
			if (y != aVector3.y) return y < aVector3.y;
			return z < aVector3.z;
	}

	template<class T>
	inline T CommonUtilities::Vector3<T>::LengthSqr() const
	{
		return x*x + y*y + z*z;
	}

	template<class T>
	inline T CommonUtilities::Vector3<T>::Length() const
	{
		return static_cast<T>(sqrt(static_cast<double>(LengthSqr())));
	}

	template<class T>
	inline Vector3<T> CommonUtilities::Vector3<T>::GetNormalized() const
	{
		T length = this->Length();
		if (length == 0)
		{
			return Vector3<T>();
		}
		else
		{
			Vector3<T> normalizedVector = *this;
			T normalizeFactor = 1 / length;

			normalizedVector.x *= normalizeFactor;
			normalizedVector.y *= normalizeFactor;
			normalizedVector.z *= normalizeFactor;
			return normalizedVector;
		}
	}

	template<class T>
	inline void CommonUtilities::Vector3<T>::Normalize()
	{
		T length = this->Length();
		if (length == 0)
		{
			return;
		}
		else
		{
			T normalizeFactor = 1 / length;
			x *= normalizeFactor;
			y *= normalizeFactor;
			z *= normalizeFactor;
		}
		
	}

	template <class T>
	Vector3<T> Vector3<T>::Lerp(Vector3<T> aVector1, Vector3<T> aVector2, float aTime)
	{
		return  (1-aTime) * aVector1 + aTime*aVector2;
	}

	template<class T>
	inline T CommonUtilities::Vector3<T>::Dot(const Vector3<T>& aVector) const
	{
		return (this->x * aVector.x) + (this->y * aVector.y) + (this->z * aVector.z);
	}

	template<class T>
	inline Vector3<T> CommonUtilities::Vector3<T>::Cross(const Vector3<T>& aVector) const
	{
		Vector3<T> returnVector;

		returnVector.x = (this->y * aVector.z) - (this->z * aVector.y);
		returnVector.y = (this->z * aVector.x) - (this->x * aVector.z);
		returnVector.z = (this->x * aVector.y) - (this->y * aVector.x);

		return returnVector;
	}

	template<class T>
	inline Vector2<T> Vector3<T>::ToVector2() const
	{
		return Vector2<T>(x, y);
	}

	template<class T>
	inline Vector4<T> Vector3<T>::ToVector4Zero() const
	{
		return Vector4<T>(x, y, z, 0);
	}

	template <class T>
	Vector4<T> Vector3<T>::ToVector4One() const
	{
		return Vector4<T>(x, y, z, 1);
	}

	template <class T>
	T* Vector3<T>::GetArray()
	{
		return &x;
	}

	//Returns the vector sum of aVector0 and aVector1
	template <class T>
	Vector3<T> operator+(const Vector3<T>& aVector0, const Vector3<T>& aVector1) 
	{
		Vector3<T> aReturnValue;
		aReturnValue.x = aVector0.x + aVector1.x;
		aReturnValue.y = aVector0.y + aVector1.y;
		aReturnValue.z = aVector0.z + aVector1.z;

		return aReturnValue;
	}

	//Returns the vector difference of aVector0 and aVector1
	template <class T>
	Vector3<T> operator-(const Vector3<T>& aVector0, const Vector3<T>& aVector1) 
	{
		Vector3<T> aReturnValue;
		aReturnValue.x = aVector0.x - aVector1.x;
		aReturnValue.y = aVector0.y - aVector1.y;
		aReturnValue.z = aVector0.z - aVector1.z;

		return aReturnValue;
	}

	//Returns the vector aVector1 component-multiplied by aVector1
	template <class T>
	Vector3<T> operator*(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		Vector3<T> aReturnValue;
		aReturnValue.x = aVector0.x * aVector1.x;
		aReturnValue.y = aVector0.y * aVector1.y;
		aReturnValue.z = aVector0.z * aVector1.z;

		return aReturnValue;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T>
	Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar) 
	{
		Vector3<T> aReturnValue;
		aReturnValue.x = aVector.x * aScalar;
		aReturnValue.y = aVector.y * aScalar;
		aReturnValue.z = aVector.z * aScalar;

		return aReturnValue;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T>
	Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector) 
	{
		Vector3<T> aReturnValue;
		aReturnValue.x = aVector.x * aScalar;
		aReturnValue.y = aVector.y * aScalar;
		aReturnValue.z = aVector.z * aScalar;

		return aReturnValue;
	}

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1 / aScalar)
	template <class T>
	Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar) 
	{
		if (aScalar == 0)
		{
			return Vector3<T>();
		}
		else
		{
			Vector3<T> aReturnValue;
			aReturnValue.x = aVector.x / aScalar;
			aReturnValue.y = aVector.y / aScalar;
			aReturnValue.z = aVector.z / aScalar;
			return aReturnValue;
		}
	}

	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template <class T>
	void operator+=(Vector3<T>& aVector0, const Vector3<T>& aVector1) 
	{
		aVector0 = aVector0 + aVector1;
	}

	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <class T>
	void operator-=(Vector3<T>& aVector0, const Vector3<T>& aVector1) 
	{
		aVector0 = aVector0 - aVector1;
	}

	//Equivalent to setting aVector to (aVector * aScalar)
	template <class T>
	void operator*=(Vector3<T>& aVector, const T& aScalar) 
	{
		aVector = aVector * aScalar;
	}

	//Equivalent to setting aVector to (aVector / aScalar)
	template <class T>
	void operator/=(Vector3<T>& aVector, const T& aScalar) 
	{
		aVector = aVector / aScalar;
	}

	template <class T>
	bool operator==(Vector3<T>& aVector1, Vector3<T>& aVector2)
	{
		if (aVector1.x != aVector2.x || aVector1.y != aVector2.y || aVector1.z != aVector2.z)
		{
			return false;
		}
		return true;
	}

	template<class T>
	inline T& Vector3<T>::operator[](const int anIndex)
	{
		assert(anIndex >= 0 && anIndex < 3);
		switch (anIndex)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		default:
			throw;
		}
	}

	template<class T>
	inline const T& Vector3<T>::operator[](const int anIndex) const
	{
		assert(anIndex >= 0 && anIndex < 3);
		switch (anIndex)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		default:
			throw;
		}

	}

	typedef Vector3<float> Vector3f;
}
namespace CU = CommonUtilities;
