#pragma once
#include <cassert>
#include <math.h>
namespace CommonUtilities
{
	template<class T>
	class Vector3;

	template <class T>
	class Vector4
	{
	public:
		T x;
		T y;
		T z;
		T w;

		//Creates a null-vector
		Vector4<T>();

		//Creates a vector (aX, aY, aZ)
		Vector4<T>(const T& aX, const T& aY, const T& aZ, const T& aW);

		//Copy constructor (compiler generated)
		Vector4<T>(const Vector4<T>& aVector) = default;

		//Assignment operator (compiler generated)
		Vector4<T>& operator=(const Vector4<T>& aVector4) = default;

		//iterator operator
		inline T& operator[](const int anIndex);
		inline const T& operator[](const int anIndex) const;

		//Destructor (compiler generated)
		~Vector4<T>() = default;

		//Explicit Type operator, create a different vector with the same values.
		//Example creates a Tga::Vector4<T> from this CommonUtillities::Vector4<T>
		template <class OtherVectorClass>
		explicit operator OtherVectorClass() const;

		//Returns a negated copy of the vector
		Vector4<T> operator-() const;

		//Returns the squared length of the vector
		T LengthSqr() const;

		//Returns the length of the vector
		T Length() const;

		//Returns a normalized copy of this vector
		Vector4<T> GetNormalized() const;

		//Normalizes the vector
		void Normalize();

		//Returns the dot product of this and aVector
		T Dot(const Vector4<T>& aVector) const;

		Vector3<T> ToVector3() const;
	};

	template<class T>
	template<class OtherVectorClass>
	inline Vector4<T>::operator OtherVectorClass() const
	{
		return OtherVectorClass(x, y, z, w);
	}

	template<class T>
	inline CommonUtilities::Vector4<T>::Vector4()
	{
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	}

	template<class T>
	inline CommonUtilities::Vector4<T>::Vector4(const T& aX, const T& aY, const T& aZ, const T& aW)
	{
		x = aX;
		y = aY;
		z = aZ;
		w = aW;
	}

	template<class T>
	inline Vector4<T> CommonUtilities::Vector4<T>::operator-() const
	{
		Vector4<T> negative;
		negative.x = -x;
		negative.y = -y;
		negative.z = -z;
		negative.w = -w;

		return negative;
	}

	template<class T>
	inline T CommonUtilities::Vector4<T>::LengthSqr() const
	{
		return x * x + y * y + z * z + w * w;
	}

	template<class T>
	inline T CommonUtilities::Vector4<T>::Length() const
	{
		return static_cast<T>(sqrt(LengthSqr()));
	}

	template<class T>
	inline Vector4<T> CommonUtilities::Vector4<T>::GetNormalized() const
	{
		T length = this->Length();
		if (length == 0)
		{
			return Vector4<T>();
		}
		else
		{
			Vector4<T> normalizedVector = *this;
			T normalizeFactor = 1 / length;

			normalizedVector.x *= normalizeFactor;
			normalizedVector.y *= normalizeFactor;
			normalizedVector.z *= normalizeFactor;
			normalizedVector.w *= normalizeFactor;

			return normalizedVector;
		}
	}

	template<class T>
	inline void CommonUtilities::Vector4<T>::Normalize()
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
			w *= normalizeFactor;

		}

	}

	template<class T>
	inline T CommonUtilities::Vector4<T>::Dot(const Vector4<T>& aVector) const
	{
		T dotProduct = (this->x * aVector.x) + (this->y * aVector.y) + (this->z * aVector.z) + (this->w * aVector.w);
		return dotProduct;
	}

	template<class T>
	inline Vector3<T> Vector4<T>::ToVector3() const
	{
		return Vector3<T>(x, y, z);
	}

	//Returns the vector sum of aVector0 and aVector1
	template <class T>
	Vector4<T> operator+(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		Vector4<T> aReturnValue;
		aReturnValue.x = aVector0.x + aVector1.x;
		aReturnValue.y = aVector0.y + aVector1.y;
		aReturnValue.z = aVector0.z + aVector1.z;
		aReturnValue.w = aVector0.w + aVector1.w;


		return aReturnValue;
	}

	//Returns the vector difference of aVector0 and aVector1
	template <class T>
	Vector4<T> operator-(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		Vector4<T> aReturnValue;
		aReturnValue.x = aVector0.x - aVector1.x;
		aReturnValue.y = aVector0.y - aVector1.y;
		aReturnValue.z = aVector0.z - aVector1.z;
		aReturnValue.w = aVector0.w - aVector1.w;


		return aReturnValue;
	}

	//Returns the vector aVector1 component-multiplied by aVector1
	template <class T>
	Vector4<T> operator*(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		Vector4<T> aReturnValue;
		aReturnValue.x = aVector0.x * aVector1.x;
		aReturnValue.y = aVector0.y * aVector1.y;
		aReturnValue.z = aVector0.z * aVector1.z;
		aReturnValue.w = aVector0.w * aVector1.w;


		return aReturnValue;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T>
	Vector4<T> operator*(const Vector4<T>& aVector, const T& aScalar)
	{
		Vector4<T> aReturnValue;
		aReturnValue.x = aVector.x * aScalar;
		aReturnValue.y = aVector.y * aScalar;
		aReturnValue.z = aVector.z * aScalar;
		aReturnValue.w = aVector.w * aScalar;


		return aReturnValue;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T>
	Vector4<T> operator*(const T& aScalar, const Vector4<T>& aVector)
	{
		Vector4<T> aReturnValue;
		aReturnValue.x = aVector.x * aScalar;
		aReturnValue.y = aVector.y * aScalar;
		aReturnValue.z = aVector.z * aScalar;
		aReturnValue.w = aVector.w * aScalar;


		return aReturnValue;
	}

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1 / aScalar)
	template <class T>
	Vector4<T> operator/(const Vector4<T>& aVector, const T& aScalar)
	{
		if (aScalar == 0)
		{
			return Vector4<T>();
		}
		else
		{
			Vector4<T> aReturnValue;
			aReturnValue.x = aVector.x / aScalar;
			aReturnValue.y = aVector.y / aScalar;
			aReturnValue.z = aVector.z / aScalar;
			aReturnValue.w = aVector.w / aScalar;

			return aReturnValue;
		}
	}

	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template <class T>
	void operator+=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		aVector0 = aVector0 + aVector1;
	}

	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <class T>
	void operator-=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		aVector0 = aVector0 - aVector1;
	}

	//Equivalent to setting aVector to (aVector * aScalar)
	template <class T>
	void operator*=(Vector4<T>& aVector, const T& aScalar)
	{
		aVector = aVector * aScalar;
	}

	//Equivalent to setting aVector to (aVector / aScalar)
	template <class T>
	void operator/=(Vector4<T>& aVector, const T& aScalar)
	{
		aVector = aVector / aScalar;
	}

	template <class T>
	bool operator==(Vector4<T>& aVector1, Vector4<T>& aVector2)
	{
		if (aVector1.x != aVector2.x || aVector1.y != aVector2.y || aVector1.z != aVector2.z || aVector1.w != aVector2.w)
		{
			return false;
		}
		return true;
	}

	template<class T>
	inline T& Vector4<T>::operator[](const int anIndex)
	{
		assert(anIndex >= 0 && anIndex < 4);
		switch (anIndex)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		case 3:
			return w;
		default:
			throw;
		}
	}

	template<class T>
	inline const T& Vector4<T>::operator[](const int anIndex) const
	{
		assert(anIndex >= 0 && anIndex < 4);
		switch (anIndex)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		case 3:
			return w;
		default:
			throw;
		}

	}

	typedef Vector4<float> Vector4f;
}
namespace CU = CommonUtilities;
