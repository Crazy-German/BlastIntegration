#pragma once
#include "Matrix4x4.hpp"
#include "Vector3.hpp"

namespace CommonUtilities
{
	template <class T>
	class Capsule
	{
	public:
		Capsule();
		Capsule(const Vector3<T>& aPosition, T aRadius, T aHeight);
		Capsule(const Vector3<T>& aPosition, T aRadius, T aHeight, const Matrix4x4<T>& aRotationMatrix);

		void Init(const Vector3<T>& aPosition, T aRadius, T aHeight, const Matrix4x4<T>& aRotationMatrix);

		void Awake(const Vector3<T>& aPosition, T aRadius, T aHeight);
		void Awake(const Vector3<T>& aPosition, T aRadius, T aHeight, const Matrix4x4<T>& aRotationMatrix);

		const Vector3<T>& GetPosition() const;
		const Vector3<T>& GetBase() const;
		const Vector3<T>& GetTip() const;
		const T GetRadius() const;
		const T GetHeight() const;
		const Matrix4x4<T>& GetRotationMatrix() const;

		void SetPosition(const Vector3<T>& aPosition);
		void SetRadius(const T& aRadius);
		void SetRotationMatrix(const Matrix4x4<T> aRotationMatrix);

		//const Vector3<T> GetFirstClosePoint(const Capsule& aCapsule);
	private:
		void UpdateBaseAndTip();
		Vector3<T> myPosition;
		Vector3<T> myBase;
		Vector3<T> myTip;

		T myHeight;
		T myRadius;
		Matrix4x4<T> myRotationMatrix;
	};

	template <class T>
	inline Capsule<T>::Capsule()
	{
		myPosition = Vector3<T>();
		myRotationMatrix = Matrix4x4<T>();
	}

	template <class T>
	inline Capsule<T>::Capsule(const Vector3<T>& aPosition, T aRadius, T aHeight)
		:myPosition(aPosition), myRadius(aRadius), myHeight(aHeight)
	{
		UpdateBaseAndTip();
	}

	template <class T>
	inline Capsule<T>::Capsule(const Vector3<T>& aPosition, T aRadius, T aHeight, const Matrix4x4<T>& aRotationMatrix)
		:myPosition(aPosition), myRadius(aRadius), myHeight(aHeight), myRotationMatrix(aRotationMatrix)
	{
		UpdateBaseAndTip();
	}

	template <class T>
	void Capsule<T>::Init(const Vector3<T>& aPosition, T aRadius, T aHeight, const Matrix4x4<T>& aRotationMatrix)
	{
		myPosition = aPosition;
		myRadius = aRadius;
		myHeight = aHeight;
		myRotationMatrix = aRotationMatrix;
		UpdateBaseAndTip();
	}


	template <class T>
	inline void Capsule<T>::Awake(const Vector3<T>& aPosition, T aRadius, T aHeight)
	{
		myPosition = aPosition;
		myHeight = aHeight;
		myRadius = aRadius;
		UpdateBaseAndTip();
	}

	template <class T>
	inline void Capsule<T>::Awake(const Vector3<T>& aPosition, T aRadius, T aHeight, const Matrix4x4<T>& aRotationMatrix)
	{
		myPosition = aPosition;
		myHeight = aHeight;
		myRadius = aRadius;
		myRotationMatrix = aRotationMatrix;
		UpdateBaseAndTip();
	}

	template <class T>
	inline const Vector3<T>& Capsule<T>::GetPosition() const
	{
		return myPosition;
	}

	template <class T>
	const Vector3<T>& Capsule<T>::GetBase() const
	{
		return myBase;
	}

	template <class T>
	const Vector3<T>& Capsule<T>::GetTip() const
	{
		return myTip;
	}

	template <class T>
	const T Capsule<T>::GetRadius() const
	{
		return myRadius;
	}

	template <class T>
	const T Capsule<T>::GetHeight() const
	{
		return myHeight;
	}

	template <class T>
	inline const Matrix4x4<T>& Capsule<T>::GetRotationMatrix() const
	{
		return myRotationMatrix;
	}

	template <class T>
	inline void Capsule<T>::SetPosition(const Vector3<T>& aPosition)
	{
		myPosition = aPosition;
	}

	template <class T>
	inline void Capsule<T>::SetRadius(const T& aRadius)
	{
		myRadius = aRadius;
	}

	template <class T>
	inline void Capsule<T>::SetRotationMatrix(const Matrix4x4<T> aRotationMatrix)
	{
		myRotationMatrix = aRotationMatrix;
		UpdateBaseAndTip();
	}

	template <class T>
	inline void Capsule<T>::UpdateBaseAndTip()
	{
		myTip = myPosition + myRotationMatrix.GetRow(2).ToVector3() * myHeight/static_cast<T>(2)*myRadius;
		myBase = myPosition - myRotationMatrix.GetRow(2).ToVector3() * myHeight/static_cast<T>(2)*myRadius;
	}
}
