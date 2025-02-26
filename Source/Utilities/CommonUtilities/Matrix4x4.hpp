#pragma once
#include <array>
#include <cassert>

#include "Vector4.hpp"
#include "Matrix3x3.hpp"

namespace CommonUtilities
{
	template <class T>
	class Matrix4x4
	{
	public:
		// Creates the identity matrix.
		Matrix4x4<T>();

		// Creates the matrix with 4 vector 4.
		Matrix4x4<T>(const Vector4<T>& aRow1, const Vector4<T>& aRow2, const Vector4<T>& aRow3, const Vector4<T>& aRow4);

		Matrix4x4<T>(std::array<T, 16> someData);
		Matrix4x4<T>(T someData[16]);

		// Copy Constructor.
		Matrix4x4<T>(const Matrix4x4<T>& aMatrix);

		// Copy topleft 3x3 to 4x4
		Matrix4x4<T>(const Matrix3x3<T>& aMatrix);

		//Operator =
		Matrix4x4<T>& operator=(const Matrix4x4<T>& aMatrix4x4);

		// () operator for accessing element (row, column) for read/write or read, respectively.
		T& operator()(const int aRow, const int aColumn);
		const T& operator()(const int aRow, const int aColumn) const;

		//Iterator operators
		std::array<T, 4>& operator[](const int anIndex);

		// Creates a transposed copy of the matrix.
		Matrix4x4<T> GetTranspose() const;

		// Static functions for creating rotation matrices.
		static Matrix4x4<T> CreateRotationAroundX(const T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundY(const T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundZ(const T aAngleInRadians);
		static void RotateAroundLocalAxis(const Vector3<T> aLocalAxis, const T angle, Matrix4x4<T>& aTransform);

		static Matrix4x4<T> CreateTranslation(const Vector3<T>& aPosition);
		static Matrix4x4<T> CreateLookAt(const Vector3<T>& aFrom, const Vector3<T>& aTo, const Vector3<T>& aUp);

		static Matrix4x4<T> CreatePerspective(const T& aHorizontalFOVdeg, const T& aAspectRatio, const T& aNearPlane, const T& aFarPlane);
		static Matrix4x4<T> CreateOrthoGraphic(const T aLeftPlane, const T aRightPlane, const T aBottomPlane, const T aTopPlane, const T aNearPlane, const T aFarPlane);

		Vector4<T> GetRow(unsigned int aRow) const;
		Vector4<T> GetColumn(unsigned int aColumn) const;
		Vector3<T> GetTranslation() const;

		void SetRow(Vector4<T> aVector, unsigned int aRow);
		void SetColumn(Vector4<T> aVector, unsigned int aColumn);
		void SetTranslation(Vector3<T> aTranslation);
		void SetScale(Vector3<T> aScale);

		// Assumes aTransform is made up of nothing but rotations and translations.
		// Is detailed in the second matrix lecture
		Matrix4x4<T> GetFastInverse() const;
		Matrix4x4<T> GetInverse() const;

		static Matrix4x4<T> GetUnrealRotationConversionMatrix();
		static Matrix4x4<T> GetBlenderRotationConversionMatrix();
	private:
		std::array<std::array<T, 4>, 4> myMatrix;
	};

	template<class T>
	inline Matrix4x4<T>::Matrix4x4()
	{
		myMatrix[0][0] = 1;
		myMatrix[0][1] = 0;
		myMatrix[0][2] = 0;
		myMatrix[0][3] = 0;

		myMatrix[1][0] = 0;
		myMatrix[1][1] = 1;
		myMatrix[1][2] = 0;
		myMatrix[1][3] = 0;

		myMatrix[2][0] = 0;
		myMatrix[2][1] = 0;
		myMatrix[2][2] = 1;
		myMatrix[2][3] = 0;

		myMatrix[3][0] = 0;
		myMatrix[3][1] = 0;
		myMatrix[3][2] = 0;
		myMatrix[3][3] = 1;
	}

	template<class T>
	inline Matrix4x4<T>::Matrix4x4(const Vector4<T>& aRow1, const Vector4<T>& aRow2, const Vector4<T>& aRow3, const Vector4<T>& aRow4)
	{
		SetRow(aRow1, 1);
		SetRow(aRow2, 2);
		SetRow(aRow3, 3);
		SetRow(aRow4, 4);
	}

	template <class T>
	Matrix4x4<T>::Matrix4x4(std::array<T,16> someData)
	{
		myMatrix[0][0] = someData[0];
		myMatrix[0][1] = someData[1];
		myMatrix[0][2] = someData[2];
		myMatrix[0][3] = someData[3];

		myMatrix[1][0] = someData[4];
		myMatrix[1][1] = someData[5];
		myMatrix[1][2] = someData[6];
		myMatrix[1][3] = someData[7];

		myMatrix[2][0] = someData[8];
		myMatrix[2][1] = someData[9];
		myMatrix[2][2] = someData[10];
		myMatrix[2][3] = someData[11];

		myMatrix[3][0] = someData[12];
		myMatrix[3][1] = someData[13];
		myMatrix[3][2] = someData[14];
		myMatrix[3][3] = someData[15];
	}

	template <class T>
	Matrix4x4<T>::Matrix4x4(T someData[16])
	{
		myMatrix[0][0] = someData[0];
		myMatrix[0][1] = someData[1];
		myMatrix[0][2] = someData[2];
		myMatrix[0][3] = someData[3];

		myMatrix[1][0] = someData[4];
		myMatrix[1][1] = someData[5];
		myMatrix[1][2] = someData[6];
		myMatrix[1][3] = someData[7];

		myMatrix[2][0] = someData[8];
		myMatrix[2][1] = someData[9];
		myMatrix[2][2] = someData[10];
		myMatrix[2][3] = someData[11];

		myMatrix[3][0] = someData[12];
		myMatrix[3][1] = someData[13];
		myMatrix[3][2] = someData[14];
		myMatrix[3][3] = someData[15];
	}

	template<class T>
	inline Matrix4x4<T>::Matrix4x4(const Matrix4x4<T>& aMatrix)
	{
		myMatrix = aMatrix.myMatrix;
	}

	template<class T>
	inline Matrix4x4<T>::Matrix4x4(const Matrix3x3<T>& aMatrix)
	{
		for (int i = 1; i < 4; i++)
		{
			for (int j = 1; j < 4; j++)
			{
				(*this)(i, j) = aMatrix(i, j);
			}
		}
		(*this)(1, 4) = 0;
		(*this)(2, 4) = 0;
		(*this)(3, 4) = 0;

		(*this)(4, 1) = 0;
		(*this)(4, 2) = 0;
		(*this)(4, 3) = 0;
		(*this)(4, 4) = 1;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::GetTranspose() const
	{
		Matrix4x4<T> returnValue;
		returnValue.SetRow(GetColumn(1), 1);
		returnValue.SetRow(GetColumn(2), 2);
		returnValue.SetRow(GetColumn(3), 3);
		returnValue.SetRow(GetColumn(4), 4);
		return returnValue;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundX(const T aAngleInRadians)
	{
		Matrix4x4<T> rotationMatrix;
		rotationMatrix(1, 1) = static_cast<T>(1);
		rotationMatrix(1, 2) = static_cast<T>(0);
		rotationMatrix(1, 3) = static_cast<T>(0);
		rotationMatrix(1, 4) = static_cast<T>(0);

		rotationMatrix(2, 1) = static_cast<T>(0);
		rotationMatrix(2, 2) = static_cast<T>(cos(aAngleInRadians));
		rotationMatrix(2, 3) = static_cast<T>(sin(aAngleInRadians));
		rotationMatrix(2, 4) = static_cast<T>(0);

		rotationMatrix(3, 1) = static_cast<T>(0);
		rotationMatrix(3, 2) = static_cast<T>(-1 * sin(aAngleInRadians));
		rotationMatrix(3, 3) = static_cast<T>(cos(aAngleInRadians));
		rotationMatrix(3, 4) = static_cast<T>(0);

		rotationMatrix(4, 1) = static_cast<T>(0);
		rotationMatrix(4, 2) = static_cast<T>(0);
		rotationMatrix(4, 3) = static_cast<T>(0);
		rotationMatrix(4, 4) = static_cast<T>(1);
		return rotationMatrix;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundY(const T aAngleInRadians)
	{
		Matrix4x4<T> rotationMatrix;
		rotationMatrix(1, 1) = static_cast<T>(cos(aAngleInRadians));
		rotationMatrix(1, 2) = static_cast<T>(0);
		rotationMatrix(1, 3) = static_cast<T> (-1 * sin(aAngleInRadians));
		rotationMatrix(1, 4) = static_cast<T>(0);

		rotationMatrix(2, 1) = static_cast<T>(0);
		rotationMatrix(2, 2) = static_cast<T>(1);
		rotationMatrix(2, 3) = static_cast<T>(0);
		rotationMatrix(2, 4) = static_cast<T>(0);

		rotationMatrix(3, 1) = static_cast<T> (sin(aAngleInRadians));
		rotationMatrix(3, 2) = static_cast<T>(0);
		rotationMatrix(3, 3) = static_cast<T> (cos(aAngleInRadians));
		rotationMatrix(3, 4) = static_cast<T>(0);

		rotationMatrix(4, 1) = static_cast<T>(0);
		rotationMatrix(4, 2) = static_cast<T>(0);
		rotationMatrix(4, 3) = static_cast<T>(0);
		rotationMatrix(4, 4) = static_cast<T>(1);
		return rotationMatrix;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundZ(const T aAngleInRadians)
	{
		Matrix4x4<T> rotationMatrix;
		rotationMatrix(1, 1) = static_cast<T>(cos(aAngleInRadians));
		rotationMatrix(1, 2) = static_cast<T>(sin(aAngleInRadians));
		rotationMatrix(1, 3) = static_cast<T>(0);
		rotationMatrix(1, 4) = static_cast<T>(0);

		rotationMatrix(2, 1) = static_cast<T>(-1 * sin(aAngleInRadians));
		rotationMatrix(2, 2) = static_cast<T>(cos(aAngleInRadians));
		rotationMatrix(2, 3) = static_cast<T>(0);
		rotationMatrix(2, 4) = static_cast<T>(0);

		rotationMatrix(3, 1) = static_cast<T>(0);
		rotationMatrix(3, 2) = static_cast<T>(0);
		rotationMatrix(3, 3) = static_cast<T>(1);
		rotationMatrix(3, 4) = static_cast<T>(0);

		rotationMatrix(4, 1) = static_cast<T>(0);
		rotationMatrix(4, 2) = static_cast<T>(0);
		rotationMatrix(4, 3) = static_cast<T>(0);
		rotationMatrix(4, 4) = static_cast<T>(1);
		return rotationMatrix;
	}

	template <class T>
	void Matrix4x4<T>::RotateAroundLocalAxis(const Vector3<T> aLocalAxis, const T angle, Matrix4x4<T>& aTransform)
	{
		Vector3<T> position = aTransform.GetTranslation();
		aTransform.SetTranslation({ 0, 0, 0 });

		Vector3<T> axis(
			aTransform.GetRow(1).ToVector3() * aLocalAxis.x +
			aTransform.GetRow(2).ToVector3() * aLocalAxis.y +
			aTransform.GetRow(3).ToVector3() * aLocalAxis.z);

		T angleRadians = angle * (static_cast<T>(3.14159265359) / static_cast<T>(180));
		aTransform *= Matrix4x4<T>::CreateRotationAroundX(axis.x * angleRadians);
		aTransform *= Matrix4x4<T>::CreateRotationAroundY(axis.y * angleRadians);
		aTransform *= Matrix4x4<T>::CreateRotationAroundZ(axis.z * angleRadians);

		aTransform.SetTranslation(position);
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateTranslation(const Vector3<T>& aPosition)
	{
		Matrix4x4<T> returnValue;
		returnValue.SetRow(Vector4<T>(aPosition.x, aPosition.y, aPosition.z, 1), 4);
		return returnValue;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateLookAt(const Vector3<T>& aFrom, const Vector3<T>& aTo, const Vector3<T>& aUp)
	{
		Vector3<T> forward = (aTo - aFrom).GetNormalized();
		Vector3<T> right = aUp.Cross(forward).GetNormalized();
		Vector3<T> up = forward.Cross(right).GetNormalized();
		right = up.Cross(forward).GetNormalized();


		Matrix4x4<T> result;

		result(1, 1) = right.x;
		result(1, 2) = right.y;
		result(1, 3) = right.z;
		result(2, 1) = up.x;
		result(2, 2) = up.y;
		result(2, 3) = up.z;
		result(3, 1) = forward.x;
		result(3, 2) = forward.y;
		result(3, 3) = forward.z;
		result(4, 1) = aFrom.x;
		result(4, 2) = aFrom.y;
		result(4, 3) = aFrom.z;

		return result;
	}

	template <class T>
	Matrix4x4<T> Matrix4x4<T>::CreatePerspective(const T& aHorizontalFOVdeg, const T& aAspectRatio, const T& aNearPlane, const T& aFarPlane)
	{
		T FOVRad = (aHorizontalFOVdeg * static_cast<T>(3.1415)) / static_cast<T>(180);
		T FOV = static_cast<T>(1) / static_cast <T>(tan(FOVRad / static_cast <T>(2)));
		T depth = aFarPlane / (aFarPlane - aNearPlane);

		Matrix4x4<T> perspectiveMatrix
		(
			Vector4<T>(FOV, static_cast<T>(0), static_cast<T>(0), static_cast<T>(0)),
			Vector4<T>(static_cast<T>(0), aAspectRatio * FOV, static_cast<T>(0), static_cast<T>(0)),
			Vector4<T>(static_cast<T>(0), static_cast<T>(0), depth, 1),
			Vector4<T>(static_cast<T>(0), static_cast<T>(0), -aNearPlane * depth, static_cast<T>(0))
		);
		return perspectiveMatrix;
	}

	template <class T>
	Matrix4x4<T> Matrix4x4<T>::CreateOrthoGraphic(const T aLeftPlane, const T aRightPlane, const T aBottomPlane, const T aTopPlane, const T aNearPlane, const T aFarPlane)
	{
		const float reciprocalWidth = 1.0f / (aRightPlane - aLeftPlane);
		const float reciprocalHeight = 1.0f / (aTopPlane - aBottomPlane);
		const float fRange = 1.0f / (aFarPlane - aNearPlane);

		Matrix4x4<T> matrix;
		matrix(1, 1) = reciprocalWidth + reciprocalWidth;
		matrix(2, 2) = reciprocalHeight + reciprocalHeight;
		matrix(3, 3) = fRange;
		matrix(4, 1) = -1 * (aLeftPlane + aRightPlane) * reciprocalWidth;
		matrix(4, 2) = -1 * (aTopPlane + aBottomPlane) * reciprocalHeight;
		matrix(4, 3) = -1 * fRange * aNearPlane;
		matrix(4, 4) = 1;
		return matrix;
	}

	template<class T>
	inline Vector4<T> Matrix4x4<T>::GetRow(unsigned int aRow) const
	{
		return Vector4<T>((*this)(aRow, 1), (*this)(aRow, 2), (*this)(aRow, 3), (*this)(aRow, 4));
	}

	template<class T>
	inline Vector4<T> Matrix4x4<T>::GetColumn(unsigned int aColumn) const
	{
		return Vector4<T>((*this)(1, aColumn), (*this)(2, aColumn), (*this)(3, aColumn), (*this)(4, aColumn));
	}

	template <class T>
	Vector3<T> Matrix4x4<T>::GetTranslation() const
	{
		return Vector3<T>((*this)(4, 1), (*this)(4, 2), (*this)(4, 3));
	}

	template<class T>
	inline void Matrix4x4<T>::SetRow(Vector4<T> aVector, unsigned int aRow)
	{
		(*this)(aRow, 1) = aVector.x;
		(*this)(aRow, 2) = aVector.y;
		(*this)(aRow, 3) = aVector.z;
		(*this)(aRow, 4) = aVector.w;
	}

	template<class T>
	inline void Matrix4x4<T>::SetColumn(Vector4<T> aVector, unsigned int aColumn)
	{
		(*this)(1, aColumn) = aVector.x;
		(*this)(2, aColumn) = aVector.y;
		(*this)(3, aColumn) = aVector.z;
		(*this)(4, aColumn) = aVector.w;
	}

	template <class T>
	void Matrix4x4<T>::SetTranslation(Vector3<T> aTranslation)
	{
		(*this)(4, 1) = aTranslation.x;
		(*this)(4, 2) = aTranslation.y;
		(*this)(4, 3) = aTranslation.z;
	}

	template <class T>
	void Matrix4x4<T>::SetScale(Vector3<T> aScale)
	{
		myMatrix[0][0] = myMatrix[0][0] * aScale.x;
		myMatrix[1][1] = myMatrix[1][1] * aScale.y;
		myMatrix[2][2] = myMatrix[2][2] * aScale.z;
	}

	template <class T>
	Matrix4x4<T> operator+(const Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		Matrix4x4<T> returnValue;
		for (unsigned int row = 1; row < 5; row++)
		{
			for (unsigned int column = 1; column < 5; column++)
			{
				returnValue(row, column) = aMatrix0(row, column) + aMatrix1(row, column);
			}
		}
		return returnValue;
	}

	template <class T>
	void operator+=(Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		aMatrix0 = aMatrix0 + aMatrix1;
	}

	template <class T>
	Matrix4x4<T> operator-(const Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		Matrix4x4<T> returnValue;
		for (unsigned int row = 1; row < 5; row++)
		{
			for (unsigned int column = 1; column < 5; column++)
			{
				returnValue(row, column) = aMatrix0(row, column) - aMatrix1(row, column);
			}
		}
		return returnValue;
	}

	template <class T>
	void operator-=(Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		aMatrix0 = aMatrix0 - aMatrix1;
	}

	template <class T>
	Matrix4x4<T> operator*(const Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		Matrix4x4<T> returnValue;
		for (unsigned int row = 1; row < 5; row++)
		{
			for (unsigned int column = 1; column < 5; column++)
			{
				returnValue(row, column) = aMatrix0.GetRow(row).Dot(aMatrix1.GetColumn(column));
			}
		}
		return returnValue;
	}

	template <class T>
	Vector4<T> operator*(const Vector4<T>& aVector, const Matrix4x4<T>& aMatrix4x4)
	{
		Vector4<T> returnValue;
		returnValue.x = aVector.x * aMatrix4x4(1, 1) + aVector.y * aMatrix4x4(2, 1) + aVector.z * aMatrix4x4(3, 1) + aVector.w * aMatrix4x4(4, 1);
		returnValue.y = aVector.x * aMatrix4x4(1, 2) + aVector.y * aMatrix4x4(2, 2) + aVector.z * aMatrix4x4(3, 2) + aVector.w * aMatrix4x4(4, 2);
		returnValue.z = aVector.x * aMatrix4x4(1, 3) + aVector.y * aMatrix4x4(2, 3) + aVector.z * aMatrix4x4(3, 3) + aVector.w * aMatrix4x4(4, 3);
		returnValue.w = aVector.x * aMatrix4x4(1, 4) + aVector.y * aMatrix4x4(2, 4) + aVector.z * aMatrix4x4(3, 4) + aVector.w * aMatrix4x4(4, 4);
		return returnValue;
	}

	template <class T>
	Vector3<T> operator*(const Vector3<T>& aVector, const Matrix4x4<T>& aMatrix4x4)
	{
		Vector3<T> returnValue;
		returnValue.x = aVector.x * aMatrix4x4(1, 1) + aVector.y * aMatrix4x4(2, 1) + aVector.z * aMatrix4x4(3, 1) + aMatrix4x4(4, 1);
		returnValue.y = aVector.x * aMatrix4x4(1, 2) + aVector.y * aMatrix4x4(2, 2) + aVector.z * aMatrix4x4(3, 2) + aMatrix4x4(4, 2);
		returnValue.z = aVector.x * aMatrix4x4(1, 3) + aVector.y * aMatrix4x4(2, 3) + aVector.z * aMatrix4x4(3, 3) + aMatrix4x4(4, 3);
		return returnValue;
	}

	template <class T>
	void operator*=(Matrix4x4<T>& aMatrix0, const Matrix4x4<T>& aMatrix1)
	{
		aMatrix0 = aMatrix0 * aMatrix1;
	}

	template <class T>
	Matrix4x4<T> operator*=(Vector4<T>& aVector, const Matrix4x4<T>& aMatrix4x4)
	{
		aVector = aVector * aMatrix4x4;
	}

	template <class T>
	bool operator==(const Matrix4x4<T>& aMatrix4x4One, const Matrix4x4<T>& aMatrix4x4Two)
	{
		for (unsigned int row = 1; row < 5; row++)
		{
			for (unsigned int column = 1; column < 5; column++)
			{
				if (!(aMatrix4x4One(row, column) == aMatrix4x4Two(row, column)))
				{
					return false;
				}
			}
		}
		return true;
	}

	template<class T>
	inline Matrix4x4<T>& Matrix4x4<T>::operator=(const Matrix4x4<T>& aMatrix4x4)
	{
		for (unsigned int row = 1; row < 5; row++)
		{
			for (unsigned int column = 1; column < 5; column++)
			{
				(*this)(row, column) = aMatrix4x4(row, column);
			}
		}
		return (*this);
	}

	template<class T>
	inline T& Matrix4x4<T>::operator()(const int aRow, const int aColumn)
	{
		assert(aRow > 0 && aRow < 5 && aColumn > 0 && aColumn < 5 && "Matrix out of range");
		return myMatrix[aRow - 1][aColumn - 1];
	}

	template<class T>
	inline const T& Matrix4x4<T>::operator()(const int aRow, const int aColumn) const
	{
		assert(aRow > 0 && aRow < 5 && aColumn > 0 && aColumn < 5 && "Matrix out of range");
		return myMatrix[aRow - 1][aColumn - 1];
	}

	template<class T>
	Matrix4x4<T> Matrix4x4<T>::GetFastInverse() const
	{
		//OLD CODE
		//Matrix4x4<T> translation;
		//translation(4, 1) = -(*this)(4, 1);
		//translation(4, 2) = -(*this)(4, 2);
		//translation(4, 3) = -(*this)(4, 3);
		//
		//Matrix3x3<T> rotation = (*this);
		//
		//return translation * Matrix4x4<T>(rotation.GetTranspose());

		//Create rotationInverse by transposing the transform
		Matrix4x4<T> rotation = (*this);
		Matrix4x4<T> transposedRotation = rotation.GetTranspose();

		//Clear transform
		transposedRotation(1, 4) = static_cast<T> (0);
		transposedRotation(2, 4) = static_cast<T> (0);
		transposedRotation(3, 4) = static_cast<T> (0);

		//Create translationInverse as new indentityMatrix and negate the transform (row 4)
		Vector3<T> translationVector;
		translationVector.x = (*this)(4, 1) * -1;
		translationVector.y = (*this)(4, 2) * -1;
		translationVector.z = (*this)(4, 3) * -1;

		//Copy 3x3 part of rotationsinverse and multiply with negated translationVector
		Matrix3x3<T> rotationInverse(transposedRotation);
		Vector3<T> inversedVector = translationVector * rotationInverse;

		Matrix4x4<T> fastInverse;
		for (size_t index = 0; index < 3; ++index)
		{
			for (size_t index2 = 0; index2 < 3; ++index2)
			{
				fastInverse.myMatrix[index][index2] = transposedRotation.myMatrix[index][index2];
			}
		}
		fastInverse(4, 1) = inversedVector.x;
		fastInverse(4, 2) = inversedVector.y;
		fastInverse(4, 3) = inversedVector.z;
		fastInverse(4, 4) = static_cast<T> (1);

		//Create fastInverse by multiplying inverses of translation * rotation. OBS! Order matters!

		return fastInverse;
	}

	template<class T>
	Matrix4x4<T> Matrix4x4<T>::GetInverse() const
	{
		Matrix4x4<T> outMatrix;
		T inv[4][4];
		T det;

		inv[0][0] =
			myMatrix[1][1] * myMatrix[2][2] * myMatrix[3][3] -
			myMatrix[1][1] * myMatrix[2][3] * myMatrix[3][2] -
			myMatrix[2][1] * myMatrix[1][2] * myMatrix[3][3] +
			myMatrix[2][1] * myMatrix[1][3] * myMatrix[3][2] +
			myMatrix[3][1] * myMatrix[1][2] * myMatrix[2][3] -
			myMatrix[3][1] * myMatrix[1][3] * myMatrix[2][2];

		inv[1][0] =
			-myMatrix[1][0] * myMatrix[2][2] * myMatrix[3][3] +
			myMatrix[1][0] * myMatrix[2][3] * myMatrix[3][2] +
			myMatrix[2][0] * myMatrix[1][2] * myMatrix[3][3] -
			myMatrix[2][0] * myMatrix[1][3] * myMatrix[3][2] -
			myMatrix[3][0] * myMatrix[1][2] * myMatrix[2][3] +
			myMatrix[3][0] * myMatrix[1][3] * myMatrix[2][2];


		inv[2][0] =
			myMatrix[1][0] * myMatrix[2][1] * myMatrix[3][3] -
			myMatrix[1][0] * myMatrix[2][3] * myMatrix[3][1] -
			myMatrix[2][0] * myMatrix[1][1] * myMatrix[3][3] +
			myMatrix[2][0] * myMatrix[1][3] * myMatrix[3][1] +
			myMatrix[3][0] * myMatrix[1][1] * myMatrix[2][3] -
			myMatrix[3][0] * myMatrix[1][3] * myMatrix[2][1];


		inv[3][0] =
			-myMatrix[1][0] * myMatrix[2][1] * myMatrix[3][2] +
			myMatrix[1][0] * myMatrix[2][2] * myMatrix[3][1] +
			myMatrix[2][0] * myMatrix[1][1] * myMatrix[3][2] -
			myMatrix[2][0] * myMatrix[1][2] * myMatrix[3][1] -
			myMatrix[3][0] * myMatrix[1][1] * myMatrix[2][2] +
			myMatrix[3][0] * myMatrix[1][2] * myMatrix[2][1];

		inv[0][1] =
			-myMatrix[0][1] * myMatrix[2][2] * myMatrix[3][3] +
			myMatrix[0][1] * myMatrix[2][3] * myMatrix[3][2] +
			myMatrix[2][1] * myMatrix[0][2] * myMatrix[3][3] -
			myMatrix[2][1] * myMatrix[0][3] * myMatrix[3][2] -
			myMatrix[3][1] * myMatrix[0][2] * myMatrix[2][3] +
			myMatrix[3][1] * myMatrix[0][3] * myMatrix[2][2];

		inv[1][1] =
			myMatrix[0][0] * myMatrix[2][2] * myMatrix[3][3] -
			myMatrix[0][0] * myMatrix[2][3] * myMatrix[3][2] -
			myMatrix[2][0] * myMatrix[0][2] * myMatrix[3][3] +
			myMatrix[2][0] * myMatrix[0][3] * myMatrix[3][2] +
			myMatrix[3][0] * myMatrix[0][2] * myMatrix[2][3] -
			myMatrix[3][0] * myMatrix[0][3] * myMatrix[2][2];

		inv[2][1] =
			-myMatrix[0][0] * myMatrix[2][1] * myMatrix[3][3] +
			myMatrix[0][0] * myMatrix[2][3] * myMatrix[3][1] +
			myMatrix[2][0] * myMatrix[0][1] * myMatrix[3][3] -
			myMatrix[2][0] * myMatrix[0][3] * myMatrix[3][1] -
			myMatrix[3][0] * myMatrix[0][1] * myMatrix[2][3] +
			myMatrix[3][0] * myMatrix[0][3] * myMatrix[2][1];

		inv[3][1] =
			myMatrix[0][0] * myMatrix[2][1] * myMatrix[3][2] -
			myMatrix[0][0] * myMatrix[2][2] * myMatrix[3][1] -
			myMatrix[2][0] * myMatrix[0][1] * myMatrix[3][2] +
			myMatrix[2][0] * myMatrix[0][2] * myMatrix[3][1] +
			myMatrix[3][0] * myMatrix[0][1] * myMatrix[2][2] -
			myMatrix[3][0] * myMatrix[0][2] * myMatrix[2][1];

		inv[0][2] =
			myMatrix[0][1] * myMatrix[1][2] * myMatrix[3][3] -
			myMatrix[0][1] * myMatrix[1][3] * myMatrix[3][2] -
			myMatrix[1][1] * myMatrix[0][2] * myMatrix[3][3] +
			myMatrix[1][1] * myMatrix[0][3] * myMatrix[3][2] +
			myMatrix[3][1] * myMatrix[0][2] * myMatrix[1][3] -
			myMatrix[3][1] * myMatrix[0][3] * myMatrix[1][2];

		inv[1][2] =
			-myMatrix[0][0] * myMatrix[1][2] * myMatrix[3][3] +
			myMatrix[0][0] * myMatrix[1][3] * myMatrix[3][2] +
			myMatrix[1][0] * myMatrix[0][2] * myMatrix[3][3] -
			myMatrix[1][0] * myMatrix[0][3] * myMatrix[3][2] -
			myMatrix[3][0] * myMatrix[0][2] * myMatrix[1][3] +
			myMatrix[3][0] * myMatrix[0][3] * myMatrix[1][2];

		inv[2][2] =
			myMatrix[0][0] * myMatrix[1][1] * myMatrix[3][3] -
			myMatrix[0][0] * myMatrix[1][3] * myMatrix[3][1] -
			myMatrix[1][0] * myMatrix[0][1] * myMatrix[3][3] +
			myMatrix[1][0] * myMatrix[0][3] * myMatrix[3][1] +
			myMatrix[3][0] * myMatrix[0][1] * myMatrix[1][3] -
			myMatrix[3][0] * myMatrix[0][3] * myMatrix[1][1];

		inv[3][2] =
			-myMatrix[0][0] * myMatrix[1][1] * myMatrix[3][2] +
			myMatrix[0][0] * myMatrix[1][2] * myMatrix[3][1] +
			myMatrix[1][0] * myMatrix[0][1] * myMatrix[3][2] -
			myMatrix[1][0] * myMatrix[0][2] * myMatrix[3][1] -
			myMatrix[3][0] * myMatrix[0][1] * myMatrix[1][2] +
			myMatrix[3][0] * myMatrix[0][2] * myMatrix[1][1];

		inv[0][3] =
			-myMatrix[0][1] * myMatrix[1][2] * myMatrix[2][3] +
			myMatrix[0][1] * myMatrix[1][3] * myMatrix[2][2] +
			myMatrix[1][1] * myMatrix[0][2] * myMatrix[2][3] -
			myMatrix[1][1] * myMatrix[0][3] * myMatrix[2][2] -
			myMatrix[2][1] * myMatrix[0][2] * myMatrix[1][3] +
			myMatrix[2][1] * myMatrix[0][3] * myMatrix[1][2];

		inv[1][3] =
			myMatrix[0][0] * myMatrix[1][2] * myMatrix[2][3] -
			myMatrix[0][0] * myMatrix[1][3] * myMatrix[2][2] -
			myMatrix[1][0] * myMatrix[0][2] * myMatrix[2][3] +
			myMatrix[1][0] * myMatrix[0][3] * myMatrix[2][2] +
			myMatrix[2][0] * myMatrix[0][2] * myMatrix[1][3] -
			myMatrix[2][0] * myMatrix[0][3] * myMatrix[1][2];

		inv[2][3] =
			-myMatrix[0][0] * myMatrix[1][1] * myMatrix[2][3] +
			myMatrix[0][0] * myMatrix[1][3] * myMatrix[2][1] +
			myMatrix[1][0] * myMatrix[0][1] * myMatrix[2][3] -
			myMatrix[1][0] * myMatrix[0][3] * myMatrix[2][1] -
			myMatrix[2][0] * myMatrix[0][1] * myMatrix[1][3] +
			myMatrix[2][0] * myMatrix[0][3] * myMatrix[1][1];

		inv[3][3] =
			myMatrix[0][0] * myMatrix[1][1] * myMatrix[2][2] -
			myMatrix[0][0] * myMatrix[1][2] * myMatrix[2][1] -
			myMatrix[1][0] * myMatrix[0][1] * myMatrix[2][2] +
			myMatrix[1][0] * myMatrix[0][2] * myMatrix[2][1] +
			myMatrix[2][0] * myMatrix[0][1] * myMatrix[1][2] -
			myMatrix[2][0] * myMatrix[0][2] * myMatrix[1][1];

		det = myMatrix[0][0] * inv[0][0] + myMatrix[0][1] * inv[1][0] + myMatrix[0][2] * inv[2][0] + myMatrix[0][3] * inv[3][0];

		if (det == 0)
		{
			return outMatrix;
		}

		det = static_cast<T>(1) / det;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				outMatrix.myMatrix[i][j] = inv[i][j] * det;
			}
		}

		return outMatrix;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::GetUnrealRotationConversionMatrix()
	{
		Matrix4x4 m;
		m = Matrix4x4::CreateRotationAroundY(1.57f) * m;
		m =  Matrix4x4::CreateRotationAroundZ(-3.14f) * m;
		return m;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::GetBlenderRotationConversionMatrix()
	{
		Matrix4x4 m;
		m = Matrix4x4::CreateRotationAroundX(3.14159265359f) * Matrix4x4::CreateRotationAroundY(-1.5708f) * m;
		return m;
	}

	template<typename T>
	inline std::array<T, 4>& Matrix4x4<T>::operator[](const int anIndex)
	{
		return myMatrix[anIndex];
	}

	typedef Matrix4x4<float> Matrix4x4f;
}
namespace CU = CommonUtilities;