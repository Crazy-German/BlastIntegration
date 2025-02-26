#pragma once
#include "Vector3.hpp"

namespace CommonUtilities
{
	template <class T>
	class Vector2
	{
	public:
		T x;
		T y;

		//Creates a null-vector
		Vector2<T>();

		//Creates a vector (aX, aY)
		Vector2<T>(const T& aX, const T& aY);

		//Copy constructor (compiler generated)
		Vector2<T>(const Vector2<T>& aVector) = default;

		//Assignment operator (compiler generated)
		Vector2<T>& operator=(const Vector2<T>& aVector2) = default;

		//Iterator operators
		inline T& operator[](const int anIndex);
		inline const T& operator[](const int anIndex) const;

		//Destructor (compiler generated)
		~Vector2<T>() = default;

		//Explicit Type operator, create a different vector with the same values.
		//Example creates a Tga::Vector2<T> from this CommonUtillities::Vector2<T>
		template <class OtherVectorClass>
		explicit operator OtherVectorClass() const;

		//Returns a negated copy of the vector
		Vector2<T> operator-() const;

		//Returns the squared length of the vector
		T LengthSqr() const;

		//Returns the length of the vector
		T Length() const;

		//Returns a normalized copy of this vector
		Vector2<T> GetNormalized() const;

		//Normalizes the vector
		void Normalize();

		//Returns the dot product of this and aVector
		T Dot(const Vector2<T>& aVector) const;

		Vector3<T> ToVector3() const;
	};

	template<class T>
	template<class OtherVectorClass>
	inline Vector2<T>::operator OtherVectorClass() const
	{
		return OtherVectorClass(static_cast<T>(x), static_cast<T>(y));
	}

	template<class T>
	inline CommonUtilities::Vector2<T>::Vector2()
	{
		x = 0;
		y = 0;
	}

	template<class T>
	inline CommonUtilities::Vector2<T>::Vector2(const T& aX, const T& aY)
	{
		x = aX;
		y = aY;
	}

	template<class T>
	inline Vector2<T> CommonUtilities::Vector2<T>::operator-() const
	{
		Vector2<T> negative;
		negative.x = -x;
		negative.y = -y;

		return negative;
	}

	template<class T>
	inline T CommonUtilities::Vector2<T>::LengthSqr() const
	{
		return x * x + y * y;
	}

	template<class T>
	inline T CommonUtilities::Vector2<T>::Length() const
	{
		return static_cast<T>(sqrt(LengthSqr()));
	}

	template<class T>
	inline Vector2<T> CommonUtilities::Vector2<T>::GetNormalized() const
	{
		T length = this->Length();
		if (length == 0)
		{
			return Vector2<T>();
		}
		else
		{
			Vector2<T> normalizedVector = *this;
			T normalizeFactor = 1 / length;

			normalizedVector.x *= normalizeFactor;
			normalizedVector.y *= normalizeFactor;
			return normalizedVector;
		}
	}

	template<class T>
	inline void CommonUtilities::Vector2<T>::Normalize()
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
		}

	}

	template<class T>
	inline T CommonUtilities::Vector2<T>::Dot(const Vector2<T>& aVector) const
	{
		T dotProduct = (this->x * aVector.x) + (this->y * aVector.y);
		return dotProduct;
	}

	template<class T>
	inline Vector3<T> Vector2<T>::ToVector3() const
	{
		return Vector3<T>(x, y, 0);
	}

	//Returns the vector sum of aVector0 and aVector1
	template <class T>
	Vector2<T> operator+(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		Vector2<T> aReturnValue;
		aReturnValue.x = aVector0.x + aVector1.x;
		aReturnValue.y = aVector0.y + aVector1.y;

		return aReturnValue;
	}

	//Returns the vector difference of aVector0 and aVector1
	template <class T>
	Vector2<T> operator-(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		Vector2<T> aReturnValue;
		aReturnValue.x = aVector0.x - aVector1.x;
		aReturnValue.y = aVector0.y - aVector1.y;

		return aReturnValue;
	}

	//Returns the vector aVector1 component-multiplied by aVector1
	template <class T>
	Vector2<T> operator*(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		Vector2<T> aReturnValue;
		aReturnValue.x = aVector0.x * aVector1.x;
		aReturnValue.y = aVector0.y * aVector1.y;

		return aReturnValue;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T>
	Vector2<T> operator*(const Vector2<T>& aVector, const T& aScalar)
	{
		Vector2<T> aReturnValue;
		aReturnValue.x = aVector.x * aScalar;
		aReturnValue.y = aVector.y * aScalar;

		return aReturnValue;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T>
	Vector2<T> operator*(const T& aScalar, const Vector2<T>& aVector)
	{
		Vector2<T> aReturnValue;
		aReturnValue.x = aVector.x * aScalar;
		aReturnValue.y = aVector.y * aScalar;

		return aReturnValue;
	}

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1 / aScalar)
	template <class T>
	Vector2<T> operator/(const Vector2<T>& aVector, const T& aScalar)
	{
		if (aScalar == 0)
		{
			return Vector2<T>();
		}
		else
		{
			Vector2<T> aReturnValue;
			aReturnValue.x = aVector.x / aScalar;
			aReturnValue.y = aVector.y / aScalar;

			return aReturnValue;
		}
	}

	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template <class T>
	void operator+=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		aVector0 = aVector0 + aVector1;
	}

	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <class T>
	void operator-=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		aVector0 = aVector0 - aVector1;
	}

	//Equivalent to setting aVector to (aVector * aScalar)
	template <class T>
	void operator*=(Vector2<T>& aVector, const T& aScalar)
	{
		aVector = aVector * aScalar;
	}

	//Equivalent to setting aVector to (aVector / aScalar)
	template <class T>
	void operator/=(Vector2<T>& aVector, const T& aScalar)
	{
		aVector = aVector / aScalar;
	}

	template <class T>
	bool operator==(Vector2<T>& aVector1, Vector2<T>& aVector2)
	{
		if (aVector1.x != aVector2.x || aVector1.y != aVector2.y)
		{
			return false;
		}
		return true;
	}
	template<class T>
	inline T& Vector2<T>::operator[](const int anIndex)
	{
		switch (anIndex)
		{
		case 0:
			return x;
		case 1:
			return y;
		default:
			throw;
		}
	}

	template<class T>
	inline const T& Vector2<T>::operator[](const int anIndex) const
	{
		switch (anIndex)
		{
		case 0:
			return x;
		case 1:
			return y;
		default:
			throw;
		}

	}

	typedef Vector2<float> Vector2f;
	typedef Vector2<int> Vector2i;
	typedef Vector2<unsigned> Vector2ui;
}
namespace CU = CommonUtilities;