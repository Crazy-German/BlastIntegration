#pragma once
#include "Matrix.hpp"

namespace CommonUtilities
{

	template <class T>
	class Camera
	{
	public:
		Camera();
		Camera(const T& aHorizontalFOV, const T& aAspectRatio, const T& aNearPlane, const T& aFarPlane);
		~Camera();

		Vector4<T> WorldSpaceToPostProjectionSpace(Vector3<T>& aWorldSpacePostion);
		Vector3<T> ToCameraSpace(Vector3<T>& aWorldSpacePostion);
		Vector3<T> ToWorldSpace(Vector3<T>& aCameraSpacePostion);
		void LookAt(const Vector3<T>& aFrom, const Vector3<T>& aTo, const Vector3<T>& aUp);
		

	private:
		Matrix4x4<T> myTransform;
		Matrix4x4<T> myClipMatrix;
	};

	template<class T>
	inline Camera<T>::Camera()
	{
	}

	template<class T>
	inline Camera<T>::Camera(const T& aHorizontalFOVdeg, const T& aAspectRatio, const T& aNearPlane, const T& aFarPlane)
	{
		myTransform = Matrix4x4<T>();

		T FOVRad = (aHorizontalFOVdeg * (T)3.1415) / (T)180;
		T FOV = (T)1 / (tan(FOVRad/2));
		T depth = aFarPlane / (aFarPlane - aNearPlane);

		myClipMatrix = Matrix4x4<T>
		(
			Vector4<T>(FOV, (T)0, (T)0, (T)0),
			Vector4<T>((T)0, aAspectRatio * FOV, (T)0, (T)0),
			Vector4<T>((T)0, (T)0, depth, 1),
			Vector4<T>((T)0, (T)0, -aNearPlane * depth, (T)0)
		);
	}

	template<class T>
	inline Camera<T>::~Camera()
	{
	}

	template<class T>
	inline Vector4<T> Camera<T>::WorldSpaceToPostProjectionSpace(Vector3<T>& aWorldSpacePostion)
	{
		Vector4<T> worldSpace = aWorldSpacePostion.ToVector4();
		worldSpace.w = (T)1;

		Vector4<T> cameraSpace = worldSpace * myTransform.GetFastInverse();
		Vector4<T> clipSpace = cameraSpace * myClipMatrix;

		Vector4<T> postProjectionPosition(clipSpace.x / clipSpace.w, clipSpace.y / clipSpace.w, clipSpace.z / clipSpace.w, clipSpace.w);
		return postProjectionPosition;
	}

	template<class T>
	inline Vector3<T> Camera<T>::ToCameraSpace(Vector3<T>& aWorldSpacePostion)
	{
		return (aWorldSpacePostion.ToVector4() * myTransform.GetFastInverse()).ToVector3();
	}

	template<class T>
	inline Vector3<T> Camera<T>::ToWorldSpace(Vector3<T>& aCameraSpacePostion)
	{
		return (aCameraSpacePostion.ToVector4() * myTransform).ToVector3();
	}

	template<class T>
	inline void Camera<T>::LookAt(const Vector3<T>& aFrom, const Vector3<T>& aTo, const Vector3<T>& aUp)
	{
		myTransform = Matrix4x4<T>::CreateLookAt(aFrom, aTo, aUp);
	}
}