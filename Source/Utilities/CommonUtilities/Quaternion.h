#pragma once
#include <corecrt_math_defines.h>

#include "Matrix4x4.hpp"
#include "Vector3.hpp"

namespace CommonUtilities
{
	template<class T>
	class Quaternion
	{
	public:
		T x; //Imaginary
		T y; //Imaginary
		T z; //Imaginary
		T w; //Real

		Quaternion();
		Quaternion(T aX, T aY, T aZ, T aW);
		Quaternion(T aX, T aY, T aZ);
		Quaternion(const Quaternion& aQuaternion);
		explicit Quaternion(const Vector3<T>& aVector);
		explicit Quaternion(const Matrix4x4<T>& aMatrix);

		Quaternion& operator=(const Quaternion& aQuaternion);
		Quaternion& operator=(const Matrix4x4<T>& aMatrix);

		Quaternion operator+(const Quaternion& aQuaternion) const;
		Quaternion operator*(const Quaternion& aQuaternion) const;
		Quaternion operator*(T aScalar) const;
		Quaternion& operator*=(T aScalar);
		Vector3<T> operator*(const Vector3<T>& aVector) const;
		Quaternion& operator*=(const Quaternion& aQuaternion);

		Quaternion& Set(T aX, T aY, T aZ, T aW);
		Quaternion& Set(T aX, T aY, T aZ);
		Quaternion& Set(const Vector3<T>& aVector);
		Quaternion& Set(const Matrix4x4<T>& aMatrix);
		Quaternion& Set(const Quaternion& aQuaternion);
		Quaternion& MakeIdentity();

		T DotProduct(const Quaternion& aQuaternion) const;
		Quaternion& Normalize();
		Quaternion& MakeInverse();
		Quaternion GetInverse() const;

		static Quaternion Lerp(Quaternion aQuaternion1, Quaternion aQuaternion2, float aTime);
		static Quaternion Slerp(Quaternion aQuaternion1, Quaternion aQuaternion2, float aTime, float aThreshold = .05f);

		Matrix4x4<T> GetMatrix() const;
		Vector3<T> GetEuler() const;
	};

	template<class T>
	Quaternion<T>::Quaternion()
	{
		x = static_cast<T>(1);
		y = static_cast<T>(0);
		z = static_cast<T>(0);
		w = static_cast<T>(0);
	}

	template<class T>
	Quaternion<T>::Quaternion(T aX, T aY, T aZ, T aW)
	{
		x = aX;
		y = aY;
		z = aZ;
		w = aW;
	}

	template<class T>
	Quaternion<T>::Quaternion(T aX, T aY, T aZ)
	{
		Set(aX, aY, aZ);
	}

	template<class T>
	Quaternion<T>::Quaternion(const Quaternion<T>& aQuaternion)
	{
		x = aQuaternion.x;
		y = aQuaternion.y;
		z = aQuaternion.z;
		w = aQuaternion.w;
	}

	template<class T>
	Quaternion<T>::Quaternion(const Vector3<T>& aVector)
	{
		Set(aVector.x, aVector.y, aVector.z);
	}

	template <class T>
	Quaternion<T>::Quaternion(const Matrix4x4<T>& aMatrix)
	{
		Set(aMatrix);
	}

	template<class T>
	Quaternion<T>& Quaternion<T>::operator=(const Quaternion<T>& aQuaternion)
	{
		x = aQuaternion.x;
		y = aQuaternion.y;
		z = aQuaternion.z;
		w = aQuaternion.w;
		return *this;
	}

	template <class T>
	Quaternion<T>& Quaternion<T>::operator=(const Matrix4x4<T>& aMatrix)
	{
		return Set(aMatrix);
	}

	template<class T>
	Quaternion<T> Quaternion<T>::operator+(const Quaternion<T>& aQuaternion) const
	{
		return Quaternion(x + aQuaternion.x, y + aQuaternion.y, z + aQuaternion.z, w + aQuaternion.w);
	}

	template<class T>
	Quaternion<T> Quaternion<T>::operator*(const Quaternion<T>& aQuaternion) const
	{
		Quaternion quaternionProduct;

		quaternionProduct.w = (aQuaternion.w * w) - (aQuaternion.x * x) - (aQuaternion.y * y) - (aQuaternion.z * z);
		quaternionProduct.x = (aQuaternion.w * x) + (aQuaternion.x * w) + (aQuaternion.y * z) - (aQuaternion.z * y);
		quaternionProduct.y = (aQuaternion.w * y) + (aQuaternion.y * w) + (aQuaternion.z * x) - (aQuaternion.x * z);
		quaternionProduct.z = (aQuaternion.w * z) + (aQuaternion.z * w) + (aQuaternion.x * y) - (aQuaternion.y * x);

		return quaternionProduct;
	}

	template<class T>
	Quaternion<T> Quaternion<T>::operator*(T aScalar) const
	{
		return Quaternion(aScalar * x, aScalar * y, aScalar * z, aScalar * w);
	}

	template<class T>
	Quaternion<T>& Quaternion<T>::operator*=(T aScalar)
	{
		x *= aScalar;
		y *= aScalar;
		z *= aScalar;
		w *= aScalar;
		return *this;
	}

	template <class T>
	Vector3<T> Quaternion<T>::operator*(const Vector3<T>& aVector) const
	{
		Vector3<T> vector2, vector3;
		Vector3<T> qVector(x, y, z);
		vector2 = qVector.Cross(aVector);
		vector3 = qVector.Cross(vector2);
		vector2 *= static_cast<T>(2) * w;
		vector3 *= static_cast<T>(2);

		return aVector + vector2 + vector3;
	}

	template<class T>
	Quaternion<T>& Quaternion<T>::operator*=(const Quaternion<T>& aQuaternion)
	{
		return (*this = aQuaternion * (*this));
	}

	template<class T>
	Quaternion<T>& Quaternion<T>::Set(T aX, T aY, T aZ, T aW)
	{
		x = aX;
		y = aY;
		z = aZ;
		w = aW;
		return *this;
	}

	template<class T>
	Quaternion<T>& Quaternion<T>::Set(T aX, T aY, T aZ)
	{
		/*double angle = static_cast<double>(aX * static_cast<T>(0.5));
		const double sinX = sin(angle);
		const double cosX = cos(angle);

		angle = static_cast<double>(aY * static_cast<T>(0.5));
		const double sinY = sin(angle);
		const double cosY = cos(angle);

		angle = static_cast<double>(aZ * static_cast<T>(0.5));
		const double sinZ = sin(angle);
		const double cosZ = cos(angle);

		const double cYcZ = cosY * cosZ;
		const double sYcZ = sinY * cosZ;
		const double cYsZ = cosY * sinZ;
		const double sYsZ = sinY * sinZ;*/

		const T s1 = static_cast<T>(sin(aX * 0.5));
		const T s2 = static_cast<T>(sin(aY * 0.5));
		const T s3 = static_cast<T>(sin(aZ * 0.5));
		const T c1 = static_cast<T>(cos(aX * 0.5));
		const T c2 = static_cast<T>(cos(aY * 0.5));
		const T c3 = static_cast<T>(cos(aZ * 0.5));
		 x = s1 * c2 * c3 + c1 * s2 * s3;
        y = c1 * s2 * c3 + s1 * c2 * s3;
        z = c1 * c2 * s3 - s1 * s2 * c3;
        w = c1 * c2 * c3 - s1 * s2 * s3;
		//Normalize();
		return *this;
	}

	template<class T>
	Quaternion<T>& Quaternion<T>::Set(const Vector3<T>& aVector)
	{
		return Set(aVector.x, aVector.y, aVector.z);
	}

	template <class T>
	Quaternion<T>& Quaternion<T>::Set(const Matrix4x4<T>& aMatrix)
	{

		T trace = aMatrix(1, 1) + aMatrix(2, 2) + aMatrix(3, 3);
		if (trace > 0)
		{
			T s = std::sqrt(trace + T(1.0)) * T(2.0); // s = 4 * qw
			w = T(0.25) * s;
			x = (aMatrix(3, 2) - aMatrix(2, 3)) / s;
			y = (aMatrix(1, 3) - aMatrix(3, 1)) / s;
			z = (aMatrix(2, 1) - aMatrix(1, 2)) / s;
		}
		else if ((aMatrix(1, 1) > aMatrix(2, 2)) && (aMatrix(1, 1) > aMatrix(3, 3)))
		{
			T s = T(std::sqrt(1.0 + aMatrix(1, 1) - aMatrix(2, 2) - aMatrix(3, 3)) * 2.0); // s = 4 * qx
			w = (aMatrix(3, 2) - aMatrix(2, 3)) / s;
			x = T(0.25) * s;
			y = (aMatrix(1, 2) + aMatrix(2, 1)) / s;
			z = (aMatrix(1, 3) + aMatrix(3, 1)) / s;
		}
		else if (aMatrix(2, 2) > aMatrix(3, 3))
		{
			T s = T(std::sqrt(T(1.0) + aMatrix(2, 2) - aMatrix(1, 1) - aMatrix(3, 3)) * T(2.0)); // s = 4 * qy
			w = (aMatrix(1, 3) - aMatrix(3, 1)) / s;
			x = (aMatrix(1, 2) + aMatrix(2, 1)) / s;
			y = T(0.25) * s;
			z = (aMatrix(2, 3) + aMatrix(3, 2)) / s;
		}
		else
		{
			T s = T(std::sqrt(T(1.0) + aMatrix(3, 3) - aMatrix(1, 1) - aMatrix(2, 2)) * T(2.0)); // s = 4 * qz
			w = (aMatrix(2, 1) - aMatrix(1, 2)) / s;
			x = (aMatrix(1, 3) + aMatrix(3, 1)) / s;
			y = (aMatrix(2, 3) + aMatrix(3, 2)) / s;
			z = T(0.25) * s;
		}

		//w = sqrt(1 + aMatrix(1, 1) + aMatrix(2, 2) + aMatrix(3, 3)) * 0.5f;
		//T w4 = T(4) * w;
		//if(abs(w4) <= T(0.0000001))
		//{
		//	
		//}
		//x = (aMatrix(3, 2) - aMatrix(2, 3)) / w4;
		//y = (aMatrix(1, 3) - aMatrix(3, 1)) / w4;
		//z = (aMatrix(2, 1) - aMatrix(1, 2)) / w4;
		//qw = sqrt(1 + m00 + m11 + m22) / 2
		//	qx = (m21 - m12) / (4 * qw)
		//	qy = (m02 - m20) / (4 * qw)
		//	qz = (m10 - m01) / (4 * qw)




/*#define SIGN(x) return (x) >= 0 ? 1:-1;
#define NORM(a, b, c, d) return sqrt((a) * (a) + (b) * (b) + (c) * (c) + (d) * (d));*/ 


		//float r11 = m.at<float>(0, 0);
		//float r12 = m.at<float>(0, 1);
		//float r13 = m.at<float>(0, 2);
		//float r21 = m.at<float>(1, 0);
		//float r22 = m.at<float>(1, 1);
		//float r23 = m.at<float>(1, 2);
		//float r31 = m.at<float>(2, 0);
		//float r32 = m.at<float>(2, 1);
		//float r33 = m.at<float>(2, 2);
		//float q0 = (r11 + r22 + r33 + 1.0f) / 4.0f;
		//float q1 = (r11 - r22 - r33 + 1.0f) / 4.0f;
		//float q2 = (-r11 + r22 - r33 + 1.0f) / 4.0f;
		//float q3 = (-r11 - r22 + r33 + 1.0f) / 4.0f;
		//if (q0 < 0.0f) {
		//	q0 = 0.0f;
		//}
		//if (q1 < 0.0f) {
		//	q1 = 0.0f;
		//}
		//if (q2 < 0.0f) {
		//	q2 = 0.0f;
		//}
		//if (q3 < 0.0f) {
		//	q3 = 0.0f;
		//}
		//q0 = sqrt(q0);
		//q1 = sqrt(q1);
		//q2 = sqrt(q2);
		//q3 = sqrt(q3);
		//if (q0 >= q1 && q0 >= q2 && q0 >= q3) {
		//	q0 *= +1.0f;
		//	q1 *= SIGN(r32 - r23);
		//	q2 *= SIGN(r13 - r31);
		//	q3 *= SIGN(r21 - r12);
		//}
		//else if (q1 >= q0 && q1 >= q2 && q1 >= q3) {
		//	q0 *= SIGN(r32 - r23);
		//	q1 *= +1.0f;
		//	q2 *= SIGN(r21 + r12);
		//	q3 *= SIGN(r13 + r31);
		//}
		//else if (q2 >= q0 && q2 >= q1 && q2 >= q3) {
		//	q0 *= SIGN(r13 - r31);
		//	q1 *= SIGN(r21 + r12);
		//	q2 *= +1.0f;
		//	q3 *= SIGN(r32 + r23);
		//}
		//else if (q3 >= q0 && q3 >= q1 && q3 >= q2) {
		//	q0 *= SIGN(r21 - r12);
		//	q1 *= SIGN(r31 + r13);
		//	q2 *= SIGN(r32 + r23);
		//	q3 *= +1.0f;
		//}
		//else {
		//	printf("coding error\n");
		//}
		//float r = NORM(q0, q1, q2, q3);
		//q0 /= r;
		//q1 /= r;
		//q2 /= r;
		//q3 /= r;

		//Mat res = (Mat_<float>(4, 1) << q0, q1, q2, q3);
		//return res;


		//if(aMatrix(3,2) < T(0))
		//{
		//	if(aMatrix(1,1) > aMatrix(2,2))
		//	{
		//		const T t = 1 + aMatrix(1,1) - aMatrix(2,2) - aMatrix(3,3);
		//		if(t == 0)
		//		{
		//			return MakeIdentity();
		//		}
		//		*this = Quaternion(t, aMatrix(2,1) + aMatrix(1,2), aMatrix(1,3)+ aMatrix(3,1), aMatrix(3,2) - aMatrix(2,3)) * (0.5 / sqrt(t));
		//	}
		//	else
		//	{
		//		const T t = T(1.0) - aMatrix(1,1) + aMatrix(2,2) - aMatrix(3,3);
		//		if(t == 0)
		//		{
		//			return MakeIdentity();
		//		}
		//		*this = Quaternion<T>(aMatrix(2,1) + aMatrix(1,2), t, aMatrix(3,2) + aMatrix(2,3), aMatrix(1,3) - aMatrix(3,1)) * (T(0.5) / sqrt(t));
		//	}
		//}
		//else
		//{
		//	if(aMatrix(1,1) < -aMatrix(2,2))
		//	{
		//		const T t = T(1.0) - aMatrix(1,1) - aMatrix(2,2) + aMatrix(3,3);
		//		if(t == 0)
		//		{
		//			return MakeIdentity();
		//		}
		//		*this = Quaternion<T>(aMatrix(1,3) + aMatrix(3,1), aMatrix(3,2) + aMatrix(2,3), t, aMatrix(2,1) - aMatrix(1,2)) * (T(0.5) / sqrt(t));
		//	}
		//	else
		//	{
		//		//     const Type t = Type(1.0) + m.column0.x + m.column1.y + m.column2.z;
		//		//     *this = PxQuatT<Type>(m.column1.z - m.column2.y, m.column2.x - m.column0.z, m.column0.y - m.column1.x, t) * (Type(0.5) / PxSqrt(t));
		//
		//		const T t = T(1.0) + aMatrix(1,1) + aMatrix(2,2) + aMatrix(3,3);
		//		if(t == 0)
		//		{
		//			return MakeIdentity();
		//		}
		//		*this = Quaternion<T>(aMatrix(3,2) - aMatrix(2,3), aMatrix(1,3) - aMatrix(3,1), aMatrix(1,2) - aMatrix(2,1), t) * (T(0.5) / sqrt(t));
		//	}
		//}



		//////TODO: Very slow
		//const T diag = aMatrix(1, 1) + aMatrix(2, 2) + aMatrix(3, 3) + 1;
		//if (diag > static_cast<T>(0))
		//{
		//	const T scale = sqrt(diag) * static_cast<T>(2);
		//	x = (aMatrix(2, 3) - aMatrix(3, 2)) / scale;
		//	y = (aMatrix(3, 1) - aMatrix(1, 3)) / scale;
		//	z = (aMatrix(1, 2) - aMatrix(2, 1)) / scale;
		//	w = static_cast<T>(0.25) * scale;
		//}
		//else
		//{
		//	if (aMatrix(1, 1) > aMatrix(2, 2) && aMatrix(1, 1) > aMatrix(3, 3))
		//	{
		//		const T scale = sqrt(static_cast<T>(1) + aMatrix(1, 1) - aMatrix(2, 2) - aMatrix(3, 3)) * static_cast<T>(2);
		//		x = static_cast<T>(0.25) * scale;
		//		y = (aMatrix(2, 1) + aMatrix(1, 2)) / scale;
		//		z = (aMatrix(1, 3) + aMatrix(3, 1)) / scale;
		//		w = (aMatrix(2, 3) - aMatrix(3, 2)) / scale;
		//	}
		//	else if (aMatrix(2, 2) > aMatrix(3, 3))
		//	{
		//		const T scale = sqrt(static_cast<T>(1) + aMatrix(2, 2) - aMatrix(1, 1) - aMatrix(3, 3)) * static_cast<T>(2);
		//		x = (aMatrix(2, 1) + aMatrix(1, 2)) / scale;
		//		y = static_cast<T>(0.25) * scale;
		//		z = (aMatrix(3, 2) + aMatrix(2, 3)) / scale;
		//		w = (aMatrix(3, 1) - aMatrix(1, 3)) / scale;
		//	}
		//	else
		//	{
		//		const T scale = sqrt(static_cast<T>(1) + aMatrix(3, 3) - aMatrix(1, 1) - aMatrix(2, 2)) * static_cast<T>(2);
		//		x = (aMatrix(3, 1) + aMatrix(1, 3)) / scale;
		//		y = (aMatrix(3, 2) + aMatrix(2, 3)) / scale;
		//		z = static_cast<T>(0.25) * scale;
		//		w = (aMatrix(1, 2) - aMatrix(2, 1)) / scale;
		//	}
		//}
		//return Normalize();
		return *this;
	}

	template<class T>
	Quaternion<T>& Quaternion<T>::Set(const Quaternion<T>& aQuaternion)
	{
		return Set(aQuaternion.x, aQuaternion.y, aQuaternion.z, aQuaternion.w);
	}

	template <class T>
	Quaternion<T>& Quaternion<T>::MakeIdentity()
	{
		x = static_cast<T>(0);
		y = static_cast<T>(0);
		z = static_cast<T>(0);
		w = static_cast<T>(1);
		return *this;
	}

	template <class T>
	T Quaternion<T>::DotProduct(const Quaternion<T>& aQuaternion) const
	{
		return (x * aQuaternion.x) + (y * aQuaternion.y) + (z * aQuaternion.z) + (w * aQuaternion.w);
	}

	template<class T>
	Quaternion<T>& Quaternion<T>::Normalize()
	{
		const T lengthSqr = x * x + y * y + z * z + w * w;
		return *this *= static_cast<T>(1.0 / sqrt(lengthSqr));
	}

	template<class T>
	Quaternion<T>& Quaternion<T>::MakeInverse()
	{
		x = -x;
		y = -y;
		z = -z;
		return *this;
	}

	template <class T>
	Quaternion<T> Quaternion<T>::GetInverse() const
	{
		return Quaternion(-x, -y, -z, w);
	}

	template <class T>
	Quaternion<T> Quaternion<T>::Lerp(Quaternion aQuaternion1, Quaternion aQuaternion2, float aTime)
	{
		const T scale = static_cast<T>(1.0f - aTime);
		return (aQuaternion1 * scale) + (aQuaternion2 * static_cast<T>(aTime));
	}

	template <class T>
	Quaternion<T> Quaternion<T>::Slerp(Quaternion aQuaternion1, Quaternion aQuaternion2, float aTime, float aThreshold)
	{
		double angle = static_cast<double>(aQuaternion1.DotProduct(aQuaternion2));
		if (angle < 0.0)
		{
			aQuaternion1 *= -1.0;
			angle *= -1.0;
		}

		if (angle <= static_cast<double>(1.0f - aThreshold))
		{
			const double theta = acos(angle);
			const double invSinTheta = 1.0 / sin(theta);
			const double scale = sin(theta * static_cast<double>(1.0f - aTime)) * invSinTheta;
			const double invScale = sin(theta * static_cast<double>(aTime)) * invSinTheta;
			return (aQuaternion1 * static_cast<T>(scale)) + (aQuaternion2 * static_cast<T>(invScale));
		}
		return Lerp(aQuaternion1, aQuaternion2, aTime);
	}

	template<class T>
	Matrix4x4<T> Quaternion<T>::GetMatrix() const
	{
		//Apparently proper way but since time is slim the wrong way that we worked around will have to do for now
		T x2 = x+x;
		T y2 = y+y;
		T z2 = z+z;

		T xx = x*x2;
		T xy = x*y2;
		T xz = x*z2;
		T yy = y*y2;
		T yz = y*z2;
		T zz = z*z2;
		T wx = w*x2;
		T wy = w*y2;
		T wz = w*z2;

		Matrix4x4f outMatrix = Matrix4x4f();

		/*outMatrix(1, 1) = static_cast<T>(1) - static_cast<T>(2) * y * y - static_cast<T>(2) * z * z;
		outMatrix(1, 2) = static_cast<T>(2) * x * y + static_cast<T>(2) * z * w;
		outMatrix(1, 3) = static_cast<T>(2) * x * z - static_cast<T>(2) * y * w;
		outMatrix(1, 4) = static_cast<T>(0);

		outMatrix(2, 1) = static_cast<T>(2) * x * y - static_cast<T>(2) * z * w;
		outMatrix(2, 2) = static_cast<T>(1) - static_cast<T>(2) * x * x - static_cast<T>(2) * z * z;
		outMatrix(2, 3) = static_cast<T>(2) * z * y + static_cast<T>(2) * x * w;
		outMatrix(2, 4) = static_cast<T>(0);

		outMatrix(3, 1) = static_cast<T>(2) * x * z + static_cast<T>(2) * y * w;
		outMatrix(3, 2) = static_cast<T>(2) * z * y - static_cast<T>(2) * x * w;
		outMatrix(3, 3) = static_cast<T>(1) - static_cast<T>(2) * x * x - static_cast<T>(2) * y * y;*/
		outMatrix(1,1) = (T(1) - (yy + zz));
		outMatrix(2, 1) = (xy + wz);// * sx;
		outMatrix(3, 1) = (xz - wy);// * sx;
				
		outMatrix(1, 2) = (xy - wz);// * sy;
        outMatrix(2,2) = (T(1) - (xx + zz));
		outMatrix(3, 2) = (yz + wx);// *sy;
				
		outMatrix(1, 3) = (xz + wy);// *sz;
		outMatrix(2, 3) = (yz - wx);// *sz;
        outMatrix(3,3) = (T(1) - (xx + yy));

        outMatrix(1,4) = T(0);
        outMatrix(2,4) = T(0);
        outMatrix(3,4) = T(0);
		outMatrix(4,4) = T(1);

		return outMatrix;
	}

	template <class T>
	Vector3<T> Quaternion<T>::GetEuler() const
	{
		//yaw pitch roll
		Vector3<T> angles;

    // roll (x-axis rotation)
	    double sinr_cosp = 2 * (w * x + y * z);
	    double cosr_cosp = 1 - 2 * (x * x + y * y);
	    angles[2] = (T)std::atan2(sinr_cosp, cosr_cosp);

	    // pitch (y-axis rotation)
	    double sinp = 2 * (w * y - z * x);
	    if (std::abs(sinp) >= 1)
	        angles[1] = (T)std::copysign(M_PI / 2, sinp); // use 90 degrees if out of range
	    else
	        angles[1] = (T)std::asin(sinp);

	    // yaw (z-axis rotation)
	    double siny_cosp = 2 * (w * z + x * y);
	    double cosy_cosp = 1 - 2 * (y * y + z * z);
	    angles[0] = (T)std::atan2(siny_cosp, cosy_cosp);
	    return angles;
	}

	typedef Quaternion<float> Quaternionf;
	typedef Quaternion<double> Quaterniond;

	template<class T>
	inline Vector3<T> Matrix3x3<T>::GetXYZRotation(Matrix3x3<T> aMatrix)
	{
		Quaternionf quaternion;
		quaternion.Set(aMatrix);
		Vector3f euler = quaternion.GetEuler();
		return euler;
	}

};
namespace CU = CommonUtilities;
