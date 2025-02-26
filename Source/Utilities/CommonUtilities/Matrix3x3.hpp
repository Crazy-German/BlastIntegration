#pragma once
#include <array>
#include <assert.h>

#include "Vector3.hpp"

namespace CommonUtilities
{
	template <class T>
	class Matrix4x4;

	template <class T>
	class Quaternion;

	template <class T>
	class Matrix3x3
	{
	public:
		// Creates the identity matrix.
		Matrix3x3<T>();

		// Creates the matrix with 3 vector 3.
		Matrix3x3<T>(const Vector3<T>& aRow1, const Vector3<T>& aRow2, const Vector3<T>& aRow3);

		// Copy Constructor.
		Matrix3x3<T>(const Matrix3x3<T>& aMatrix);

		// Copies the top left 3x3 part of the Matrix4x4.
		Matrix3x3<T>(const Matrix4x4<T>& aMatrix);

		//Operator =
		Matrix3x3<T>& operator=(const Matrix3x3<T>& aMatrix3X3);

		// () operator for accessing element (row, column) for read/write or read, respectively.
		T& operator()(const int aRow, const int aColumn);
		const T& operator()(const int aRow, const int aColumn) const;

		// Creates a transposed copy of the matrix.
		Matrix3x3<T> GetTranspose() const;

		// Static functions for creating rotation matrices.
		static Matrix3x3<T> CreateRotationAroundX(const T aAngleInRadians);
		static Matrix3x3<T> CreateRotationAroundY(const T aAngleInRadians);
		static Matrix3x3<T> CreateRotationAroundZ(const T aAngleInRadians);
		static Vector3<T> GetXYZRotation(Matrix3x3<T> aMatrix);

		Vector3<T> GetRow(unsigned int aRow) const;
		Vector3<T> GetColumn(unsigned int aColumn) const;

		void SetRow(Vector3<T> aVector, unsigned int aRow);
		void SetColumn(Vector3<T> aVector, unsigned int aColumn);

		// Assumes aTransform is made up of nothing but rotations and translations.
		// Is detailed in the second matrix lecture
		Matrix3x3<T> GetFastInverse() const;
	private:
		std::array<std::array<T, 3>, 3> myMatrix;
	};

	template<class T>
	inline Matrix3x3<T>::Matrix3x3()
	{
		myMatrix[0][0] = 1;
		myMatrix[0][1] = 0;
		myMatrix[0][2] = 0;

		myMatrix[1][0] = 0;
		myMatrix[1][1] = 1;
		myMatrix[1][2] = 0;

		myMatrix[2][0] = 0;
		myMatrix[2][1] = 0;
		myMatrix[2][2] = 1;
	}

	template<class T>
	inline Matrix3x3<T>::Matrix3x3(const Vector3<T>& aRow1, const Vector3<T>& aRow2, const Vector3<T>& aRow3)
	{
		myMatrix[0][0] = aRow1.x;
		myMatrix[0][1] = aRow1.y;
		myMatrix[0][2] = aRow1.z;

		myMatrix[1][0] = aRow2.x;
		myMatrix[1][1] = aRow2.y;
		myMatrix[1][2] = aRow2.z;

		myMatrix[2][0] = aRow3.x;
		myMatrix[2][1] = aRow3.y;
		myMatrix[2][2] = aRow3.z;
	}

	template<class T>
	inline Matrix3x3<T>::Matrix3x3(const Matrix3x3<T>& aMatrix)
	{
		myMatrix = aMatrix.myMatrix;
	}

	template<class T>
	inline Matrix3x3<T>::Matrix3x3(const Matrix4x4<T>& aMatrix)
	{
		Vector4<T>row1 = aMatrix.GetRow(1);
		Vector4<T>row2 = aMatrix.GetRow(2);
		Vector4<T>row3 = aMatrix.GetRow(3);

		SetRow(Vector3<T>(row1.x, row1.y, row1.z), 1);
		SetRow(Vector3<T>(row2.x, row2.y, row2.z), 2);
		SetRow(Vector3<T>(row3.x, row3.y, row3.z), 3);
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::GetTranspose() const
	{
		Matrix3x3<T> returnValue;

		returnValue.SetRow(GetColumn(1), 1);
		returnValue.SetRow(GetColumn(2), 2);
		returnValue.SetRow(GetColumn(3), 3);

		return returnValue;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundX(const T aAngleInRadians)
	{
		Matrix3x3<T> rotationMatrix;
		rotationMatrix.SetRow(Vector3<T>(1, 0, 0), 1);
		rotationMatrix.SetRow(Vector3<T>(0, cos(aAngleInRadians), sin(aAngleInRadians)), 2);
		rotationMatrix.SetRow(Vector3<T>(0, -sin(aAngleInRadians), cos(aAngleInRadians)), 3);
		return rotationMatrix;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundY(const T aAngleInRadians)
	{
		Matrix3x3<T> rotationMatrix;
		rotationMatrix.SetRow(Vector3<T>(cos(aAngleInRadians), 0, -sin(aAngleInRadians)), 1);
		rotationMatrix.SetRow(Vector3<T>(0,	1, 0), 2);
		rotationMatrix.SetRow(Vector3<T>(sin(aAngleInRadians), 0, cos(aAngleInRadians)), 3);
		return rotationMatrix;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundZ(const T aAngleInRadians)
	{
		Matrix3x3<T> rotationMatrix;
		rotationMatrix.SetRow(Vector3<T>(cos(aAngleInRadians), sin(aAngleInRadians), 0), 1);
		rotationMatrix.SetRow(Vector3<T>(-sin(aAngleInRadians), cos(aAngleInRadians), 0), 2);
		rotationMatrix.SetRow(Vector3<T>(0, 0, 1), 3);
		return rotationMatrix;
	}

	template<class T>
	inline Vector3<T> Matrix3x3<T>::GetRow(unsigned int aRow) const
	{
		return Vector3<T>((*this)(aRow, 1), (*this)(aRow, 2), (*this)(aRow, 3));
	}

	template<class T>
	inline Vector3<T> Matrix3x3<T>::GetColumn(unsigned int aColumn) const
	{
		return Vector3<T>((*this)(1,aColumn), (*this)(2, aColumn), (*this)(3, aColumn));
	}

	template<class T>
	inline void Matrix3x3<T>::SetRow(Vector3<T> aVector, unsigned int aRow)
	{
		(*this)(aRow, 1) = aVector.x;
		(*this)(aRow, 2) = aVector.y;
		(*this)(aRow, 3) = aVector.z;
	}

	template<class T>
	inline void Matrix3x3<T>::SetColumn(Vector3<T> aVector, unsigned int aColumn)
	{
		(*this)(1, aColumn) = aVector.x;
		(*this)(2, aColumn) = aVector.y;
		(*this)(3, aColumn) = aVector.z;
	}

	template <class T>
	Matrix3x3<T> operator+(const Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
	{
		Matrix3x3<T> returnValue;
		for (unsigned int row = 1; row < 4; row++)
		{
			for (unsigned int column = 1; column < 4; column++)
			{
				returnValue(row, column) = aMatrix0(row, column) + aMatrix1(row, column);
			}
		}
		return returnValue;
	}

	template <class T>
	void operator+=(Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
	{
		aMatrix0 = aMatrix0 + aMatrix1;
	}

	template <class T>
	Matrix3x3<T> operator-(const Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
	{
		Matrix3x3<T> returnValue;
		for (unsigned int row = 1; row < 4; row++)
		{
			for (unsigned int column = 1; column < 4; column++)
			{
				returnValue(row, column) = aMatrix0(row, column) - aMatrix1(row, column);
			}
		}
		return returnValue;
	}

	template <class T>
	void operator-=(Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
	{
		aMatrix0 = aMatrix0 - aMatrix1;
	}

	template <class T>
	Matrix3x3<T> operator*(const Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
	{
		Matrix3x3<T> returnValue;
		for (unsigned int row = 1; row < 4; row++)
		{
			for (unsigned int column = 1; column < 4; column++)
			{
				returnValue(row, column) = aMatrix0.GetRow(row).Dot(aMatrix1.GetColumn(column));
			}
		}
		return returnValue;
	}

	template <class T>
	Vector3<T> operator*(const Vector3<T>& aVector, const Matrix3x3<T>& aMatrix3x3)
	{
		Vector3<T> returnValue;
		returnValue.x = aVector.x * aMatrix3x3(1, 1) + aVector.y * aMatrix3x3(2, 1) + aVector.z * aMatrix3x3(3, 1);
		returnValue.y = aVector.x * aMatrix3x3(1, 2) + aVector.y * aMatrix3x3(2, 2) + aVector.z * aMatrix3x3(3, 2);
		returnValue.z = aVector.x * aMatrix3x3(1, 3) + aVector.y * aMatrix3x3(2, 3) + aVector.z * aMatrix3x3(3, 3);
		return returnValue;
	}

	template <class T>
	void operator*=(Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
	{
		aMatrix0 = aMatrix0 * aMatrix1;
	}

	template <class T>
	void operator*=(Vector3<T>& aVector, const Matrix3x3<T>& aMatrix3x3)
	{
		aVector = aVector * aMatrix3x3;
	}

	template <class T>
	bool operator==(const Matrix3x3<T>& Matrix3x3One, const Matrix3x3<T>& aMatrix3x3Two)
	{
		for (unsigned int row = 1; row < 4; row++)
		{
			for (unsigned int column = 1; column < 4; column++)
			{
				if (!(Matrix3x3One(row, column) == aMatrix3x3Two(row, column)))
				{
					return false;
				}
			}
		}
		return true;
	}

	template<class T>
	inline Matrix3x3<T>& Matrix3x3<T>::operator=(const Matrix3x3<T>& aMatrix3X3)
	{
		for (unsigned int row = 1; row < 4; row++)
		{
			for (unsigned int column = 1; column < 4; column++)
			{
				(*this)(row, column) = aMatrix3X3(row, column);
			}
		}
		return (*this);
	}

	template<class T>
	inline T& Matrix3x3<T>::operator()(const int aRow, const int aColumn)
	{
		assert(aRow > 0 && aRow < 4 && aColumn > 0 && aColumn < 4 && "Matrix out of range");
		return myMatrix[aRow - 1][aColumn - 1];
	}

	template<class T>
	inline const T& Matrix3x3<T>::operator()(const int aRow, const int aColumn) const
	{
		assert(aRow > 0 && aRow < 4 && aColumn > 0 && aColumn < 4 && "Matrix out of range");
		return myMatrix[aRow - 1][aColumn - 1];
	}

	template<class T>
	Matrix3x3<T> Matrix3x3<T>::GetFastInverse() const
	{
		Matrix3x3<T> translation;
		translation(3, 1) = -(*this)(3, 1);
		translation(3, 2) = -(*this)(3, 2);

		Matrix3x3<T> rotation;
		rotation(1, 1) = (*this)(1, 1);
		rotation(1, 2) = (*this)(2, 1);

		rotation(2, 1) = (*this)(1, 2);
		rotation(2, 2) = (*this)(2, 2);

		return translation * rotation.GetTranspose();
	}
	typedef Matrix3x3<float> Matrix3x3f;
}
namespace CU = CommonUtilities;