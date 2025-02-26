#pragma once
#include "Matrix.hpp"
#include "Plane.hpp"
#include "Vector.hpp"
#include <vector>
#include "CommonMath.hpp"
#include  <array>
#include "AABB3D.hpp"

namespace CommonUtilities
{
	template<class T>
	class OBB3D
	{
	public:
		//Default constructor
		OBB3D();

		//Copy constructor.
		OBB3D(OBB3D<T>& aOBB3D);

		//Constructor with position, size and rotation.
		OBB3D(const Vector3<T>& aPosition, const Vector3<T>& aSize, const Matrix4x4<T>& aRotationMatrix);

		//Init with position, size and rotation.
		void Init(const Vector3<T>& aPosition, const Vector3<T>& aSize, const Matrix4x4<T>& aRotationMatrix);

		//Return true if point isinside OBB3D, else return false.
		bool IsInsideMeshCell(const Vector3<T>& aPosition) const;

		Vector3<T> NearestPoint(const Vector3<T>& aPoint) const;
		Interval<T> GetInterval(const Vector3<T>& aAxis) const;

		AABB3D<T> OBB3DToAABB3D() const;
		AABB3D<T> OBB3DToAABB3DLocalSpace() const;

		const Vector3<T>& GetPosition() const;
		const Matrix4x4<T>& GetRotationMatrix() const;
		const std::array<Vector3<T>, 8>& GetVerticies() const;

		void SetPosition(const Vector3<T>& aPosition);
		void SetSize(const Vector3<T>& aSize);
		void SetRotationMatrix(const Matrix4x4<T> aRotationMatrix);
		void UpdateVerticies();


	private:
		Vector3<T> myPosition;
		Vector3<T> mySize;
		Matrix4x4<T> myRotationMatrix;
		std::array<Vector3<T>, 8> myVerticies;
	};

	template<class T>
	inline OBB3D<T>::OBB3D()
	{
		myPosition = Vector3<T>();
		mySize = Vector3<T>();
		myRotationMatrix = Matrix4x4<T>();
	}

	template<class T>
	inline OBB3D<T>::OBB3D(OBB3D<T>& aOBB3D)
	{
		myPosition = aOBB3D.myPosition;
		mySize = aOBB3D.mySize;
		myRotationMatrix = aOBB3D.myRotationMatrix;
		UpdateVerticies();
	}

	template<class T>
	inline OBB3D<T>::OBB3D(const Vector3<T>& aPosition, const Vector3<T>& aSize, const Matrix4x4<T>& aRotationMatrix)
	{
		myPosition = aPosition;
		mySize = aSize;
		myRotationMatrix = aRotationMatrix;
		UpdateVerticies();
	}

	template<class T>
	inline void OBB3D<T>::Init(const Vector3<T>& aPosition, const Vector3<T>& aSize, const Matrix4x4<T>& aRotationMatrix)
	{
		myPosition = aPosition;
		mySize = aSize;
		myRotationMatrix = aRotationMatrix;
		UpdateVerticies();
	}

	template<class T>
	inline bool OBB3D<T>::IsInsideMeshCell(const Vector3<T>& aPosition) const
	{
		Vector3<T> direction = aPosition - myPosition;
		std::vector<Vector3<T>> orientationsArray =
		{
			myRotationMatrix.GetRow(1).ToVector3(),
			myRotationMatrix.GetRow(2).ToVector3(),
			myRotationMatrix.GetRow(3).ToVector3()
		};
		std::array<T, 3> sizeArray = { mySize.x, mySize.y, mySize.z };

		for (size_t i = 0; i < 3; i++)
		{
			T distance = direction.Dot(orientationsArray[i]);
			if (abs(distance) > abs(sizeArray[i]))
			{
				return false;
			}
		}
		return true;
	}

	template<class T>
	inline Vector3<T> OBB3D<T>::NearestPoint(const Vector3<T>& aPoint) const
	{
		Vector3<T> returnValue = myPosition;
		Vector3<T> direction = aPoint - myPosition;
		std::vector<Vector3<T>> orientationsArray =
		{
			myRotationMatrix.GetRow(1).ToVector3(),
			myRotationMatrix.GetRow(2).ToVector3(),
			myRotationMatrix.GetRow(3).ToVector3()
		};
		std::array<T, 3> sizeArray = { mySize.x, mySize.y, mySize.z };

		for (size_t i = 0; i < 3; i++)
		{
			T distance = direction.Dot(orientationsArray[i]);
			distance = CommonUtilities::Clamp(distance, -sizeArray[i], sizeArray[i]);
			returnValue += (orientationsArray[i] * distance);
		}
		return returnValue;
	}

	template<class T>
	inline const Vector3<T>& OBB3D<T>::GetPosition() const
	{
		return myPosition;
	}

	template<class T>
	inline const Matrix4x4<T>& OBB3D<T>::GetRotationMatrix() const
	{
		return myRotationMatrix;
	}

	template<class T>
	inline CommonUtilities::Interval<T> OBB3D<T>::GetInterval(const Vector3<T>& aAxis) const
	{
		Interval<T> returnValue;
		returnValue.min = aAxis.Dot(myVerticies[0]);
		returnValue.max = returnValue.min;

		for (int i = 0; i < 8; i++)
		{
			T dot = aAxis.Dot(myVerticies[i]);
			returnValue.min = min(returnValue.min, dot);
			returnValue.max = max(returnValue.max, dot);
		}
		return returnValue;
	}

	template<class T>
	inline void OBB3D<T>::SetPosition(const Vector3<T>& aPoistion)
	{
		myPosition = aPoistion;
		UpdateVerticies();
	}

	template<class T>
	inline void OBB3D<T>::SetSize(const Vector3<T>& aSize)
	{
		mySize = aSize;
		UpdateVerticies();
	}

	template<class T>
	inline void OBB3D<T>::SetRotationMatrix(const Matrix4x4<T> aRotationMatrix)
	{
		myRotationMatrix = aRotationMatrix;
		UpdateVerticies();
	}

	template<class T>
	void OBB3D<T>::UpdateVerticies()
	{
		Vector3<T> orientationX = myRotationMatrix.GetRow(1).ToVector3();
		Vector3<T> orientationY = myRotationMatrix.GetRow(2).ToVector3();
		Vector3<T> orientationZ = myRotationMatrix.GetRow(3).ToVector3();

		myVerticies =
		{
			myPosition + (orientationX * mySize.x) + (orientationY * mySize.y) + (orientationZ * mySize.z),
			myPosition - (orientationX * mySize.x) + (orientationY * mySize.y) + (orientationZ * mySize.z),
			myPosition + (orientationX * mySize.x) - (orientationY * mySize.y) + (orientationZ * mySize.z),
			myPosition - (orientationX * mySize.x) - (orientationY * mySize.y) + (orientationZ * mySize.z),
			myPosition + (orientationX * mySize.x) + (orientationY * mySize.y) - (orientationZ * mySize.z),
			myPosition - (orientationX * mySize.x) + (orientationY * mySize.y) - (orientationZ * mySize.z),
			myPosition + (orientationX * mySize.x) - (orientationY * mySize.y) - (orientationZ * mySize.z),
			myPosition - (orientationX * mySize.x) - (orientationY * mySize.y) - (orientationZ * mySize.z)
		};
	}

	template<class T>
	inline const std::array<Vector3<T>, 8>& OBB3D<T>::GetVerticies() const
	{
		return myVerticies;
	}

	template<class T>
	AABB3D<T> OBB3D<T>::OBB3DToAABB3D() const
	{
		return AABB3D<T>(myPosition - mySize, myPosition + mySize);
	}

	template<class T>
	AABB3D<T> OBB3D<T>::OBB3DToAABB3DLocalSpace() const
	{
		return AABB3D<T>(-mySize, mySize);
	}
}

